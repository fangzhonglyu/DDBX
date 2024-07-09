import functions_framework
from google.cloud import spanner
from google.api_core.exceptions import AlreadyExists
from flask import jsonify
import jwt, datetime, os, bcrypt, uuid, base64, time


#==============================================================================
# Configurations
#==============================================================================

instance_id = "ddbx"
database_id = "test_player_info"

# Initialize Google Cloud Spanner API
client = spanner.Client()
instance = client.instance(instance_id)
database = instance.database(database_id)

# Configure JWT Key from environment variable
SECRET_KEY = os.getenv('JWT_PRIVATE_KEY')

# Use manual HTTP error responses and not Flask's default HTML responses
RESPONSE400 = jsonify({"error": "Bad Request"})
RESPONSE400.status_code = 400

RESPONSE401 = jsonify({"error": "Unauthorized"})
RESPONSE401.status_code = 401

RESPONSE404 = jsonify({"error": "Not Found"})
RESPONSE404.status_code = 404

RESPONSE405 = jsonify({"error": "Method Not Allowed"})
RESPONSE405.status_code = 405

RESPONSE409 = jsonify({"error": "Conflict"})
RESPONSE409.status_code = 409

RESPONSE500 = jsonify({"error": "Internal Server Error"})
RESPONSE500.status_code = 500

RESPONSE501 = jsonify({"error": "Not Implemented"})
RESPONSE501.status_code = 501


#==============================================================================
# Shared Helper Functions
#==============================================================================

def generate_token(playerUUID):
    """Generates a JWT token for the given playerUUID for auth purposes.
    Args:
        playerUUID (str): The UUID of the player (UUID4 format)
    Returns:
        str: The JWT token
    """
    payload = {
        "exp": datetime.datetime.utcnow() + datetime.timedelta(days=1),
        "iat": datetime.datetime.utcnow(),
        "sub": playerUUID,
    }
    return jwt.encode(payload, SECRET_KEY, algorithm="HS256")

def request_to_string(req):
    """Converts a Flask request object to a string for logging.
    Args:
        req (flask.Request): The request object.
    Returns:
        str: The string representation of the request.
    """
    parts = [
        f"Method: {req.method}",
        f"URL: {req.url}",
        f"Headers: {req.headers}",
        f"Body: {req.get_data(as_text=True)}",  # Convert bytes to string
    ]
    return "\n".join(parts)
    
def log_error(entry_point, request, error_msg):
    """Logs an error message and the request that caused it.
    Args:
        entry_point (str): The name of the entry point
        request (flask.Request): The request object
        error_msg (str): The error message
    """
    print("Error in " + entry_point + ": " + error_msg)
    print("============BEGIN REQUEST DUMP============")
    print(request_to_string(request))
    print("=============END REQUEST DUMP=============\n")
    
#==============================================================================
# Main Entry Point
#==============================================================================

@functions_framework.http
def process_request(request):
    """HTTP Cloud Function for processing a request to the DDBX database system.
    Args:
        request (flask.Request): The request object.
        <https://flask.palletsprojects.com/en/1.1.x/api/#incoming-request-data>
    Returns:
        The response text, or any set of values that can be turned into a
        Response object using `make_response`
        <https://flask.palletsprojects.com/en/1.1.x/api/#flask.make_response>.
    """
    
    request_path = request.path

    #Process different sub-entry points
    if request_path == "/login":
        return login(request)
    elif request_path == "/signup":
        return signup(request)
    elif request_path == "/balance":
        return balance(request)
    else:
        return RESPONSE404
    
#==============================================================================
# Endpoint Specific Helper Functions
#==============================================================================
    
def gen_login_response(playerID, playerUUID):
    """Generates JSON for a successful login response.
    Args:
        playerID (str): The player's username
        playerUUID (str): The player's UUID
    Returns:
        JSON: The response JSON.
    """
    token = generate_token(playerUUID)
    return jsonify(
        {
            "playerID": playerID,
            "playerUUID": playerUUID,
            "token": token
        }
    )
    
def gen_signup_response(playerID, playerUUID):
    """Generates JSON for a successful signup response.
    Args:
        playerID (str): The player's username
        playerUUID (str): The player's UUID
    Returns:
        JSON: The response JSON.
    """
    return jsonify(
        {
            "playerID": playerID,
            "playerUUID": playerUUID
        }
    )

#==============================================================================
# Endpoints
#==============================================================================

@functions_framework.http
def login(request):
    """Login endpoint for DDBX. Authenticates a user and returns a JWT token.
    Args:
        request (flask.Request): The request object.
    Returns:
        Either a JSON response or an error response. See `gen_login_response`.
    """
    
    # time the request
    t1, t2, t3, t4 = time.time(), None, None, None
    
    # Check for valid request method and JSON
    if request.method != "POST":
        return RESPONSE405
    request_json = request.get_json(silent=True)
    if not request_json:
        return RESPONSE400
    
    # Extract username and password from JSON
    username = request_json.get('username')
    password = request_json.get('password')
    if not username or not password:
        return RESPONSE400
    
    t2 = time.time()
    # Perform the login process
    try:
        with database.snapshot() as snapshot:
            # Query the database for the user's UUID and password hash
            sql = "SELECT playerUUID, passHash FROM players WHERE playerID = @username"
            params = {"username": username}
            param_types = {"username": spanner.param_types.STRING}
            results = snapshot.execute_sql(sql, params=params, param_types=param_types)
            
            
            users = list(results)
            
             # Check if the user exists
            if len(users) != 1:
                return RESPONSE401
            
            playerUUID, passHash = users[0]
            
            
            b64Hash = base64.b64decode(passHash)
            t3 = time.time()
           
            # Check if the password is correct
            if bcrypt.checkpw(password.encode('utf-8'), b64Hash):
                t4 = time.time()
                print(f"Time to execute SQL: {t3-t2}")
                print(f"Time to check password: {t4-t3}")
                print(f"Total time: {t4-t1}")
                print(f"setup time: {t2-t1}")
                return gen_login_response(username, playerUUID)
            else:
                return RESPONSE401      
    except Exception as e:
        # if not isinstance(e, HTTPException):
        #     # Internal error occurred, log the error and return a 500 response
        log_error("login", request, str(e))
        return RESPONSE500


@functions_framework.http
def signup(request):
    """Signup endpoint for DDBX. Registers a new user in the database.
    Args:
        request (flask.Request): The request object.
    Returns:
        Either a JSON response or an error response. See `gen_signup_response`.
    """
    
    # Check for valid request method and JSON
    if request.method != "POST":
        return RESPONSE405
    request_json = request.get_json(silent=True)
    if not request_json:
        return RESPONSE400
    
    # Extract username and password from JSON
    username = request_json.get('username')
    password = request_json.get('password')
    if not username or not password:
        return RESPONSE400
    
    # Generate a UUID and hash the password
    passHash = bcrypt.hashpw(password.encode('utf-8'), bcrypt.gensalt())
    passHashbase64 = base64.b64encode(passHash)
    playerUUID = str(uuid.uuid4())
    
    # Perform the signup process
    try:
        # Use a batch to insert the new user into the database
        with database.batch() as batch:
            batch.insert(table="players", columns=("playerUUID","playerID", "passHash"), values=[(playerUUID, username, passHashbase64)])
        return jsonify({"message": "User signup successful."})
    # Return a 409 error if the username already exists
    except AlreadyExists as e:
        return RESPONSE409
    except Exception as e:
        # Internal error occurred, log the error and return a 500 response
        log_error("signup", request, str(e))
        return RESPONSE500

@functions_framework.http
def balance(request):
    # Check for valid request method and JSON
    if "Authorization" not in request.headers:
        return RESPONSE400
     
    token = request.headers["Authorization"].split(" ")[1]
    print(token)
        
    # decode the token
    try:
        payload = jwt.decode(token, SECRET_KEY, algorithms=["HS256"])
    except jwt.ExpiredSignatureError:
        return RESPONSE401
    except jwt.InvalidTokenError:
        return RESPONSE401
    
    if request.method == "POST":
        #update the balance
        request_json = request.get_json(silent=True)
        if not request_json:
            return RESPONSE400
        balance = request_json.get('balance')
        if not balance:
            return RESPONSE400
        
        try:
            def update_balance(transaction):
                # Query the database for the user's UUID and password hash
                sql = "UPDATE players SET balance = @balance WHERE playerUUID = @playerUUID"
                params = {"playerUUID": payload["sub"], "balance": balance}
                param_types = {"playerUUID": spanner.param_types.STRING, "balance": spanner.param_types.INT64}
                transaction.execute_update(sql, params=params, param_types=param_types)

            database.run_in_transaction(update_balance)
            return jsonify({"message": "Balance updated successfully."})
        except Exception as e:
            # Internal error occurred, log the error and return a 500 response
            log_error("balance", request, str(e))
            return RESPONSE500

    # get the balance of the player
    try:
        with database.snapshot() as snapshot:
            # Query the database for the user's UUID and password hash
            sql = "SELECT balance FROM players WHERE playerUUID = @playerUUID"
            params = {"playerUUID": payload["sub"]}
            param_types = {"playerUUID": spanner.param_types.STRING}
            results = snapshot.execute_sql(sql, params=params, param_types=param_types)
            
            users = list(results)
            
            # Check if the user exists
            if len(users) != 1:
                return RESPONSE401
            
            balance = users[0][0]
            
            return jsonify({"balance": balance})
    except Exception as e:
        # Internal error occurred, log the error and return a 500 response
        log_error("balance", request, str(e))
        return RESPONSE500
    
@functions_framework.http
def lobby(request):
    """lobby endpoint for DDBX. Directs clients to lobby servers
    Args:
        request (flask.Request): The request object.
    Returns:
        Either a JSON response or an error response. See `gen_signup_response`.
    """
    # Check for valid request method and JSON
    if "Authorization" not in request.headers:
        return RESPONSE400
    
    token = request.headers["Authorization"].split(" ")[1]
    print(token)
        
    # decode the token
    try:
        payload = jwt.decode(token, SECRET_KEY, algorithms=["HS256"])
    except jwt.ExpiredSignatureError:
        return RESPONSE401
    except jwt.InvalidTokenError:
        return RESPONSE401
    
    if request.method != "POST":
        return RESPONSE400
    
    request_json = request.get_json(silent=True)
    if not request_json:
        return RESPONSE400
    roomid = request_json.get('roomid')
    
    try:
        # Use a batch to insert the new user into the database
        with database.snapshot() as snapshot:
            # Query the database for the user's UUID and password hash
            sql = "SELECT * FROM lobbies WHERE lobbyID = @roomid"
            params = {"roomid": roomid}
            param_types = {"roomid": spanner.param_types.INT64}
            results = snapshot.execute_sql(sql, params=params, param_types=param_types)
            
            rooms = list(results)
            
            if len(rooms) != 1:
                return RESPONSE404
            
            return jsonify({"message": "Room found.", "lobbyServer": rooms[0]['server']})
    except Exception as e:
        # Internal error occurred, log the error and return a 500 response
        log_error("lobby", request, str(e))
        return RESPONSE500
    
    
        
    