#include "GraphicsContext.hpp"

#include "../Diagnostics/Assert.hpp"

namespace cbn
{
	//-------------------------------------------------------------------------------------

	int GraphicsContext::s_ContextCount = 0;
	bool GraphicsContext::s_OpenGLLoaded = false;

	//-------------------------------------------------------------------------------------

	Ptr<GraphicsContext> GraphicsContext::Create(const Version& context_version, bool debug_context)
	{
		
		// Attempt to initialize GLFW, error out from the create function if GLFW
		// could not be initialized. Note that if it has already been initialized,
		// then the init function will return true immediately
		if(glfwInit() == GLFW_FALSE)
		{
			// Initialisation failed, return nullptr
			return nullptr;
		}

		// Set hints so that we create a core OpenGL context of the given context version 
        // Note that if the version is not supported, then the initialisation will simply fail
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, context_version.get_major_version());
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, context_version.get_minor_version());
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		
		
		// If debug context is set then we will attempt to create a debug context for debug output
		// This is standard as of OpenGL 4.3, however is still supported if the GL_ARB_debug_output
		// extension is supported. We will have to query for its support after context creation
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, debug_context ? GLFW_TRUE : GLFW_FALSE);

		// Since context creation is connected with window creation in GLFW
		// specify hints for creating an invisible non-resizable window 
		glfwWindowHint(GLFW_FOCUS_ON_SHOW, GLFW_TRUE);
		glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
		glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

		// Create a GLFW window and store the context/window handle within a GraphicsContext
		// instance. If the handle is null, then an error has occurred so return nullptr
		GLFWwindow* context_handle = glfwCreateWindow(1280, 720, "Untitled Window", NULL, NULL);
		
		// Check that the context was created successfully
		if(context_handle == NULL)
		{
			return nullptr;
		}

		// Context creation does not make the context current to the thread, so make it current.
		glfwMakeContextCurrent(context_handle);

		// If the OpenGL functions have not been loaded yet, then we need to load them now
		if(!s_OpenGLLoaded)
		{
			// Load the OpenGL functions, returning nullptr if they could not be loaded
			if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
			{
				// Note that since the load has failed and we are not going
				// to use this context, we want to destroy the context we created
				glfwDestroyWindow(context_handle);
				return nullptr;
			}

			// Update the loaded flag so that the functions are not loaded again
			s_OpenGLLoaded = true;
		}

		// Create the graphics context object which will control the context handle
		Ptr<GraphicsContext> context = Ptr<GraphicsContext>(new GraphicsContext(context_handle, context_version, debug_context));

		// If debug mode was picked, try to initialize the debug output
		if(debug_context)
		{
			// If the context is meant to be a debug context, and its debug output 
			// could not be initialized, then we will consider the context creation
			// to have failed, so return nullptr. Note that when the context goes out
			// of scope, the handle will be cleaned up
			if(!try_initialize_debug_output(context))
			{
				return nullptr;
			}
		}

		// If we have made it here, the context was created successfully so return it
		return context;
	}

	//-------------------------------------------------------------------------------------

	bool GraphicsContext::try_initialize_debug_output(const Ptr<GraphicsContext>& context)
	{
		// Check that the context is actually a debug context
		GLint context_flag;
		glGetIntegerv(GL_CONTEXT_FLAGS, &context_flag);
		if(context_flag & GL_CONTEXT_FLAG_DEBUG_BIT)
		{
			// A debug context was successfully created, however debug ouput is only
			// supported in post OpenGL 4.3. Earlier versions will need to make use
			// of the GLAD_GL_ARB_debug_output extension if supported
			if(context->m_Version >= Version(4, 3))
			{
				// We are using an OpenGL context post OpenGL 4.3 which supports
				// debug output, so simply set up the callback as normal
				glDebugMessageCallback(&GraphicsContext::debug_callback, context.get());
				glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
				glEnable(GL_DEBUG_OUTPUT);
			
				return true;
			}
			else if(GLAD_GL_ARB_debug_output)
			{
				// If we are here, a debug context could not be created so 
				// instead we will try use the GL_ARB_debug_output extension
				glDebugMessageCallbackARB(&GraphicsContext::debug_callback, context.get());
				glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
				glEnable(GL_DEBUG_OUTPUT);
				
				return true;
			}
		}

		// If we get here, then we could not initialize debug output so return false
		return false;
	}

	//-------------------------------------------------------------------------------------

	void GraphicsContext::debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* user_param)
	{
		// Invoke the error event in the context provided by the user param
		auto context = reinterpret_cast<const GraphicsContext*>(user_param);
		context->ErrorEvent.invoke(std::string(message), source, type, severity);
	}

	//-------------------------------------------------------------------------------------

	GraphicsContext::GraphicsContext(GLFWwindow* handle, const Version& version, const bool debug_context)
		: m_GLFWHandle(handle),
		m_Version(version),
		m_DebugContext(debug_context)
	{
		CBN_Assert(handle != nullptr, "Cannot instantiate GraphicsContext with invalid handle");
	
		// Increment the context count by 1
		s_ContextCount++;
	}
	
	//-------------------------------------------------------------------------------------

	GraphicsContext::~GraphicsContext()
	{
		// Reduce the context count by 1
		s_ContextCount--;

		// Destroy the context
		glfwDestroyWindow(m_GLFWHandle);

		// If this was the last existing valid context, then terminate GLFW
		if(s_ContextCount == 0)
		{
			glfwTerminate();
		}
	}
	
	//-------------------------------------------------------------------------------------

	bool GraphicsContext::is_extension_supported(const std::string_view& extension) const
	{
		// Use the GLFW provided function for checking extension support
		return glfwExtensionSupported(extension.data()) == GLFW_TRUE;
	}
	
	//-------------------------------------------------------------------------------------

	void* GraphicsContext::load_function(const std::string_view& function_name) const
	{
		return glfwGetProcAddress(function_name.data());
	}

	//-------------------------------------------------------------------------------------

	std::vector<std::string> GraphicsContext::get_supported_extensions() const
	{
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

		//TODO: fill in the capabilities struct and actually get them
		return Capabilities{};
	}

	//-------------------------------------------------------------------------------------

	const Version GraphicsContext::get_opengl_version() const
	{

		return m_Version;
	}

	//-------------------------------------------------------------------------------------

	std::string GraphicsContext::get_device_name() const
	{

		return std::string(reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
	}
	
	//-------------------------------------------------------------------------------------

	void GraphicsContext::bind_to_current_thread() const
	{

		glfwMakeContextCurrent(m_GLFWHandle);
	}
	
	//-------------------------------------------------------------------------------------

	std::string GraphicsContext::get_driver_vendor() const
	{
		return std::string(reinterpret_cast<const char*>(glGetString(GL_VENDOR)));
	}
	
	//-------------------------------------------------------------------------------------

	bool GraphicsContext::is_debug_context() const
	{
		return m_DebugContext;
	}

	//-------------------------------------------------------------------------------------
}
