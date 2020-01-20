#pragma once

#include <stack>
#include <vector>
#include <string>

#include "OpenGL.hpp"

#include "../Utility/Version.hpp"
#include "../Utility/Event.hpp"
#include "../Utility/Error.hpp"

namespace cbn
{

	class GraphicsContext
	{
	public:

		//TODO:
		Event<std::string> ErrorEvent;

		struct Capabilities
		{

		};

	private:

		bool m_Initialized;
	
		std::string generate_opengl_error_message(const GLenum gl_error_code) const;

	public:

		GraphicsContext();

		~GraphicsContext();

		bool try_initialize(const Version& context_version = Version(0, 0), bool debug_mode = false);

		bool is_extension_supported(const std::string_view& extension) const;

		std::vector<std::string> get_supported_extensions() const;

		const Capabilities get_hardware_capabilities() const;

		const Version get_opengl_version() const;

		std::string get_driver_vendor() const;

		std::stack<Error> get_errors() const;

		std::string get_device_name() const;
		
		bool is_initialized() const;
		
	};

}