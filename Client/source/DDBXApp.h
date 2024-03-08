//
//  DDBXApp.h
//  DDBX Client
//
//  Root class for the DDBX Client application.
//
//  Author: Barry Lyu
//  Version: 3/8/24
//

#ifndef __DDBX_APP_H__
#define __DDBX_APP_H__
#include <cugl/cugl.h>
#include "DDBXGameScene.h"
#include "DDBXLoadingScene.h"
#include "DDBXLoginScene.h"
#include "NLClientScene.h"

using namespace cugl::physics2::net;

/**
 * This class represents the application root for the ship demo.
 */
class DDBXApp : public cugl::Application {
    
enum Status {
    LOAD,
    LOGIN,
    GAME
};

protected:
    /** The global sprite batch for drawing (only want one of these) */
    std::shared_ptr<cugl::SpriteBatch> _batch;
    /** The global asset manager */
    std::shared_ptr<cugl::AssetManager> _assets;
    
    // Player modes

    /** The primary controller for the game world */
    GameScene _gameplay;
    /** The controller for the loading screen */
    LoadingScene _loading;
    /** The controller for the login screen */
    LoginScene _loginScene;
    
    /** Whether or not we have finished loading all assets */
    bool _loaded;
    
    /** The current status of the application */
    Status _status;
    
public:
#pragma mark Constructors
    /**
     * Creates, but does not initialized a new application.
     *
     * This constructor is called by main.cpp.  You will notice that, like
     * most of the classes in CUGL, we do not do any initialization in the
     * constructor.  That is the purpose of the init() method.  Separation
     * of initialization from the constructor allows main.cpp to perform
     * advanced configuration of the application before it starts.
     */
    DDBXApp() : cugl::Application(), _loaded(false) {}
    
    /**
     * Disposes of this application, releasing all resources.
     *
     * This destructor is called by main.cpp when the application quits.
     * It simply calls the dispose() method in Application.  There is nothing
     * special to do here.
     */
    ~DDBXApp() { }
    
    
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
    virtual void onStartup() override;
    
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
    virtual void onShutdown() override;
    
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
    virtual void onSuspend() override;
    
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
    virtual void onResume()  override;
    
#pragma mark Application Loop

    virtual void preUpdate(float timestep) override;

    virtual void postUpdate(float timestep) override;

    virtual void fixedUpdate() override;
    
    /**
     * The method called to update the application data.
     *
     * @param timestep  The amount of time (in seconds) since the last frame
     */
    virtual void update(float timestep) override;
    
    /**
     * Inidividualized update method for the login scene.
     *
     * @param timestep  The amount of time (in seconds) since the last frame
     */
    void updateLoginScene(float timestep);
    
    /**
     * The method called to draw the application to the screen.
     */
    virtual void draw() override;
};
#endif /* __DDBX_APP_H__ */
