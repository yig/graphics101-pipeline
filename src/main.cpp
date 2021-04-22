#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "glcompat.h"

#include <iostream>

#include "pythonlike.h"
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

bool save_screenshot( const std::string& path ) {
    // From: https://vallentin.dev/2013/09/02/opengl-screenshot
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    
    int x = viewport[0];
    int y = viewport[1];
    int width = viewport[2];
    int height = viewport[3];
    
    char* data = (char*) malloc( (size_t) (width * height * 3) ); // 3 components (R, G, B)
    if( !data ) return false;
    
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(x, y, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);
    
    stbi_flip_vertically_on_write( 1 );
    const bool success = stbi_write_png( path.c_str(), width, height, 3, data, 0 );
    if( success ) std::cout << "Saved screenshot: " << path << '\n';
    else std::cerr << "ERROR: Could not save screenshot: " << path << '\n';
    
    free(data);
    
    return success;
}

void key_callback( GLFWwindow* window, int key, int scancode, int action, int mods ) {
    if( key == GLFW_KEY_S && action == GLFW_PRESS ) {
        const std::string path = graphics101::saveFileDialog( "Save Screenshot As...", "screenshot.png", "PNG Image (*.png)" );
        save_screenshot( path );
    }
}

void usage( char* argv0 ) {
    std::cerr << "Usage: " << argv0 << " [--width pixels] [--height pixels] [--screenshot path/to/save.png] [path/to/scene.json]\n";
}

}

int main( int argc, char* argv[] ) {
    // Gather the args into a vector.
    std::vector<std::string> args( argv + 1, argv + argc );
    if( pythonlike::get_optional_parameter( args, "-h" ) || pythonlike::get_optional_parameter( args, "--help" ) ) {
        usage( argv[0] );
        return -1;
    }
    
    // Parse arguments.
    int width = 500;
    int height = 500;
    std::string screenshotpath;
    bool save_and_quit = false;
    {
        std::string val;
        if( pythonlike::get_optional_parameter( args, "--width", val ) ) width = std::stoi(val);
        if( pythonlike::get_optional_parameter( args, "--height", val ) ) height = std::stoi(val);
        save_and_quit = pythonlike::get_optional_parameter( args, "--screenshot", screenshotpath );
    }
    if( args.size() > 1 ) {
        usage( argv[0] );
        return -1;
    }
    
    // No need to make a menubar for save and quit.
    // if( save_and_quit ) glfwInitHint(GLFW_COCOA_MENUBAR, GLFW_FALSE);
    
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
    // No need to show the window for save and quit.
    // if( save_and_quit ) glfwWindowHint( GLFW_VISIBLE, GLFW_FALSE );
    
    // Create a window
    GLFWwindow *window = glfwCreateWindow( width, height, "GPU Pipeline", 0, 0 );
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
    
    assert( args.size() <= 1 );
    if( args.size() == 1 ) {
        scene_path = args.front();
    } else {
        scene_path = graphics101::loadFileDialog( "Open Scene", "", "Scene JSON Files (*.js *.json)" );
        // If the user cancels, length will be zero.
        if( scene_path.size() == 0 ) {
            // If the user cancels, don't show them an error. They cancelled. Just let them quit.
            // errorDialog( "Error", "You must select a JSON scene file to proceed." );
            usage( argv[0] );
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
    
    // The timer has never been called, so initialize `lastTimer`
    // to a time earlier than the program start time minus the callback interval.
    double lastTimer = -2*( fabs(gui->timerCallbackMilliseconds()/1000.) );
    while( !glfwWindowShouldClose(window) ) {
        // Draw
        gui->draw();
        
        // Swap front and back buffers
        glfwSwapBuffers(window);
        
        // Save a screenshot and quit?
        if( save_and_quit ) {
            save_screenshot( screenshotpath );
            break;
        }
        
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
