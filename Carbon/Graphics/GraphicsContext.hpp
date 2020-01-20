#pragma once

#include <stack>
#include <vector>
#include <string>
#include <memory>

#include "OpenGL.hpp"
#include "Window.hpp"

#include "../Utility/Version.hpp"
#include "../Utility/Event.hpp"
#include "../Utility/Error.hpp"

namespace cbn
{

	class GraphicsContext
	{
	public:

		//TODO: update error class and update this to use that
		Event<std::string,GLenum, GLenum, GLenum> ErrorEvent;

		// TODO: fill this out
		struct Capabilities
		{

		};

	private:

		static int s_ContextCount;
		static bool s_GLFWInitialized;
		static bool s_OpenGLFunctionsLoaded;

		Version m_ContextVersion;
		Window m_AssociatedWindow;
		bool m_Initialized;
	
		static void debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* user_param);

		std::string generate_opengl_error_message(const GLenum gl_error_code) const;

	public:

		GraphicsContext();

		~GraphicsContext();

		bool try_initialize(const Version& context_version, bool debug_mode = false);

		bool is_extension_supported(const std::string_view& extension) const;

		void* load_function(const std::string_view& function_name) const; 

		std::vector<std::string> get_supported_extensions() const;

		const Capabilities get_hardware_capabilities() const;

		const Version get_opengl_version() const;

		std::string get_driver_vendor() const;

		std::stack<Error> get_errors() const;

		std::string get_device_name() const;
		
		void bind_to_current_thread() const;

		bool is_initialized() const;

		Window& get_render_window();
		
	};

}