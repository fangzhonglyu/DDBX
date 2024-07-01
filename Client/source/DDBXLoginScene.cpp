//
//  DDBXLoginScene.cpp
//  DDBX Client
//
//  Login controller for the DDBX Client application.
//
//  Author: Barry Lyu
//  Version: 3/8/24
//
#include <cugl/cugl.h>
#include <iostream>
#include <sstream>
#include <cpr/cpr.h>

#include "DDBXLoginScene.h"

//#define server_url (_assets->get<JsonValue>("server")->getString("BackendURL"))

#define server_url std::string("https://us-east4-ddbx-soxehli.cloudfunctions.net/test_player_info")

#define login_body(username,password) ("{\"username\":\"" + (username) + "\",\"password\":\"" + (password) + "\"}")

#define balance_body(balance) ("{\"balance\":" + (std::to_string(balance)) + "}")

using namespace cugl;
using namespace cugl::net;
using namespace std;

#pragma mark -
#pragma mark Level Layout

/** Regardless of logo, lock the height to this */
#define SCENE_HEIGHT  720

/**
 * Converts a hexadecimal string to a decimal string
 *
 * This function assumes that the string is 4 hexadecimal characters
 * or less, and therefore it converts to a decimal string of five
 * characters or less (as is the case with the lobby server). We
 * pad the decimal string with leading 0s to bring it to 5 characters
 * exactly.
 *
 * @param hex the hexadecimal string to convert
 *
 * @return the decimal equivalent to hex
 */
static std::string hex2dec(const std::string hex) {
    Uint32 value = strtool::stou32(hex,0,16);
    std::string result = strtool::to_string(value);
    if (result.size() < 5) {
        size_t diff = 5-result.size();
        std::string alt(5,'0');
        for(size_t ii = 0; ii < result.size(); ii++) {
            alt[diff+ii] = result[ii];
        }
        result = alt;
    }
    return result;
}

#pragma mark -
#pragma mark Provided Methods
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
bool LoginScene::init(const std::shared_ptr<cugl::AssetManager>& assets) {
    // Initialize the scene to a locked width
    Size dimen = Application::get()->getDisplaySize();
    dimen *= SCENE_HEIGHT/dimen.height;
    if (assets == nullptr) {
        return false;
    } else if (!Scene2::init(dimen)) {
        return false;
    }
    
    // Start up the input handler
    _assets = assets;
    
    // Acquire the scene built by the asset loader and resize it the scene
    std::shared_ptr<scene2::SceneNode> scene = _assets->get<scene2::SceneNode>("host");
    scene->setContentSize(dimen);
    scene->doLayout(); // Repositions the HUD

    _startgame = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("host_center_start"));
    _gameid = std::dynamic_pointer_cast<scene2::TextField>(_assets->get<scene2::SceneNode>("host_center_game_field_text"));
    _player = std::dynamic_pointer_cast<scene2::TextField>(_assets->get<scene2::SceneNode>("host_center_players_field_text"));

    _player->setHidden(true);
    
    // Program the buttons

    _startgameListener = _startgame->addListener([this](const std::string& name, bool down) {
        if (down) {
            _loginClicked = true;
            _response = cpr::PostAsync(cpr::Url{server_url + "/login"},
                                        cpr::Body(login_body(_gameid->getText(), _player->getText())),
                                        cpr::Header{{"Content-Type", "application/json"}});
            CULog("LOGIN REQUEST SENT");
        }
    });
    
    addChild(scene);
    setActive(false);
    return true;
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void LoginScene::dispose() {
    if (_active) {
        removeAllChildren();
        _active = false;
    }
}

/**
 * Sets whether the scene is currently active
 *
 * This method should be used to toggle all the UI elements.  Buttons
 * should be activated when it is made active and deactivated when
 * it is not.
 *
 * @param value whether the scene is currently active
 */
void LoginScene::setActive(bool value) {
    if (isActive() != value) {
        Scene2::setActive(value);
        /**
         * TODO: if value is true, you need to activate the _backout button, and set the clicked variable to false. You need to also call the network controller to start a connection as a host. If the value is false, and reset all buttons and textfields to their original state.
         */
#pragma mark BEGIN SOLUTION
        if (value) {
            _gameid->activate();
            _player->activate();
            _backClicked = false;
            updateText(_startgame, "LOGIN");
            _startgame->activate();
        } else {
            _gameid->deactivate();
            _player->deactivate();
            _startgame->deactivate();
            updateText(_startgame, "INACTIVE");
            // If any were pressed, reset them
            _startgame->setDown(false);
        }
        
        
#pragma mark END SOLUTION
    }
}


/**
 * Updates the text in the given button.
 *
 * Techincally a button does not contain text. A button is simply a scene graph
 * node with one child for the up state and another for the down state. So to
 * change the text in one of our buttons, we have to descend the scene graph.
 * This method simplifies this process for you.
 *
 * @param button    The button to modify
 * @param text      The new text value
 */
void LoginScene::updateText(const std::shared_ptr<scene2::Button>& button, const std::string text) {
    auto label = std::dynamic_pointer_cast<scene2::Label>(button->getChildByName("up")->getChildByName("label"));
    label->setText(text);
}

int counter = 0;

#pragma mark -
#pragma mark Student Methods
/**
 * The method called to update the scene.
 *
 * We need to update this method to constantly talk to the server
 *
 * @param timestep  The amount of time (in seconds) since the last frame
 */
void LoginScene::update(float timestep) {
    if(_loginClicked){
        _startgame->deactivate();
    }
    else{
        _startgame->activate();
    }
    
//    if(_checkBalanceClicked && _resp2.wait_for(std::chrono::seconds(0)) == std::future_status::ready){
//        auto response = _resp2.get();
//        if(response.status_code == 200){
//            auto json = JsonValue::allocWithJson(response.text);
//            _player->setText("Balance: " + json->getString("balance"));
//        }
//        else{
//            _player->setText("Check failed");
//        }
//        _checkBalanceClicked = false;
//    }
    
    
    if(_loginClicked && _response.wait_for(std::chrono::seconds(0)) == std::future_status::ready){
        auto response = _response.get();
        if(response.status_code == 200){
            updateText(_startgame, "LOGGEDIN");
            CULog("%s", response.text.c_str());
            auto json = JsonValue::allocWithJson(response.text);
            _authToken = json->getString("token");
            _uuid = json->getString("uuid");
            _isLoggedin = true;
//            _gameid->addExitListener([this](const std::string& name, const std::string& text) {
//                if(stoi(text)){
//                    _response = cpr::PostAsync(cpr::Url{server_url + "/balance"},
//                                                cpr::Body(balance_body(stoi(text))),
//                                               cpr::Header{{"Content-Type", "application/json"}},cpr::Bearer{_authToken});
//                }
//            });
//            updateText(_startgame, "CHECK BALANCE");
            CUAssertLog(_startgame->removeListener(_startgameListener), "Listener not removed");
//            _startgame->addListener([this](const std::string& name, bool down) {
//                if (down) {
//                    _checkBalanceClicked = true;
//                    _resp2 = cpr::GetAsync(cpr::Url{server_url + "/balance"}, cpr::Header{{"Content-Type", "application/json"}},cpr::Bearer{_authToken});
//                    
//                }
//            });
        }
        else{
            updateText(_startgame, "FAILED");
            
            _startgame->setDown(false);
        }
        _loginClicked = false;
    }
    counter++;
    setColor(Color4(counter % 255, 100, 100));
}
