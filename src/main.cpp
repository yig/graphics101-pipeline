#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "glcompat.h"

#include <iostream>

#include "dialogs.h"
#include "pipelineguifactory.h"

int main( int argc, char* argv[] )
{
    // Initialize GLFW and request an OpenGL Core Profile
    if( !glfwInit() ) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    // Create a window
    GLFWwindow *window = glfwCreateWindow( 500, 500, "GPU Pipeline", 0, 0 );
    if( !window )
    {
        std::cerr << "Failed to create a window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent( window );
    glfwSwapInterval( 1 );
    
    // Initialize gl3w.
    if( gl3wInit() ) {
        std::cerr << "Failed to initialize OpenGL\n";
        return -1;
    }
    if( !gl3wIsSupported(3,3) ) {
        std::cerr << "OpenGL 3.3 is not supported\n";
        return -1;
    }
    std::cout << "OpenGL " << glGetString(GL_VERSION) << ", GLSL " << glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n";
    
    std::string scene_path;
    if( argc == 2 ) {
        scene_path = argv[1];
    } else {
        const std::string scene_path = graphics101::loadFileDialog( "Open Scene", "", "Scene JSON Files (*.js *.json)" );
        // If the user cancels, length will be zero.
        if( scene_path.size() == 0 ) {
            // If the user cancels, don't show them an error. They cancelled. Just let them quit.
            // errorDialog( "Error", "You must select a JSON scene file to proceed." );
            return -1;
        }
    }
    
    graphics101::PipelineGUIPtr gui = graphics101::PipelineGUIFromScenePath( scene_path );
    if( !gui ) {
        graphics101::errorDialog( "Error", "Unable to load scene. Check console for details." );
        return -1;
    }
    
    gui->init();
    
    // The timer has never been called, so `lastTimer`
    // is earlier than the program start time.
    const double timerCallbackSeconds = gui->timerCallbackMilliseconds()/1000.;
    double lastTimer = -2*timerCallbackSeconds;
    while( !glfwWindowShouldClose(window) ) {
        // Draw
        gui->draw();
        
        // Swap front and back buffers
        glfwSwapBuffers(window);
        
        // Poll for and process events. Wait timerCallbackMilliseconds.
        // If timerCallbackSeconds is negative, then we don't refresh.
        if( timerCallbackSeconds < 0 ) glfwWaitEvents();
        else {
            double now = glfwGetTime();
            const double waitTime = lastTimer + timerCallbackSeconds - now;
            
            // glfwWaitEventsTimeout() requires a positive number.
            if( waitTime > 0 ) glfwWaitEventsTimeout( waitTime );
            else glfwPollEvents();
            
            now = glfwGetTime();
            if( now - lastTimer > timerCallbackSeconds ) {
                gui->timerEvent( now );
                
                // If we can't keep up with the frame rate, we slow down but don't
                // try to catch up later.
                lastTimer = now;
                // Try to catch up. If we can't keep up, we will run as fast as possible.
                // lastTimer += timerCallbackSeconds;
            }
        }
    }
    
    glfwTerminate();
    return 0;
}
