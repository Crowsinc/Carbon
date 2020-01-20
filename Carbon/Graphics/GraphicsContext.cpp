#include "GraphicsContext.hpp"


namespace cbn
{

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

	GraphicsContext::GraphicsContext() {}
	
	//-------------------------------------------------------------------------------------

	GraphicsContext::~GraphicsContext() {}
	
	//-------------------------------------------------------------------------------------

	bool GraphicsContext::try_initialize(const Version & context_version, bool debug_mode)
	{
		// Check that the required version is supported

		//TODO: error out in other methods below if the context is not initialized
	}
	
	//-------------------------------------------------------------------------------------

	bool GraphicsContext::is_extension_supported(const std::string_view& extension) const
	{
		// Get all the supported extensions and search for the given one
		const auto supported_extensions = get_supported_extensions();
		return std::find(supported_extensions.begin(), supported_extensions.end(), extension) != supported_extensions.end();
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
		GLint major_version = 0 , minor_version = 0;

		// Get the major and minor OpenGL versions and return them
		// within a version class. Note that this functionality is
		// not supported before OpenGL 3.0 which may cause issues
		glGetIntegerv(GL_MAJOR_VERSION, &major_version);
		glGetIntegerv(GL_MINOR_VERSION, &minor_version);

		return Version(major_version, minor_version);
	}

	//-------------------------------------------------------------------------------------

	std::stack<Error> GraphicsContext::get_errors() const
	{
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
		return std::string(reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
	}
	
	//-------------------------------------------------------------------------------------

	bool GraphicsContext::is_initialized() const
	{
		return m_Initialized;
	}

	//-------------------------------------------------------------------------------------

	std::string GraphicsContext::get_driver_vendor() const
	{
		return std::string(reinterpret_cast<const char*>(glGetString(GL_VENDOR)));
	}

	//-------------------------------------------------------------------------------------
}
