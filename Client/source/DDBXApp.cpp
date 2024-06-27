//
//  DDBXApp.cpp
//  DDBX Client
//
//  Root class for the DDBX Client application.
//
//  Author: Barry Lyu
//  Version: 3/8/24
//

#include "DDBXApp.h"

using namespace cugl;

#pragma mark -
#pragma mark Server State

#define server_url std::string("https://us-east4-ddbx-soxehli.cloudfunctions.net/test_player_info")

#define login_body(username,password) ("{\"username\":\"" + (username) + "\",\"password\":\"" + (password) + "\"}")


/**
 * The method called after OpenGL is initialized, but before running the application.
 *
 * This is the method in which all user-defined program intialization should
 * take place.  You should not create a new init() method.
 *
 * When overriding this method, you should call the parent method as the
 * very last line.  This ensures that the state will transition to FOREGROUND,
 * causing the application to run.
 */
void DDBXApp::onStartup() {
    _assets = AssetManager::alloc();
    _assets->attach<JsonValue>(JsonLoader::alloc()->getHook());

    _status = LOAD;
    
    // Que up the other assets
    _assets->loadDirectory("json/assets.json");
    
    cugl::net::NetworkLayer::start(net::NetworkLayer::Log::INFO);

    CULog("Assets loaded");

    _status = LOGIN;
    _response = cpr::PostAsync(cpr::Url{server_url + "/login"},
                                        cpr::Body(login_body(std::string("server1"), std::string("first"))),
                                        cpr::Header{{"Content-Type", "application/json"}});

    auto json = _assets->get<JsonValue>("server");
    _config.set(json);
    _networks = _networks->alloc(_config);

    setDeterministic(true);
      
    Application::onStartup(); // YOU MUST END with call to parent

    
}

/**
 * The method called when the application is ready to quit.
 *
 * This is the method to dispose of all resources allocated by this
 * application.  As a rule of thumb, everything created in onStartup()
 * should be deleted here.
 *
 * When overriding this method, you should call the parent method as the
 * very last line.  This ensures that the state will transition to NONE,
 * causing the application to be deleted.
 */
void DDBXApp::onShutdown() {
    _assets = nullptr;
    if(_networks){
        _networks->close();
        _networks = nullptr;
    }

	Application::onShutdown();  // YOU MUST END with call to parent
}

/**
 * The method called when the application is suspended and put in the background.
 *
 * When this method is called, you should store any state that you do not
 * want to be lost.  There is no guarantee that an application will return
 * from the background; it may be terminated instead.
 *
 * If you are using audio, it is critical that you pause it on suspension.
 * Otherwise, the audio thread may persist while the application is in
 * the background.
 */
void DDBXApp::onSuspend() {
}

/**
 * The method called when the application resumes and put in the foreground.
 *
 * If you saved any state before going into the background, now is the time
 * to restore it. This guarantees that the application looks the same as
 * when it was suspended.
 *
 * If you are using audio, you should use this method to resume any audio
 * paused before app suspension.
 */
void DDBXApp::onResume() {
}

static std::string dec2hex(const std::string dec) {
    Uint32 value = strtool::stou32(dec);
    if (value >= 655366) {
        value = 0;
    }
    return strtool::to_hexstring(value,4);
}

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
#pragma mark Server Loop

void DDBXApp::preUpdate(float timestep){
    
}

void DDBXApp::postUpdate(float timestep) {
    
}

void DDBXApp::fixedUpdate() {
    if(_status == LOAD && _assets->complete()){
        
    }

    if(_status == LOGIN && _response.wait_for(std::chrono::seconds(0)) == std::future_status::ready){
        auto response = _response.get();
        if(response.status_code == 200){
            _status = LOBBY;
            auto json = JsonValue::allocWithJson(response.text);
            _authToken = json->getString("token");
            _uuid = json->getString("playerUUID");
            printf("Token: %s\n", _authToken.c_str());
            printf("UUID: %s\n", _uuid.c_str());
            _networks->open(_uuid, _authToken);
            CULog("Logged In");
        }
    }

    if(_status == LOBBY && _networks->getState() == NetcodeConnection::State::CONNECTED){
        _status = GAME;
        CULog("Connected to server");
        CULog("Room ID: %s", _networks->getRoom().c_str());
        CULog("HEX Room ID: %s", dec2hex(_networks->getRoom()).c_str());
    }
}

/**
 * The method called to update the application data.
 *
 * @param timestep  The amount of time (in seconds) since the last frame
 */
void DDBXApp::update(float timestep) {
    //deprecated
}
