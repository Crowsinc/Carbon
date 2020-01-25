
#include <Graphics/GraphicsContext.hpp>
#include <Graphics/Window.hpp>
#include <Utility/Type.h>

#include <algorithm>
#include <iostream>
#include <thread>

#include <Graphics/Shader.hpp>
#include <Graphics/QuadRenderer.hpp>
#include <Graphics/ShaderProgram.hpp>

bool runflag = true;
cbn::Ptr<cbn::GraphicsContext> context;
cbn::Ptr<cbn::Window> window;

const char* vertex_source = "#version 400 core\n"
"layout(location = 0) in vec4 VertexData;\n"
"layout(location = 1) in vec4 Colour;\n"
"out vec4 color;\n"
"void main(void)\n"
"{\n"
"	gl_Position = vec4(VertexData.xy, 0.0, 1.0);\n"
"	color = Colour;\n"
"}\n"
";";

const char* fragment_source = "#version 400 core\n"
"in vec4 color;\n"
"out vec4 fragColour;\n"
"void main(void)\n"
"{\n"
"	fragColour = color;\n"
"}\n"
";";

struct VertexData
{
	glm::vec4 colour;
};

int main()
{

	if((context = cbn::GraphicsContext::Create(cbn::Version(4, 6), false)) != nullptr)
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

	// Should create an error
	glEnable(GL_TEXTURE_1D);

	std::string error_log;

	const auto vertex_sh = cbn::Shader::Create(vertex_source, cbn::Shader::Stage::VERTEX, error_log);
	if(vertex_sh == nullptr)
	{
		std::cout << error_log << std::endl;
	}

	const auto frag_sh = cbn::Shader::Create(fragment_source, cbn::Shader::Stage::FRAGMENT, error_log);
	if(frag_sh == nullptr)
	{
		std::cout << error_log << std::endl;
	}

	const auto program = cbn::ShaderProgram::Create(vertex_sh, nullptr, frag_sh, error_log);
	if(program == nullptr)
	{
		std::cout << error_log << std::endl;
	}

	cbn::QuadRenderer<VertexData> renderer({cbn::QuadRenderer<VertexData>::LayoutAttribute{false, GL_FLOAT, 4}});

	cbn::Transform trans({0,0});
	glm::vec2 base_size(128);

	VertexData data{glm::vec4(0.5f,0.8f,1,1)};

	cbn::Camera cam(1280, 720);

	while(runflag)
	{
		renderer.begin(cam);
	
		renderer.submit(base_size, trans.to_transform_matrix(), data);

		renderer.end();
		renderer.render(*program);

		// Update the window
		window->update();
		std::this_thread::sleep_for(std::chrono::milliseconds(16));
	}
	
	return 0;
}