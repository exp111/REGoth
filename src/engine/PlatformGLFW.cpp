

#include <bx/platform.h>
#if BX_PLATFORM_LINUX || BX_PLATFORM_OSX || BX_PLATFORM_WINDOWS
#include "PlatformGLFW.h"
#include "utils/Utils.h"
#include <thread>


#if defined(_glfw3_h_)
// If GLFW/glfw3.h is included before bgfxplatform.h we can enable GLFW3
// window interop convenience code.

#	if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
#		define GLFW_EXPOSE_NATIVE_X11
#		define GLFW_EXPOSE_NATIVE_GLX
#	elif BX_PLATFORM_OSX
#		define GLFW_EXPOSE_NATIVE_COCOA
#		define GLFW_EXPOSE_NATIVE_NSGL
#	elif BX_PLATFORM_WINDOWS
#		define GLFW_EXPOSE_NATIVE_WIN32
#		define GLFW_EXPOSE_NATIVE_WGL
#	endif //
#	include <GLFW/glfw3native.h>
#endif
#include <bgfx/bgfxplatform.h>
#include <bgfx-cmake/bx/include/bx/commandline.h>



//#include <bx/bx.h>
//#include <bgfx/bgfx.h>
//#include <bx/string.h>
//#include <bx/crtimpl.h>

//#include <time.h>

//#include <bx/thread.h>
//#include <bx/os.h>
//#include <bx/handlealloc.h>
//#include <string.h> // memset
//#include <string>

//#include <fcntl.h>

//#if BX_PLATFORM_EMSCRIPTEN
//#	include <emscripten.h>
//#endif // BX_PLATFORM_EMSCRIPTEN

//#include "entry_p.h"
//#include "cmd.h"
//#include "input.h"

//#define RMT_ENABLED ENTRY_CONFIG_PROFILER
//#include <remotery/lib/Remotery.h>

using namespace Engine;


inline void glfwSetWindow(GLFWwindow* _window)
{
    bgfx::PlatformData pd;
#	if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
    pd.ndt = glfwGetX11Display();
    pd.nwh = (void*) (uintptr_t) glfwGetGLXWindow(_window);
    pd.context = glfwGetGLXContext(_window);
#	elif BX_PLATFORM_OSX
    pd.ndt          = NULL;
    pd.nwh          = glfwGetCocoaWindow(_window);
    pd.context      = glfwGetNSGLContext(_window);
#	elif BX_PLATFORM_WINDOWS
    pd.ndt          = NULL;
    pd.nwh          = glfwGetWin32Window(_window);
    pd.context      = NULL;
#	endif // BX_PLATFORM_WINDOWS
    pd.backBuffer = NULL;
    pd.backBufferDS = NULL;
    setPlatformData(pd);
}


int32_t PlatformGLFW::run(int argc, char** argv)
{
    int width = 1280;
    int height = 720;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    bx::CommandLine cmd(argc, (const char**)argv);

    bool fullscreen = false;
    if(cmd.hasArg('f'))
    {
        fullscreen = true;

        // Get native resolution
        const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        width = mode->width;
        height = mode->height;
    }

    GLFWwindow* window;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(width, height, "REGoth", fullscreen ? glfwGetPrimaryMonitor() : NULL, NULL);
    if (!window)
    {
        std::cout << "GLFW: Failed to create window!";
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    //glfwMakeContextCurrent(window);

    // Initialize bgfx
    glfwSetWindow(window);

    glfwSetKeyCallback(window, GLFWkeyEvent);
    glfwSetMouseButtonCallback(window, GLFWmouseButtonEvent);
    glfwSetCursorPosCallback(window, GLFWmouseMoveEvent);
    glfwSetScrollCallback(window, GLFWscrollEvent);

    glfwSetWindowSizeCallback(window, GLFWwindowSizeEvent);
    GLFWwindowSizeEvent(window, width, height);

    setMouseLockCallback([&window](bool lock) {
        if (lock)
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        else
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    });

    // TODO read bindings from file

//    bindKey(GLFW_KEY_W, ActionType::DebugSkySpeed, false);

    // Camera Switch Keys

    bindKey(GLFW_KEY_F1, ActionType::CameraFirstPerson, false);
    bindKey(GLFW_KEY_F2, ActionType::CameraThirdPerson, false);
    bindKey(GLFW_KEY_F3, ActionType::CameraFree, false);
    bindKey(GLFW_KEY_F4, ActionType::CameraViewer, false);

    // FirstPerson Camera Controller

    bindKey(GLFW_KEY_W, ActionType::FirstPersonMoveForward, true, false);
    bindKey(GLFW_KEY_S, ActionType::FirstPersonMoveForward, true, true);
    bindKey(GLFW_KEY_D, ActionType::FirstPersonMoveRight, true, false);
    bindKey(GLFW_KEY_A, ActionType::FirstPersonMoveRight, true, true);

    bindMouseAxis(MouseAxis::CursorX, ActionType::FirstPersonLookHorizontal, true, false);
    bindMouseAxis(MouseAxis::CursorY, ActionType::FirstPersonLookVertical, true, true);

    bindKey(GLFW_KEY_UP, ActionType::FirstPersonLookVertical, true, false);
    bindKey(GLFW_KEY_DOWN, ActionType::FirstPersonLookVertical, true, true);
    bindKey(GLFW_KEY_RIGHT, ActionType::FirstPersonLookHorizontal, true, false);
    bindKey(GLFW_KEY_LEFT, ActionType::FirstPersonLookHorizontal, true, true);

    // Free Camera Controller

    bindKey(GLFW_KEY_W, ActionType::FreeMoveForward, true, false);
    bindKey(GLFW_KEY_S, ActionType::FreeMoveForward, true, true);
    bindKey(GLFW_KEY_D, ActionType::FreeMoveRight, true, false);
    bindKey(GLFW_KEY_A, ActionType::FreeMoveRight, true, true);
    bindKey(GLFW_KEY_Q, ActionType::FreeMoveUp, true, true);
    bindKey(GLFW_KEY_E, ActionType::FreeMoveUp, true, false);

    bindMouseAxis(MouseAxis::CursorX, ActionType::FreeLookHorizontal, true, false);
    bindMouseAxis(MouseAxis::CursorY, ActionType::FreeLookVertical, true, true);

    bindKey(GLFW_KEY_UP, ActionType::FreeLookVertical, true, false);
    bindKey(GLFW_KEY_DOWN, ActionType::FreeLookVertical, true, true);
    bindKey(GLFW_KEY_RIGHT, ActionType::FreeLookHorizontal, true, false);
    bindKey(GLFW_KEY_LEFT, ActionType::FreeLookHorizontal, true, true);

    // Viewer Camera Controller
    bindMouseAxis(MouseAxis::CursorX, ActionType::ViewerHorizontal, true);
    bindMouseAxis(MouseAxis::CursorY, ActionType::ViewerVertical, true);
    bindMouseButton(GLFW_MOUSE_BUTTON_MIDDLE, ActionType::ViewerRotate, true);
    bindMouseButton(GLFW_MOUSE_BUTTON_LEFT, ActionType::ViewerClick, false);
    bindKey(GLFW_KEY_LEFT_SHIFT, ActionType::ViewerPan, true);
    bindKey(GLFW_KEY_LEFT_CONTROL, ActionType::ViewerZoom, true);
    bindMouseAxis(MouseAxis::ScrollY, ActionType::ViewerMouseWheel, false);

    // Player Controller
    bindKey(GLFW_KEY_W, ActionType::PlayerForward, true);
    bindKey(GLFW_KEY_S, ActionType::PlayerBackward, true);
    bindKey(GLFW_KEY_LEFT, ActionType::PlayerTurnLeft, true);
    bindKey(GLFW_KEY_RIGHT, ActionType::PlayerTurnRight, true);
    bindKey(GLFW_KEY_A, ActionType::PlayerStrafeLeft, true);
    bindKey(GLFW_KEY_D, ActionType::PlayerStrafeRight, true);
    bindKey(GLFW_KEY_SPACE, ActionType::DebugMoveSpeed, true);
    bindKey(GLFW_KEY_LEFT_SHIFT, ActionType::DebugMoveSpeed2, true);
    bindKey(GLFW_KEY_LEFT_CONTROL, ActionType::PlayerAction, false);

    bindKey(GLFW_KEY_P, ActionType::PauseGame, false);

    bindKey(GLFW_KEY_B, ActionType::OpenStatusMenu, false);
    bindKey(GLFW_KEY_F10, ActionType::OpenConsole, false);
    bindKey(GLFW_KEY_ESCAPE, ActionType::Escape, false);



//    // special keys test
//    bindKey(GLFW_KEY_LEFT_SHIFT, ActionType::PlayerForward, true);
//    bindKey(GLFW_KEY_RIGHT_SHIFT, ActionType::PlayerBackward, true);
//    bindKey(GLFW_KEY_LEFT_CONTROL, ActionType::PlayerTurnLeft, true);
//    bindKey(GLFW_KEY_RIGHT_CONTROL, ActionType::PlayerTurnRight, true);
//    bindKey(GLFW_KEY_LEFT_ALT, ActionType::PlayerStrafeLeft, true);
//    bindKey(GLFW_KEY_RIGHT_ALT, ActionType::PlayerStrafeRight, true);

    // Start logic thread
    std::promise<int32_t> returnValue;
    auto future = returnValue.get_future();
    std::thread thread(&PlatformGLFW::mainLoop, this, std::move(returnValue), argc, argv);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {

        /* Poll for and process events */
        //glfwPollEvents();
        glfwWaitEvents();
    }

    thread.join();
    glfwTerminate();
    return future.get();
}

void PlatformGLFW::GLFWkeyEvent(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    Input::keyEvent(key, scancode, action, mods);
}

void PlatformGLFW::GLFWmouseButtonEvent(GLFWwindow *window, int button, int action, int mods)
{
    Input::mouseButtonEvent(button, action, mods);
}

void PlatformGLFW::GLFWmouseMoveEvent(GLFWwindow *window, double xPos, double yPos)
{
    Input::mouseMoveEvent(xPos, yPos);
}

void PlatformGLFW::GLFWscrollEvent(GLFWwindow *window, double xOffset, double yOffset)
{
    Input::scrollEvent(xOffset, yOffset);
}

void PlatformGLFW::GLFWwindowSizeEvent(GLFWwindow *window, int width, int height)
{
    Input::windowSizeEvent(width, height);

    m_WindowHeight = height;
    m_WindowWidth = width;
}
#endif