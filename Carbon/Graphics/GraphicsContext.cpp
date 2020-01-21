#include "GraphicsContext.hpp"

namespace cbn
{
	//-------------------------------------------------------------------------------------

	int GraphicsContext::s_ContextCount = 0;
	bool GraphicsContext::s_GLFWInitialized = false;
	bool GraphicsContext::s_OpenGLFunctionsLoaded = false;

	//-------------------------------------------------------------------------------------

	void GraphicsContext::debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* user_param)
	{
		// Invoke the error event in the context provided by the user param
		reinterpret_cast<const GraphicsContext*>(user_param)->ErrorEvent.invoke(std::string(message),source,type,severity);
	}
	
	//-------------------------------------------------------------------------------------

	std::string GraphicsContext::generate_opengl_error_message(const GLenum gl_error_code) const
	{
		// Convert given OpenGL error codes into their respective text description
		switch(gl_error_code)
		{
			case GL_NO_ERROR:
				return "No error";
			
			case GL_INVALID_ENUM:
				return "An invalid value has been specified for an enumurated argument";
			
			case GL_INVALID_OPERATION:
				return "An operation was specified which is not allowed in the current context state";
			
			case GL_INVALID_FRAMEBUFFER_OPERATION:
				return "An operation was attempted on an incomplete framebuffer object";
			
			case GL_OUT_OF_MEMORY:
				return "Unable to execute command due to insufficient memory";
			
			case GL_STACK_UNDERFLOW:
				return "An operation tried to cause an internal stack to underflow";
			
			case GL_STACK_OVERFLOW:
				return "An operation tried to cause an internal stack to overflow";
		}

		// If the error code is invalid, simply set the message to "unknown error"
		return "Unknown error";
	}
	
	//-------------------------------------------------------------------------------------

	GraphicsContext::GraphicsContext() 
		: m_Initialized(false),
		  m_ContextVersion(0,0)
	{
		// Increase the context count by 1
		s_ContextCount++;
	}
	
	//-------------------------------------------------------------------------------------

	GraphicsContext::~GraphicsContext() 
	{
		// Reduce the context count by 1
		s_ContextCount--;

		// If the context was initialized and its the last existing context 
		// then we should also terminate GLFW on destruction of this context
		if(s_GLFWInitialized && s_ContextCount == 0)
		{
			s_GLFWInitialized = false;
			glfwTerminate();
		}
	}
	
	//-------------------------------------------------------------------------------------

	bool GraphicsContext::try_initialize(const Version& context_version, bool debug_mode)
	{
		// If the context has already been initialized, then error out
		if(is_initialized()) return false;

		// If GLFW itself has not been initialised yet, then initialise it now
		if(!s_GLFWInitialized)
		{
			// If the GLFW initialisation failed, then return false
			if(glfwInit() == GLFW_FALSE)
			{
				return false;
			}

			// If the initialisation succeeded, then set the initialised flag
			s_GLFWInitialized = true;
		}

		// Set hints so that we create a core OpenGL context of the given context version 
		// Note that if the version is not supported, then the initialisation will simply fail
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, context_version.get_major_version());
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, context_version.get_minor_version());
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		// If debug mode has been requested then we need to make a debug context
		// Note that this is only supported with OpenGL 4.3 and beyond, so initialisation
		// will fail if debug mode is requested and the context version is too low
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, debug_mode ? GLFW_TRUE : GLFW_FALSE);
		if(debug_mode && context_version < Version(4, 3))
		{
			return false;
		}

		// Specify hints for creating an invisible non-resizable window 
		glfwWindowHint(GLFW_FOCUS_ON_SHOW, GLFW_TRUE);
		glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
		glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

		// Attempt to create the OpenGL context, which is tied with a window in GLFW
		GLFWwindow* window_handle = glfwCreateWindow(1280, 720, "Untitled Window", NULL, NULL);

		// If the context was not created successfully, then return false
		if(window_handle == NULL)
		{
			return false;
		}

		// If we are here, the context was created successfully 
		// so make sure the context is current to this thread as it should
		glfwMakeContextCurrent(window_handle);

		// Create the window associated with this context
		m_AssociatedWindow = std::unique_ptr<Window>(new Window(window_handle));

		// If the OpenGL functions have not been loaded yet, then we need to load them now
		if(!s_OpenGLFunctionsLoaded)
		{
			// If the OpenGL functions could not be loaded due to 
			// some error, then return false for a failed initialisation
			if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
			{
				return false;
			}

			// Make sure to set that the functions have been loaded so 
			// that any other context don't try to reload them
			s_OpenGLFunctionsLoaded = true;
		}

		// If we made a debug context, then we should set a debug callback
		if(debug_mode)
		{
			glDebugMessageCallback(&GraphicsContext::debug_callback, this);
		}

		// If we have made it here, then we successfuly initialised 
		// the context so update the flag and context version
		m_ContextVersion = context_version;
		m_Initialized = true;

		return true;
	}
	
	//-------------------------------------------------------------------------------------

	bool GraphicsContext::is_extension_supported(const std::string_view& extension) const
	{
		CBN_Assert(is_initialized(), "Cannot check for extension support without first initializing the context");

		// Get all the supported extensions and search for the given one
		const auto supported_extensions = get_supported_extensions();
		return std::find(supported_extensions.begin(), supported_extensions.end(), extension) != supported_extensions.end();
	}
	
	//-------------------------------------------------------------------------------------

	void* GraphicsContext::load_function(const std::string_view& function_name) const
	{
		CBN_Assert(is_initialized(), "Cannot load any OpenGL context functions without first initializing the context");

		return glfwGetProcAddress(function_name.data());
	}

	//-------------------------------------------------------------------------------------

	std::vector<std::string> GraphicsContext::get_supported_extensions() const
	{
		CBN_Assert(is_initialized(), "Cannot get the supported context extensions without first initializing the context");

		std::vector<std::string> extensions;

		// Check how many extensions are supported by the context and 
		// reserve the required amount of space to store them in the vector
		GLint extension_count = 0;
		glGetIntegerv(GL_NUM_EXTENSIONS, &extension_count);
		extensions.reserve(extension_count);

		// Iterate through all extensions and add them to the vector
		for(int k = 0; k < extension_count; k++)
		{
			extensions.emplace_back(reinterpret_cast<const char*>(glGetStringi(GL_EXTENSIONS, k)));
		}

		return extensions;
	}

	//-------------------------------------------------------------------------------------

	const GraphicsContext::Capabilities GraphicsContext::get_hardware_capabilities() const
	{
		CBN_Assert(is_initialized(), "Cannot get the OpenGL hardware capabilities without first initializing the context");

		//TODO: fill in the capabilities struct and actually get them
		return Capabilities{};
	}

	//-------------------------------------------------------------------------------------

	const Version GraphicsContext::get_opengl_version() const
	{
		CBN_Assert(is_initialized(), "Cannot get the OpenGL context version without first initializing the context");

		return m_ContextVersion;
	}

	//-------------------------------------------------------------------------------------

	std::stack<Error> GraphicsContext::get_errors() const
	{
		CBN_Assert(is_initialized(), "Cannot get retrieve errors without first initializing the context");

		std::stack<Error> errors;
		GLenum error_code;

		// Grab OpenGL errors until there are no more errors left,
		// resulting in an ordered stack of errors which has occurred. 
		while((error_code = glGetError()) != GL_NO_ERROR)
		{
			errors.emplace(error_code, generate_opengl_error_message(error_code));
		}

		return errors;
	}

	//-------------------------------------------------------------------------------------

	std::string GraphicsContext::get_device_name() const
	{
		CBN_Assert(is_initialized(), "Cannot get device name without first initializing the context");

		return std::string(reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
	}
	
	//-------------------------------------------------------------------------------------

	void GraphicsContext::bind_to_current_thread() const
	{
		CBN_Assert(is_initialized(), "Cannot bind current thread to context without first initializing the context");

		glfwMakeContextCurrent(m_AssociatedWindow->get_handle());
	}
	
	//-------------------------------------------------------------------------------------

	bool GraphicsContext::is_initialized() const
	{
		return m_Initialized;
	}

	//-------------------------------------------------------------------------------------

	std::string GraphicsContext::get_driver_vendor() const
	{
		CBN_Assert(is_initialized(), "Cannot get driver vendor without first initializing the context");

		return std::string(reinterpret_cast<const char*>(glGetString(GL_VENDOR)));
	}

	//-------------------------------------------------------------------------------------

	Window& GraphicsContext::get_render_window()
	{
		CBN_Assert(is_initialized(), "Cannot get render window without first initializing the context");
		
		return *m_AssociatedWindow.get();
	}

	//-------------------------------------------------------------------------------------
}
