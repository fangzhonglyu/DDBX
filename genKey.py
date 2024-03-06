import os
import base64

# Generate a 256-bit (32-byte) random secret key
secret_key = os.urandom(32)
# Optionally, encode the key in Base64 to get a text representation
secret_key_base64 = base64.b64encode(secret_key).decode('utf-8')

print(secret_key_base64)