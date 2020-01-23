
#include <Graphics/GraphicsContext.hpp>
#include <Graphics/Window.hpp>

#include <algorithm>
#include <iostream>
#include <thread>

bool runflag = true;

int main()
{
	cbn::GraphicsContext context;

	if(context.try_initialize(cbn::Version(4, 6), true))
	{
		std::cout << "Context Created!" << std::endl;
	}
	else
	{
		std::cout << "Context Failed To Create!" << std::endl;
	}

	cbn::Window& window = context.get_render_window();

	window.CloseRequestEvent.subscribe([&]()
	{
		runflag = false;
	});

	window.KeyPressEvent.subscribe([](int i)
	{
		std::cout << "key pressed!" << std::endl;
	});

	window.show();

	window.set_display_mode(cbn::Window::DisplayMode::RESIZABLE);

	window.set_title("Carbon Sample");

	while(runflag)
	{
		
		std::this_thread::sleep_for(std::chrono::milliseconds(16));

		// Update the window
		window.update();
	}
	
	return 0;
}