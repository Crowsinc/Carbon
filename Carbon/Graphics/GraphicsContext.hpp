#pragma once

#include <stack>
#include <vector>
#include <string>
#include <memory>

#include "OpenGL.hpp"

#include "../Utility/Version.hpp"
#include "../Utility/Event.hpp"
#include "../Utility/Type.h"

namespace cbn
{

	class GraphicsContext
	{
		friend class Window;
	public:

		Event<std::string,GLenum, GLenum, GLenum> ErrorEvent;

		// TODO: fill this out
		struct Capabilities
		{

		};

		static Ptr<GraphicsContext> Create(const Version& context_version, bool debug_context = false);

	private:

		static bool s_OpenGLLoaded;
		static int s_ContextCount;

		GLFWwindow* m_GLFWHandle;
		const Version m_Version;
		bool m_DebugContext;
		
		static bool try_initialize_debug_output(const Ptr<GraphicsContext>& context);

		static void debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* user_param);

		GraphicsContext(GLFWwindow* handle, const Version& version, const bool debug_context);
	
	public:

		~GraphicsContext();

		bool is_extension_supported(const std::string_view& extension) const;

		void* load_function(const std::string_view& function_name) const; 

		std::vector<std::string> get_supported_extensions() const;

		const Capabilities get_hardware_capabilities() const;

		const Version get_opengl_version() const;

		std::string get_driver_vendor() const;

		std::string get_device_name() const;
		
		void bind_to_current_thread() const;
		
		bool is_debug_context() const;

	};

}