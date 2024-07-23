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
#include "CUHashTools.hpp"

using namespace cugl;
using namespace cugl::graphics;

#pragma mark -
#pragma mark Application State

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
    _batch  = SpriteBatch::alloc();

    // Start-up basic input
#ifdef CU_TOUCH_SCREEN
    Input::activate<Touchscreen>();
#else
    Input::activate<Mouse>();
#endif
    
    Input::activate<Keyboard>();
    Input::activate<TextInput>();

    _assets->attach<audio::Sound>(audio::SoundLoader::alloc()->getHook());
    _assets->attach<Font>(FontLoader::alloc()->getHook());
    _assets->attach<Texture>(TextureLoader::alloc()->getHook());
    _assets->attach<JsonValue>(JsonLoader::alloc()->getHook());
    _assets->attach<WidgetValue>(WidgetLoader::alloc()->getHook());
    _assets->attach<scene2::SceneNode>(scene2::Scene2Loader::alloc()->getHook());

    _assets->loadDirectory("json/loading.json");
    _loading.init(_assets,"json/assets.json");
    _loading.setSpriteBatch(_batch);
    _status = LOAD;
    
    // Que up the other assets
    _loading.start();
    audio::AudioEngine::start(24);
    
    netcode::NetworkLayer::start(netcode::NetworkLayer::Log::INFO);
      
    Application::onStartup(); // YOU MUST END with call to parent
    setDeterministic(true);
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
    _gameplay.dispose();
    _loginScene.dispose();
    _assets = nullptr;
    _batch = nullptr;
    
    // Shutdown input
#ifdef CU_TOUCH_SCREEN
    Input::deactivate<Touchscreen>();
#else
    Input::deactivate<Mouse>();
#endif
    
	audio::AudioEngine::stop();
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
    CULog("RESUMED");
    audio::AudioEngine::get()->pause();
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
    CULog("RESUMED");
    audio::AudioEngine::get()->resume();
}


#pragma mark -
#pragma mark Application Loop

void DDBXApp::preUpdate(float timestep){
    if (_status == LOAD && _loading.isActive()) {
        _loading.update(timestep);
    }
    else if (_status == LOAD) {
        _loading.dispose(); // Disables the input listeners in this mode
        _loginScene.init(_assets);
        _loginScene.setSpriteBatch(_batch);
        _loginScene.setActive(true);
        _status = LOGIN;
    }
    else if (_status == LOGIN) {
        _loginScene.update(timestep);
        auto _network = _loginScene.getNet();
        if(_network){
            if(_network->getStatus() == NetEventController::Status::HANDSHAKE && _network->getShortUID()){
                CULog("GAME START HANDSHAKE");
                _gameplay.init(_assets, _network, _network->isHost(),_loginScene.getSaveData());
                _gameplay.setSpriteBatch(_batch);
                _network->markReady();
            }
            else if (_network->getStatus() == NetEventController::Status::INGAME) {
                CULog("IN GAME");
                _loginScene.setActive(false);
                _gameplay.setActive(true);
                _status = GAME;
            }
            else if (_network->getStatus() == NetEventController::Status::NETERROR) {
                _network->disconnect();
                _gameplay.setActive(false);
                _gameplay.dispose();
                _loginScene.dispose();
                _loginScene.init(_assets);
                _loginScene.setSpriteBatch(_batch);
                _loginScene.setActive(true);
                _status = LOGIN;
            }
        }
    }
    else if (_status == GAME){
        if(_gameplay.isComplete()){
            auto data = _gameplay.serializeGame();
            std::string b64 = cugl::hashtool::b64_encode(*data);
            CULog("Serialized Game Data: %s", b64.c_str());
            
            _gameplay.setActive(false);
            auto _network = _loginScene.getNet();
            _network->disconnect();
            _status = LOGIN;
            auto respsave = cpr::Post(cpr::Url{server_url + "/save"},
                                      cpr::Body{b64},
                                      cpr::Header{{"Content-Type", "application/json"}},
                                      cpr::Bearer{_loginScene.getAuthToken()});
            CUAssertLog(respsave.status_code == 200, "Not saved");
            _loginScene.setActive(true);
        }
        _gameplay.preUpdate(timestep);
    }
}

void DDBXApp::postUpdate(float timestep) {
    if (_status == GAME) {
        _gameplay.postUpdate(timestep);
    }
}

void DDBXApp::fixedUpdate() {
    if(_loginScene.getNet()){
        _loginScene.getNet()->updateNet();
    }
    if (_status == GAME) {
        _gameplay.fixedUpdate();
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


/**
 * Inidividualized update method for the host scene.
 *
 * This method keeps the primary {@link #update} from being a mess of switch
 * statements. It also handles the transition logic from the host scene.
 *
 * @param timestep  The amount of time (in seconds) since the last frame
 */
void DDBXApp::updateLoginScene(float timestep) {
    _loginScene.update(timestep);
}

/**
 * The method called to draw the application to the screen.
 *
 * This is your core loop and should be replaced with your custom implementation.
 * This method should OpenGL and related drawing calls.
 *
 * When overriding this method, you do not need to call the parent method
 * at all. The default implmentation does nothing.
 */
void DDBXApp::draw() {
    switch (_status) {
        case LOAD:
            _loading.render();
            break;
        case LOGIN:
            _loginScene.render();
            break;
        case GAME:
            _gameplay.render();
        default:
            break;
    }
}

