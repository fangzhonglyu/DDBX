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

    _assets->attach<Font>(FontLoader::alloc()->getHook());
    _assets->attach<Texture>(TextureLoader::alloc()->getHook());
    _assets->attach<Sound>(SoundLoader::alloc()->getHook());
    _assets->attach<scene2::SceneNode>(Scene2Loader::alloc()->getHook());
    _assets->attach<JsonValue>(JsonLoader::alloc()->getHook());
    _assets->attach<WidgetValue>(WidgetLoader::alloc()->getHook());

    _status = LOAD;
    
    // Que up the other assets
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
    _assets = nullptr;
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


#pragma mark -
#pragma mark Server Loop

void DDBXApp::preUpdate(float timestep){
    
}

void DDBXApp::postUpdate(float timestep) {
    
}

void DDBXApp::fixedUpdate() {
    
}

/**
 * The method called to update the application data.
 *
 * @param timestep  The amount of time (in seconds) since the last frame
 */
void DDBXApp::update(float timestep) {
    //deprecated
}
