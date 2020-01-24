
#include <Graphics/GraphicsContext.hpp>
#include <Graphics/Window.hpp>
#include <Utility/Type.h>

#include <algorithm>
#include <iostream>
#include <thread>

bool runflag = true;
cbn::Ptr<cbn::GraphicsContext> context;
cbn::Ptr<cbn::Window> window;

int main()
{
	// Shader::FromFile("fwafwa", type)
	// Shader::FromString("dwafwa", type);

	// ShaderProgram::Create(shader1, shader2, shader3)

	if((context = cbn::GraphicsContext::Create(cbn::Version(3, 2), false)) != nullptr)
	{
		std::cout << "Context Created!" << std::endl;
	}
	else
	{
		std::cout << "Context Failed To Create!" << std::endl;
		std::cin.get();
	}

	context->ErrorEvent.subscribe([](std::string msg, GLenum type, GLenum severity, GLenum something) {
		std::cout << "Debug Output!" << std::endl;
	});

	if((window = cbn::Window::Create(context)) != nullptr)
	{
		std::cout << "Window Created!" << std::endl;
	}
	else
	{
		std::cout << "Window Failed To Create!" << std::endl;
	}

	window->CloseRequestEvent.subscribe([&]()
	{
		runflag = false;
	});

	window->set_display_mode(cbn::Window::DisplayMode::RESIZABLE);

	window->set_title("Carbon Sample");

	glEnable(GL_TEXTURE_1D);

	while(runflag)
	{
		
		std::this_thread::sleep_for(std::chrono::milliseconds(16));

		// Update the window
		window->update();
	}
	
	return 0;
}