#pragma once

#include <string>
#include <glm/glm.hpp>

#include "OpenGL.hpp"
#include "../Utility/Event.hpp"
#include "../Utility/Version.hpp"
#include "../Utility/Resource.hpp"

namespace cbn
{

	class Window
	{
	public:
		
		enum class DisplayMode
		{
			BORDERLESS,
			FULLSCREEN,
			RESIZABLE,
			WINDOWED
		};

		Event<std::string, GLenum, GLenum> ErrorEvent;
		Event<DisplayMode> DisplayModeChangeEvent;
		Event<glm::vec2> ResolutionChangeEvent;
		Event<std::string> TitleChangeEvent;
		Event<bool> VisibilityChangeEvent;
		Event<void> CloseRequestEvent;
		Event<bool> FocusChangeEvent;
		Event<bool> VSyncChangeEvent;

		struct Properties
		{
			bool vsync;
			std::string title;
			glm::vec2 resolution;
			DisplayMode display_mode;
		
			bool opengl_debug;
			Version opengl_version;
		};

		static Res<Window> Create(Properties window_properties);

	private:

		static bool s_OpenGLLoaded;
		DisplayMode m_DisplayMode;
		GLFWwindow* m_GLFWHandle;
		Version m_OpenGLVersion;
		glm::vec2 m_Resolution;
		std::string m_Title;
		bool m_OpenGLDebug;
		bool m_VSync;

		static void gl_error_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* user_param);

		static void glfw_resize_callback(GLFWwindow* glfw_handle, int width, int height);

		static void glfw_focus_callback(GLFWwindow* glfw_handle, int is_focused);

		static void glfw_close_callback(GLFWwindow* glfw_handle);

		static void glfw_set_display_mode(GLFWwindow* glfw_handle, DisplayMode display_mode);

		static GLFWmonitor* glfw_find_window_monitor(GLFWwindow* glfw_handle);

		Window(GLFWwindow* glfw_handle, Properties properties);

		static void Delete(Window& window);
		
	public:

		Window(const Window& window) = delete;

		Window(Window&& window);

		void show();

		void hide();

		void focus();

		void update();

		bool is_visible() const;

		bool is_focused() const;

		bool is_vsync_enabled() const;

		bool is_debug_enabled() const;

		glm::vec2 get_resolution() const;

		std::string_view get_title() const;

		DisplayMode get_display_mode() const;

		const Version get_opengl_version() const;

		void set_vsync(const bool enable_vsync);

		void set_title(const std::string& title);

		void set_resolution(const glm::vec2& resolution);

		void set_display_mode(const DisplayMode display_mode);
		
		void operator=(Window&& window);

	};

}