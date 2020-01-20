#pragma once

#include "OpenGL.hpp"

namespace cbn
{

	class Window
	{
		friend class GraphicsContext;
	public:


	private:


		Window();

		bool try_initialize(const GLFWwindow* window_handle);

	public:

		GLFWwindow* get_handle() const;

	};

}