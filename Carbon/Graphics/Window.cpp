#include "Window.hpp"

namespace cbn
{
	
	//-------------------------------------------------------------------------------------

	Ptr<Window> Window::Create(Ptr<GraphicsContext>& graphics_context)
	{
		// While window creation normally has no reason to fail, we will perform
		// checks to ensure that only a single window is created per graphics context
		// in order to ensure that the user pointers and callbacks for the window will work properly

		// We will determine whether a window exists for this context 
		// based on whether the user pointer is set to null or not
		if(glfwGetWindowUserPointer(graphics_context->m_Handle) != NULL)
		{
			// Since the user pointer isnt null, then a window already exists
			// for this context so return nullptr to fail the creation of the window
			return nullptr;
		}

		// If we are here, then we allowed to create a window for this context
		Ptr<Window> window = Ptr<Window>(new Window(graphics_context->m_Handle));

		// Set up the required GLFW callbacks, and user pointer for the window
		glfwSetWindowUserPointer(window->m_ContextHandle, window.get());
		glfwSetWindowCloseCallback(window->m_ContextHandle, &Window::window_close_callback);
		glfwSetWindowFocusCallback(window->m_ContextHandle, &Window::window_focus_callback);
		glfwSetWindowSizeCallback(window->m_ContextHandle, &Window::window_resize_callback);

		// Return the successfully created window
		return window;
	}

	//-------------------------------------------------------------------------------------

	void Window::window_resize_callback(GLFWwindow* window_handle, int width, int height)
	{
		// Get the window from the handle's user pointer
		Window* window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window_handle));

		// Invoke the resolution change event
		window->ResolutionChangeEvent.invoke(glm::vec2(width, height));
	}
	
	//-------------------------------------------------------------------------------------

	void Window::window_focus_callback(GLFWwindow* window_handle, int is_focused)
	{
		// Get the window from the handle's user pointer
		Window* window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window_handle));

		// Invoke the focus change event
		window->FocusEvent.invoke(is_focused);
	}
	
	//-------------------------------------------------------------------------------------

	void Window::window_close_callback(GLFWwindow* window_handle)
	{
		// Get the window from the handle's user pointer
		Window* window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window_handle));

		// Invoke the close requested event
		window->CloseRequestEvent.invoke();
	}
	
	//-------------------------------------------------------------------------------------

	GLFWmonitor* Window::determine_monitor()
	{
		// To determine which monitor the window is currently in, we will
		// compare the position of the window to the coords of the monitors

		GLFWmonitor** monitors;
		int window_x, window_y, monitor_count;

		// Get all monitors and the window position
		monitors = glfwGetMonitors(&monitor_count);
		glfwGetWindowPos(m_ContextHandle, &window_x, &window_y);

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

	void Window::make_fullscreen()
	{
		// Update the window attributes to make the window have a frame and not be resizable
		glfwSetWindowAttrib(m_ContextHandle, GLFW_DECORATED, GLFW_TRUE);
		glfwSetWindowAttrib(m_ContextHandle, GLFW_RESIZABLE, GLFW_FALSE);

		// Get the monitor that the window is within, we will fullscreen on this monitor
		const auto monitor = determine_monitor();
		auto video_mode = glfwGetVideoMode(monitor);

		// Update the window to be fullscreen within the monitor
		glfwSetWindowMonitor(m_ContextHandle, monitor, 0, 0, video_mode->width, video_mode->height, GLFW_DONT_CARE);

		// Update the display mode member for the window and invoke the display mode change event
		DisplayModeChangeEvent.invoke(DisplayMode::FULLSCREEN);
		m_DisplayMode = DisplayMode::FULLSCREEN;

	}

	//-------------------------------------------------------------------------------------

	void Window::make_borderless()
	{
		//TODO: note that this make not actually be making a borderless fullscreen window?

		// Update the display mode to be fullscreen, 
		make_fullscreen();

		// Update the window attributes so that its not decorated
		glfwSetWindowAttrib(m_ContextHandle, GLFW_DECORATED, GLFW_FALSE);

		// Update the display mode member for the window and invoke the display mode change event
		DisplayModeChangeEvent.invoke(DisplayMode::BORDERLESS);
		m_DisplayMode = DisplayMode::BORDERLESS;
	}

	//-------------------------------------------------------------------------------------

	void Window::make_resizable()
	{
		// Update the display mode to be windowed, 
		make_windowed();

		// Update the window attributes so that its resizable
		glfwSetWindowAttrib(m_ContextHandle, GLFW_RESIZABLE, GLFW_TRUE);
	
		// Update the display mode member for the window and invoke the display mode change event
		DisplayModeChangeEvent.invoke(DisplayMode::RESIZABLE);
		m_DisplayMode = DisplayMode::RESIZABLE;
	}

	//-------------------------------------------------------------------------------------

	void Window::make_windowed()
	{
		// Update the window attributes to make the window have a frame and not be resizable
		glfwSetWindowAttrib(m_ContextHandle, GLFW_DECORATED, GLFW_TRUE);
		glfwSetWindowAttrib(m_ContextHandle, GLFW_RESIZABLE, GLFW_FALSE);

		// Get the monitor that the window is within, this will be used
		// to determine where the window should be positioned once its made windowed
		const auto monitor = determine_monitor();
		auto video_mode = glfwGetVideoMode(monitor);

		// Find the correct positions to center the window
		int position_x = static_cast<int>((video_mode->width / 2) - (m_Resolution.x / 2));
		int position_y = static_cast<int>((video_mode->height / 2) - (m_Resolution.y / 2));

		// Set the window to be windowed
		glfwSetWindowMonitor(m_ContextHandle, nullptr, position_x, position_y, static_cast<int>(m_Resolution.x), static_cast<int>(m_Resolution.y), GLFW_DONT_CARE);

		// Update the display mode member for the window and invoke the display mode change event
		DisplayModeChangeEvent.invoke(DisplayMode::WINDOWED);
		m_DisplayMode = DisplayMode::WINDOWED;
	}

	//-------------------------------------------------------------------------------------

	Window::Window(GLFWwindow* context_handle)
		: m_ContextHandle(context_handle)
	{
		// Update window to its default setup, this will also
		// set the default values for the window members
		set_display_mode(DisplayMode::WINDOWED);
		set_title("Untitled Window");
		set_resolution({1280, 720});
		set_vsync(false);
		show();

	}
	
	//-------------------------------------------------------------------------------------

	Window::~Window()
	{
		// Since the window handle is actually managed by the GraphicsContext, when 
		// the window goes out of scope we will simply hide the window. This way, another
		// window can simply be created from the same context in the future. 
		hide();

		// Reset the user pointer to null as this is what we are using to check if a 
		// window already exists for a context in the Create() function
		glfwSetWindowUserPointer(m_ContextHandle, NULL);
	}
	
	//-------------------------------------------------------------------------------------

	void Window::show()
	{
		// Only show the window if its hidden
		if(is_visible()) return;

		// Show the window and invoke the visibility change event
		glfwShowWindow(m_ContextHandle);
		VisibilityChangeEvent.invoke(true);
	}
	
	//-------------------------------------------------------------------------------------

	void Window::hide()
	{
		// Only hide the window if its visible
		if(!is_visible()) return;
		
		// Hide the window and invoke the visibility change event
		glfwShowWindow(m_ContextHandle);
		VisibilityChangeEvent.invoke(false);
	}
	
	//-------------------------------------------------------------------------------------

	void Window::focus()
	{
		// Only focus the window if its not focused already
		if(is_focused()) return;

		// Focus the window, note that we do not need to invoke
		// the event since it is handled by the GLFW callback
		glfwFocusWindow(m_ContextHandle);
	}
	
	//-------------------------------------------------------------------------------------

	void Window::update()
	{
		// Swap the buffers to show the latest frame buffer
		glfwSwapBuffers(m_ContextHandle);

		// Poll all input events
		glfwPollEvents();
	}
	
	//-------------------------------------------------------------------------------------

	bool Window::is_visible() const
	{
		return glfwGetWindowAttrib(m_ContextHandle, GLFW_VISIBLE) == GLFW_TRUE;
	}
	
	//-------------------------------------------------------------------------------------

	bool Window::is_focused() const
	{
		return glfwGetWindowAttrib(m_ContextHandle, GLFW_FOCUSED) == GLFW_TRUE;
	}
	
	//-------------------------------------------------------------------------------------

	bool Window::is_vsync_enabled() const
	{
		return m_VSyncEnabled;
	}
	
	//-------------------------------------------------------------------------------------

	glm::vec2 Window::get_resolution() const
	{
		return m_Resolution;
	}
	
	//-------------------------------------------------------------------------------------

	std::string_view Window::get_title() const
	{
		return m_Title;
	}
	
	//-------------------------------------------------------------------------------------

	void Window::set_vsync(const bool enable_vsync)
	{
		// Only update if we are actually changing the state of the window
		if(enable_vsync != m_VSyncEnabled)
		{
			// Update the the swap interval based on whether we are enabling or disabling vsync
			glfwSwapInterval(enable_vsync ? 1 : 0);

			// Update the vsync flag and invoke the vsync change event
			VSyncChangeEvent.invoke(enable_vsync);
			m_VSyncEnabled = enable_vsync;
		}
	}
	
	//-------------------------------------------------------------------------------------

	void Window::set_title(const std::string& title)
	{
		// Only update if we are actually changing the state of the window
		if(title != m_Title)
		{
			// Update the the swap interval based on whether we are enabling or disabling vsync
			glfwSetWindowTitle(m_ContextHandle, title.c_str());

			// Update the title member and invoke the title change event
			TitleChangeEvent.invoke(title);
			m_Title = title;
		}
	}
	
	//-------------------------------------------------------------------------------------

	void Window::set_resolution(const glm::vec2& resolution)
	{
		// Only update if we are actually changing the state of the window
		if(resolution != m_Resolution)
		{
			// Update the the swap interval based on whether we are enabling or disabling vsync
			glfwSetWindowSize(m_ContextHandle, static_cast<int>(resolution.x), static_cast<int>(resolution.y));

			// Update the resolution member, note that we do not need to 
			// invoke the event as it will be invoked by the GLFW resize callback
			m_Resolution = resolution;
		}
	}
	
	//-------------------------------------------------------------------------------------

	void Window::set_display_mode(const DisplayMode display_mode)
	{
		// Only update if we are actually changing the state of the window
		if(display_mode != m_DisplayMode)
		{
			// Switch the display mode of the window
			// Note that the event is invoked in the make_xx methods
			switch(display_mode)
			{
				case DisplayMode::WINDOWED:
				{
					make_windowed();
					break;
				}
				case DisplayMode::RESIZABLE:
				{
					make_resizable();
					break;
				}
				case DisplayMode::BORDERLESS:
				{
					make_borderless();
					break;
				}
				case DisplayMode::FULLSCREEN:
				{
					make_fullscreen();
					break;
				}
			}
		}
	}
	
	//-------------------------------------------------------------------------------------

}