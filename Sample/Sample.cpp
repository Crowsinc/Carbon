
#include <Graphics/GraphicsContext.hpp>
#include <Graphics/Window.hpp>

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

	window.CloseRequestEvent.add_callback([&]()
	{
		runflag = false;
	});

	window.KeyPressEvent.add_callback([](int i)
	{
		std::cout << "key pressed!" << std::endl;
	});

	window.show();

	window.set_title("Carbon Sample");

	while(runflag)
	{
		
		std::this_thread::sleep_for(std::chrono::milliseconds(16));

		// Update the window
		window.update();
	}
	
	std::cin.get();
	return 0;
}