//
//  DDBXLoginScene.h
//  DDBX Client
//
//  Login controller for the DDBX Client application.
//
//  Author: Barry Lyu
//  Version: 3/8/24
//


#ifndef __DDBX_LOGIN_SCENE_H__
#define __DDBX_LOGIN_SCENE_H__
#include <cugl/cugl.h>
#include <vector>
#include <cpr/cpr.h>

#define PING_TEST_COUNT 5

#define server_url std::string("https://us-east4-ddbx-soxehli.cloudfunctions.net/test_player_info")

#define login_body(username,password) ("{\"username\":\"" + (username) + "\",\"password\":\"" + (password) + "\"}")

#define balance_body(balance) ("{\"balance\":" + (std::to_string(balance)) + "}")

#define lobby_body(roomid) ("{\"roomid\":" + (std::to_string(roomid)) + "}")

using namespace cugl::physics2::distrib;

/**
 * This class provides the interface to make a new game.
 *
 * Most games have a since "matching" scene whose purpose is to initialize the
 * network controller.  We have separate the host from the client to make the
 * code a little more clear.
 */
class LoginScene : public cugl::scene2::Scene2 {

protected:
    /** The asset manager for this scene. */
    std::shared_ptr<cugl::AssetManager> _assets;
    /** The network connection (as made by this scene) */
    std::shared_ptr<NetEventController> _network;

    /** The menu button for starting a game */
    std::shared_ptr<cugl::scene2::Button> _startgame;
    
    /** The back button for the menu scene */
    std::shared_ptr<cugl::scene2::Button> _backout;
    
    std::shared_ptr<cugl::scene2::Label> _label1;
    
    std::shared_ptr<cugl::scene2::Label> _label2;
    
    /** The game id label (for updating) */
    std::shared_ptr<cugl::scene2::TextField> _gameid;
    /** The players label (for updating) */
    std::shared_ptr<cugl::scene2::TextField> _player;
    
    cpr::AsyncResponse _response;
    cpr::AsyncResponse _resp2;
    
    std::shared_ptr<std::vector<std::byte>> _saveData;
    
    int _roomid;
    Uint32 _startgameListener;

    /** Whether the startGame button had been pressed. */
    bool _loginClicked = false;
    bool _lobbyClicked = false;
    bool _negotiating = false;
    bool _fetchingSave = false;
    bool _isHost = false;
    bool _waitingToStart = false;
    /** Whether the back button had been pressed. */
    bool _backClicked = false;
    /** Whether the player is logged in. */
    bool _isLoggedin = false;

    std::string _authToken;
    std::string _uuid;

public:
#pragma mark -
#pragma mark Constructors
    /**
     * Creates a new host scene with the default values.
     *
     * This constructor does not allocate any objects or start the game.
     * This allows us to use the object without a heap pointer.
     */
    LoginScene() : cugl::scene2::Scene2(), _response(cpr::AsyncWrapper<cpr::Response>(std::future<cpr::Response>())), _resp2(cpr::AsyncWrapper<cpr::Response>(std::future<cpr::Response>())){
    }
    
    /**
     * Disposes of all (non-static) resources allocated to this mode.
     *
     * This method is different from dispose() in that it ALSO shuts off any
     * static resources, like the input controller.
     */
    ~LoginScene() { dispose(); }
    
    /**
     * Disposes of all (non-static) resources allocated to this mode.
     */
    void dispose() override;
    
    /**
     * Initializes the controller contents, and starts the game
     *
     * In previous labs, this method "started" the scene.  But in this
     * case, we only use to initialize the scene user interface.  We
     * do not activate the user interface yet, as an active user
     * interface will still receive input EVEN WHEN IT IS HIDDEN.
     *
     * That is why we have the method {@link #setActive}.
     *
     * @param assets    The (loaded) assets for this game mode
     *
     * @return true if the controller is initialized properly, false otherwise.
     */
    bool init(const std::shared_ptr<cugl::AssetManager>& assets);
    
    /**
     * Sets whether the scene is currently active
     *
     * This method should be used to toggle all the UI elements.  Buttons
     * should be activated when it is made active and deactivated when
     * it is not.
     *
     * @param value whether the scene is currently active
     */
    virtual void setActive(bool value) override;

    /**
     * The method called to update the scene.
     *
     * We need to update this method to constantly talk to the server
     *
     * @param timestep  The amount of time (in seconds) since the last frame
     */
    void update(float timestep) override;
    
    /**
     * Returns whether the back button is pressed
     */
    bool getBackClicked() { return _backClicked; };

    bool isLoggedin() { return _isLoggedin; }
    
    std::shared_ptr<NetEventController> getNet() { return _network; }
    
    std::string getAuthToken() { return _authToken; }
    
    std::shared_ptr<std::vector<std::byte>> getSaveData() { return _saveData; }

private:
    /**
     * Updates the text in the given button.
     *
     * @param button    The button to modify
     * @param text      The new text value
     */
    void updateText(const std::shared_ptr<cugl::scene2::Button>& button, const std::string text);
    
};

#endif /* __DDBX_LOGIN_SCENE_H__ */
