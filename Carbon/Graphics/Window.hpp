#pragma once

#include <glm/glm.hpp>

#include "../Utility/Event.hpp"
#include "GraphicsContext.hpp"
#include "OpenGL.hpp"

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
			WINDOWED,
		};

		Event<DisplayMode> DisplayModeChangeEvent;
		Event<glm::vec2> ResolutionChangeEvent;
		Event<std::string> TitleChangeEvent;
		Event<bool> VisibilityChangeEvent;
		Event<void> CloseRequestEvent; 
		Event<bool> VSyncChangeEvent;
		Event<bool> FocusEvent;
		
		static Ptr<Window> Create(Ptr<GraphicsContext>& graphics_context);

	private:

		GLFWwindow* m_ContextHandle;
		DisplayMode m_DisplayMode;
		glm::vec2 m_Resolution;
		std::string m_Title;
		bool m_VSyncEnabled;

		static void window_resize_callback(GLFWwindow* window_handle, int width, int height);

		static void window_focus_callback(GLFWwindow* window_handle, int is_focused);

		static void window_close_callback(GLFWwindow* window_handle);

		GLFWmonitor* determine_monitor();

		void make_fullscreen();

		void make_borderless();

		void make_resizable();

		void make_windowed();

		Window(GLFWwindow* context_handle);

	public:

		~Window();

		void show();

		void hide();

		void focus();

		void update();

		bool is_visible() const;

		bool is_focused() const;
		
		bool is_vsync_enabled() const;
		
		glm::vec2 get_resolution() const;

		std::string_view get_title() const;
		
		void set_vsync(const bool enable_vsync);
		
		void set_title(const std::string& title);
		
		void set_resolution(const glm::vec2& resolution);
		
		void set_display_mode(const DisplayMode display_mode);

		//TODO: void set_monitor(const GLFWmonitor& monitor); get_monitor etc.

		//TODO: void set_icon(Image& image); get_icon etc. // complete when we make an image util class
	};

}