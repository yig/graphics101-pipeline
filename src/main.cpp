#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "glcompat.h"

#include <iostream>

#include "dialogs.h"
#include "pipelineguifactory.h"

// For save_screenshot()
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

namespace {
// The global GUI.
graphics101::PipelineGUIPtr gui = nullptr;

// Callback function wrappers.
void framebuffer_size_callback( GLFWwindow* window, int width, int height ) {
    assert( gui );
    gui->resize( width, height );
}

void mouse_button_callback( GLFWwindow* window, int button, int action, int mods ) {
    assert( gui );
    
    double x, y;
    glfwGetCursorPos( window, &x, &y );
    
    graphics101::PipelineGUI::Event event;
    event.x = x;
    event.y = y;
    
    if( action == GLFW_PRESS ) gui->mousePressEvent( event );
    else if( action == GLFW_RELEASE ) gui->mouseReleaseEvent( event );
}
void cursor_position_callback( GLFWwindow* window, double x, double y ) {
    assert( gui );
    
    graphics101::PipelineGUI::Event event;
    event.x = x;
    event.y = y;
    
    bool drag = false;
    for( int button = GLFW_MOUSE_BUTTON_1; button <= GLFW_MOUSE_BUTTON_LAST; ++button ) {
        int state = glfwGetMouseButton( window, button );
        if( state == GLFW_PRESS )
        {
            drag = true;
            break;
        }
    }
    
    if( drag ) gui->mouseDragEvent( event );
    else gui->mouseMoveEvent( event );
}

void save_screenshot() {
    const std::string path = graphics101::saveFileDialog( "Save Screenshot As...", "screenshot.png", "PNG Image (*.png)" );
    
    // From: https://vallentin.dev/2013/09/02/opengl-screenshot
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    
    int x = viewport[0];
    int y = viewport[1];
    int width = viewport[2];
    int height = viewport[3];
    
    char* data = (char*) malloc( (size_t) (width * height * 3) ); // 3 components (R, G, B)
    if( !data ) return;
    
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(x, y, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);
    
    stbi_flip_vertically_on_write( 1 );
    const bool success = stbi_write_png( path.c_str(), width, height, 3, data, 0 );
    if( success ) std::cout << "Saved screenshot: " << path << '\n';
    
    free(data);
}

void key_callback( GLFWwindow* window, int key, int scancode, int action, int mods ) {
    if( key == GLFW_KEY_S && action == GLFW_PRESS ) {
        save_screenshot();
    }
}

}

int main( int argc, char* argv[] ) {
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
        scene_path = graphics101::loadFileDialog( "Open Scene", "", "Scene JSON Files (*.js *.json)" );
        // If the user cancels, length will be zero.
        if( scene_path.size() == 0 ) {
            // If the user cancels, don't show them an error. They cancelled. Just let them quit.
            // errorDialog( "Error", "You must select a JSON scene file to proceed." );
            return -1;
        }
    }
    
    gui = graphics101::PipelineGUIFromScenePath( scene_path );
    if( !gui ) {
        graphics101::errorDialog( "Error", "Unable to load scene. Check console for details." );
        return -1;
    }
    
    gui->init();
    {
        // The framebuffer size will be different from the window size on high DPI screens.
        int width, height;
        glfwGetFramebufferSize( window, &width, &height );
        gui->resize( width, height );
    }
    
    // Setup event callbacks.
    glfwSetFramebufferSizeCallback( window, framebuffer_size_callback );
    glfwSetMouseButtonCallback( window, mouse_button_callback );
    glfwSetCursorPosCallback( window, cursor_position_callback );
    glfwSetKeyCallback( window, key_callback );
    // TODO: Keyboard callback 's' for save screenshot: https://vallentin.dev/2013/09/02/opengl-screenshot
    
    // The timer has never been called, so `lastTimer`
    // is earlier than the program start time.
    double lastTimer = -2*( gui->timerCallbackMilliseconds()/1000. );
    while( !glfwWindowShouldClose(window) ) {
        // Draw
        gui->draw();
        
        // Swap front and back buffers
        glfwSwapBuffers(window);
        
        // Poll for and process events. Wait timerCallbackMilliseconds.
        // If timerCallbackSeconds is negative, then we don't refresh.
        // Get this number inside the loop so the gui can change it dynamically if needed.
        const double timerCallbackSeconds = gui->timerCallbackMilliseconds()/1000.;
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
