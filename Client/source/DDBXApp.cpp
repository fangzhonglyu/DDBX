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
    
    _assets->attach<Font>(FontLoader::alloc()->getHook());
    _assets->attach<Texture>(TextureLoader::alloc()->getHook());
    _assets->attach<Sound>(SoundLoader::alloc()->getHook());
    _assets->attach<scene2::SceneNode>(Scene2Loader::alloc()->getHook());
    _assets->attach<JsonValue>(JsonLoader::alloc()->getHook());
    _assets->attach<WidgetValue>(WidgetLoader::alloc()->getHook());

    _loading.init(_assets);
    _status = LOAD;
    
    // Que up the other assets
    AudioEngine::start(24);
    _assets->loadDirectoryAsync("json/assets.json",nullptr);
    
    cugl::net::NetworkLayer::start(net::NetworkLayer::Log::INFO);
      
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
    
	AudioEngine::stop();
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
    AudioEngine::get()->pause();
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
    AudioEngine::get()->resume();
}


#pragma mark -
#pragma mark Application Loop

void DDBXApp::preUpdate(float timestep){
    if (_status == LOAD && _loading.isActive()) {
        _loading.update(0.01f);
    }
    else if (_status == LOAD) {
        _loading.dispose(); // Disables the input listeners in this mode
        _loginScene.init(_assets);
        _loginScene.setActive(true);
        _status = LOGIN;
    }
    else if (_status == LOGIN) {
 /*       if (_loginScene.isLoggedin()) {
			_loginScene.setActive(false);
			_gameplay.init(_assets);
			_status = GAME;
		}*/
        _loginScene.update(timestep);
        if(_loginScene.isLoggedin()){
            _loginScene.setActive(false);
            _gameplay.init(_assets);
            _status = GAME;
        }
    }
    else if (_status == GAME){
        if(_gameplay.isComplete()){
            _gameplay.reset();
            _status = LOGIN;
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
            _loading.render(_batch);
            break;
        case LOGIN:
            _loginScene.render(_batch);
            break;
        case GAME:
            _gameplay.render(_batch);
        default:
            break;
    }
}

