#include "Window.hpp"

namespace cbn
{

	//-------------------------------------------------------------------------------------

	bool Window::s_OpenGLLoaded = false;

	//-------------------------------------------------------------------------------------

	void Window::gl_error_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* user_param)
	{
		// The user parameter is a GLFW window handle, which is itself associated 
		// with the window object that contains the error event that needs to be called
		const Window* window = static_cast<Window*>(glfwGetWindowUserPointer((GLFWwindow*)user_param));
		window->ErrorEvent.dispatch(std::string(message), source, severity);
	}

	//-------------------------------------------------------------------------------------

	void Window::glfw_resize_callback(GLFWwindow* glfw_handle, int width, int height)
	{
		Window* window = static_cast<Window*>(glfwGetWindowUserPointer(glfw_handle));
		
		// If the display mode is changed or is RESIZABLE, the window can be resized 
		// without using the set_resolution function hence we need to update the resolution
		// attribute of the window. We also want to invoke the property change event. 
		window->m_Resolution = glm::vec2(width, height);
		window->ResolutionEvent.dispatch(window->m_Resolution);
	}
	
	//-------------------------------------------------------------------------------------

	void Window::glfw_focus_callback(GLFWwindow* glfw_handle, int is_focused)
	{
		const Window* window = static_cast<const Window*>(glfwGetWindowUserPointer(glfw_handle));
		window->FocusEvent.dispatch(is_focused);
	}
	
	//-------------------------------------------------------------------------------------

	void Window::glfw_close_callback(GLFWwindow * glfw_handle)
	{
		const Window* window = static_cast<Window*>(glfwGetWindowUserPointer(glfw_handle));
		window->CloseEvent.dispatch();
	}
	
	//-------------------------------------------------------------------------------------

	void Window::glfw_set_display_mode(GLFWwindow* glfw_handle, DisplayMode display_mode)
	{
		// Set window default window attributes which are common to most display modes
		glfwSetWindowAttrib(glfw_handle, GLFW_RESIZABLE, GLFW_FALSE);
		glfwSetWindowAttrib(glfw_handle, GLFW_DECORATED, GLFW_TRUE);

		// Get the monitor that the window is within in order to make sure that 
		// the window stays on this monitor when the display mode is changed
		GLFWmonitor* monitor = glfw_find_window_monitor(glfw_handle);
		const GLFWvidmode* video_mode = glfwGetVideoMode(monitor);

		switch(display_mode)
		{
			case DisplayMode::BORDERLESS:
				// Remove the border of the window, then follow normal fullscreen creation
				glfwSetWindowAttrib(glfw_handle, GLFW_DECORATED, GLFW_FALSE);
			case DisplayMode::FULLSCREEN:
			{
				// Update the window to be fullscreen within the monitor
				glfwSetWindowMonitor(glfw_handle, monitor, 0, 0, video_mode->width, video_mode->height, GLFW_DONT_CARE);
				break;
			}
		
			case DisplayMode::RESIZABLE:
				// Make the window resizable then follow normal windowed window creation
				glfwSetWindowAttrib(glfw_handle, GLFW_RESIZABLE, GLFW_TRUE);
			case DisplayMode::WINDOWED:
			{
				int width, height;
				glfwGetWindowSize(glfw_handle, &width, &height);

				// Find the required window position in order to keep the window centred 
				// on the monitor when it is windowed. The window position specified is for 
				// the top left corner so take half the resolution off the window resolution
				int position_x = static_cast<int>((video_mode->width  / 2) - (width  / 2));
				int position_y = static_cast<int>((video_mode->height / 2) - (height / 2));

				// Set the window to be windowed
				glfwSetWindowMonitor(glfw_handle, nullptr, position_x, position_y, width, height, GLFW_DONT_CARE);
				break;
			}
			default:
				CBN_Assert(false, "Unknown display mode");
		}
	}

	//-------------------------------------------------------------------------------------

	GLFWmonitor* Window::glfw_find_window_monitor(GLFWwindow * glfw_handle)
	{
		// To determine which monitor the window is currently in, we will
        // compare the position of the window to the coords of the monitors

		GLFWmonitor** monitors;
		int window_x, window_y, monitor_count;

		// Get all monitors and the window position
		monitors = glfwGetMonitors(&monitor_count);
		glfwGetWindowPos(glfw_handle, &window_x, &window_y);

		// Iterate through monitors until we find which one the window is within 
		for(int i = 0; i < monitor_count; i++) {

			// Get the monitor's mode in order to get its resolution
			const GLFWvidmode* monitor_mode = glfwGetVideoMode(monitors[i]);

			// Get the monitors position
			int monitor_x, monitor_y;
			glfwGetMonitorPos(monitors[i], &monitor_x, &monitor_y);

			// Check if the windows coords is within the monitors coords
			if(window_x >= monitor_x && window_x <= monitor_mode->width
				&& window_y >= monitor_y && window_y <= monitor_mode->height)
			{
				// Since the window is within this monitor, return it
				return monitors[i];
			}
		}

		// If no monitor could be found then an error has occured so assert and return nullptr
		CBN_Assert(false, "Could not determine which monitor the window is positioned in");

		return nullptr;
	}
	
	//-------------------------------------------------------------------------------------

	URes<Window> Window::Create(Properties window_properties)
	{

		// Initialize GLFW for the first time. If GLFW is already initialized then it 
		// will immediately return true. If false is returned, initialization failed.
		if(glfwInit() == GLFW_FALSE)
		{
			return nullptr;
		}

		// If an OpenGL version is specified then we want to create a context using that version
		if(window_properties.opengl_version != Version{0,0,0})
		{
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, window_properties.opengl_version.get_major_version());
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, window_properties.opengl_version.get_minor_version());
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		}

		// If debug context is set then we will attempt to create a debug context for debug output
		// This is standard as of OpenGL 4.3, however is still supported if the GL_ARB_debug_output
		// extension is supported. We will have to query for its support after context creation
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, window_properties.opengl_debug ? GLFW_TRUE : GLFW_FALSE);

		// We don't want the window to be visible until context creation has fully succeeded
		glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

		// Use default values to create the window, if initialization succeeds then these
		// settings will be updated so the actual values specified don't matter that much.
		GLFWwindow* glfw_handle = NULL;
		if((glfw_handle = glfwCreateWindow(1280, 720, "Untitled Window", NULL, NULL)) == NULL)
		{
			return nullptr;
		}

		glfwMakeContextCurrent(glfw_handle);

		// Load the required OpenGL functions from the driver if they haven't been loaded yet
		if(!s_OpenGLLoaded)
		{
			if(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == NULL)
			{
				// If the OpenGL functions could not be loaded, then window 
				// creation has failed so the window needs to be destroyed
				glfwDestroyWindow(glfw_handle);
				return nullptr;
			}
			s_OpenGLLoaded = true;
		}

		// Set up the callback for the OpenGL debug context. 
		// This will fail if a debug context was not created 
		if(window_properties.opengl_debug)
		{
			// Debug ouput is only supported post OpenGL 4.3. Earlier versions will 
			// need to make use of the GLAD_GL_ARB_debug_output extension if supported
			if(window_properties.opengl_version >= Version(4, 3))
			{
				glDebugMessageCallback(&Window::gl_error_callback, glfw_handle);
				glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
				glEnable(GL_DEBUG_OUTPUT);
			}
			else if(GLAD_GL_ARB_debug_output)
			{
				glDebugMessageCallbackARB(&Window::gl_error_callback, glfw_handle);
				glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
				glEnable(GL_DEBUG_OUTPUT);
			}
			else return nullptr;
		}

		// The OpenGL version specified in properties may not match the actual version of the context
		// which gets created. Especially if no OpenGL version was specified in the first place. 
		window_properties.opengl_version = {static_cast<unsigned>(GLVersion.major), static_cast<unsigned>(GLVersion.minor)};

		// Enable colour blending, there no reason you wouldnt want this on
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// If we have made it here, then the window was successfully created
		return Resource::WrapUnique(new Window(glfw_handle, window_properties));
	}
	
	//-------------------------------------------------------------------------------------

	Window::Window(GLFWwindow* glfw_handle, Properties properties)
		: m_OpenGLVersion(properties.opengl_version),
		  m_OpenGLDebug(properties.opengl_debug),
		  m_GLFWHandle(glfw_handle)
	{
		CBN_Assert(glfw_handle != NULL, "Handle cannot be nullptr");

		set_vsync(properties.vsync);
		set_title(properties.title);
		set_resolution(properties.resolution);
		set_display_mode(properties.display_mode);
	
		glfwSetWindowUserPointer(glfw_handle, this);
		glfwSetWindowSizeCallback(glfw_handle, &Window::glfw_resize_callback);
		glfwSetWindowCloseCallback(glfw_handle, &Window::glfw_close_callback);
		glfwSetWindowFocusCallback(glfw_handle, &Window::glfw_focus_callback);
	
		show();
	}

	//-------------------------------------------------------------------------------------

	Window::Window(Window&& window) noexcept
		: m_DisplayMode(window.m_DisplayMode),
		  m_GLFWHandle(window.m_GLFWHandle),
		  m_OpenGLVersion(window.m_OpenGLVersion),
		  m_Resolution(window.m_Resolution),
		  m_Title(window.m_Title),
		  m_OpenGLDebug(window.m_OpenGLDebug),
		  m_VSync(window.m_VSync)
	{
		// Since this new window object will now control the GLFW handle, we need to update the user pointer
		glfwSetWindowUserPointer(m_GLFWHandle, this);
	}
	
	//-------------------------------------------------------------------------------------

	Window::~Window()
	{
		glfwDestroyWindow(m_GLFWHandle);
	}
	
	//-------------------------------------------------------------------------------------

	void Window::show()
	{
		if(is_visible()) return;
		
		glfwShowWindow(m_GLFWHandle);
		VisibilityEvent.dispatch(true);
	}

	//-------------------------------------------------------------------------------------

	void Window::hide()
	{
		if(!is_visible()) return;

		glfwShowWindow(m_GLFWHandle);
		VisibilityEvent.dispatch(false);
	}

	//-------------------------------------------------------------------------------------

	void Window::focus()
	{
		if(is_focused()) return;

		// Focus the window, note that we do not need to invoke
		// the event since it is handled by the GLFW callback
		glfwFocusWindow(m_GLFWHandle);
	}

	//-------------------------------------------------------------------------------------

	void Window::update()
	{
		// Swap the buffers to show the latest frame buffer
		glfwSwapBuffers(m_GLFWHandle);

		// Poll all input events
		glfwPollEvents();
	}
	
	//-------------------------------------------------------------------------------------

	void Window::clear() const
	{
		glClear(GL_COLOR_BUFFER_BIT);
	}

	//-------------------------------------------------------------------------------------

	bool Window::is_visible() const
	{
		return glfwGetWindowAttrib(m_GLFWHandle, GLFW_VISIBLE) == GLFW_TRUE;
	}

	//-------------------------------------------------------------------------------------

	bool Window::is_focused() const
	{
		return glfwGetWindowAttrib(m_GLFWHandle, GLFW_FOCUSED) == GLFW_TRUE;
	}

	//-------------------------------------------------------------------------------------
	
	bool Window::is_vsync_enabled() const
	{
		return m_VSync;
	}

	//-------------------------------------------------------------------------------------
	
	bool Window::is_debug_enabled() const
	{
		return m_OpenGLDebug;
	}

	//-------------------------------------------------------------------------------------
	
	glm::uvec2 Window::get_resolution() const
	{
		return m_Resolution;
	}

	//-------------------------------------------------------------------------------------
	
	std::string_view Window::get_title() const
	{
		return m_Title;
	}

	//-------------------------------------------------------------------------------------
	
	Window::DisplayMode Window::get_display_mode() const
	{
		return m_DisplayMode;
	}

	//-------------------------------------------------------------------------------------
	
	const Version Window::get_opengl_version() const
	{
		return m_OpenGLVersion;
	}

	//-------------------------------------------------------------------------------------
	
	void Window::set_vsync(const bool enable_vsync)
	{
		// A swap interval of 1 means that OpenGL will
		// wait 1 refresh before swapping buffers.
		glfwSwapInterval(enable_vsync ? 1 : 0);

		m_VSync = enable_vsync;
		VSyncEvent.dispatch(enable_vsync);
	}

	//-------------------------------------------------------------------------------------
	
	void Window::set_title(const std::string & title)
	{
		glfwSetWindowTitle(m_GLFWHandle, title.c_str());

		m_Title = title;
		TitleEvent.dispatch(title);
	}

	//-------------------------------------------------------------------------------------
	
	void Window::set_resolution(const glm::vec2& resolution)
	{
		//TODO: temp, determine where this should actually go
		glViewport(0, 0, resolution.x, resolution.y);

		glfwSetWindowSize(m_GLFWHandle, static_cast<int>(resolution.x), static_cast<int>(resolution.y));

		// Update the resolution member but do not invoke the resolution
		// change event since it will be automatically invoked by the resize callback
		m_Resolution = resolution;
	}

	//-------------------------------------------------------------------------------------
	
	void Window::set_display_mode(const DisplayMode display_mode)
	{
		glfw_set_display_mode(m_GLFWHandle, display_mode);
			
		m_DisplayMode = display_mode;
		DisplayModeEvent.dispatch(display_mode);
	}
	
	//-------------------------------------------------------------------------------------

	void Window::operator=(Window&& window) noexcept
	{
		m_OpenGLVersion = window.m_OpenGLVersion;
		m_OpenGLDebug = window.m_OpenGLDebug;
		m_DisplayMode = window.m_DisplayMode;
		m_GLFWHandle = window.m_GLFWHandle;
		m_Resolution = window.m_Resolution;
		m_Title = window.m_Title;
		m_VSync = window.m_VSync;
	
		// Since this new window object will now control the GLFW handle, we need to update the user pointer
		glfwSetWindowUserPointer(m_GLFWHandle, this);
	}
	
	//-------------------------------------------------------------------------------------

}