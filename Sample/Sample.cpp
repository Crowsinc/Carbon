
#define CBN_DISABLE_ASSERTS

#include <Graphics/Window.hpp>
#include <Carbon.hpp>
#include <Diagnostics/Stopwatch.hpp>

#include <Maths/Maths.hpp>

#include <algorithm>
#include <iostream>
#include <thread>

#include <Graphics/Resources/Shader.hpp>
#include <Graphics/QuadRenderer.hpp>
#include <Graphics/Resources/ShaderProgram.hpp>

bool runflag = true;
cbn::Res<cbn::Window> window;

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
	cbn::Window::Properties props{};
	
	// Window properties
	props.title = "Carbon Sample";
	props.resolution = {1280,720};
	props.display_mode = cbn::Window::DisplayMode::WINDOWED;
	
	// Window graphics API properties
	props.opengl_version = {4,6,0};

#ifdef _DEBUG
	props.opengl_debug = true;
	props.vsync = true;

#else
	props.opengl_debug = false;
	props.vsync = false;
#endif

	window = cbn::Window::Create(props);
	if(window)
	{
		std::cout << "Window Created!" << std::endl;
	}
	else
	{
		std::cout << "Window Failed To Create!" << std::endl;
	}


	window->ErrorEvent.subscribe([](std::string msg, GLenum source, GLenum severity)
	{
		std::cout << msg << std::endl;
	});

	window->CloseRequestEvent.subscribe([&]()
	{
		runflag = false;
	});

	window->set_title("Carbon Sample");

	// Should create an error
	glEnable(GL_TEXTURE_1D);

	std::string error_log;

	const auto vertex_sh = cbn::Shader::Compile(vertex_source, cbn::Shader::Stage::VERTEX, error_log);
	if(!vertex_sh.exists())
	{
		std::cout << error_log << std::endl;
	}

	const auto frag_sh = cbn::Shader::Compile(fragment_source, cbn::Shader::Stage::FRAGMENT, error_log);
	if(!frag_sh.exists())
	{
		std::cout << error_log << std::endl;
	}

	auto program = cbn::ShaderProgram::Create(vertex_sh, nullptr, frag_sh, error_log);
	if(!program.exists())
	{
		std::cout << error_log << std::endl;
	}

	VertexData tmp = {};

	cbn::VertexDataDescriptor descriptor;
	descriptor.add_attribute(tmp.colour, false);

	cbn::QuadRenderer<VertexData> renderer(descriptor);

	std::vector<cbn::Transform> transforms;
	for(int x = 0; x < props.resolution.x; x += 8)
	{
		for(int y = 0; y < props.resolution.y; y += 8)
		{
			transforms.emplace_back(glm::vec2{x+ 2,y + 2});
		}
	}


	glm::vec2 base_size(8);

	VertexData data[3];
	data[0] = {glm::vec4(0.5f, 0.8f, 1, 1)};
	data[1] = {glm::vec4(0.8f, 0.2f, 0.4f, 1)};
	data[2] = {glm::vec4(0.2f, 0.3f, 0.4f, 1)};

	cbn::Camera cam(1280, 720);

	cbn::Stopwatch watch;
	watch.start();

	int frames = 0;

	while(runflag)
	{
		renderer.begin(cam);
	 
		int i = 0;
		for(auto& trans : transforms)
		{
			trans.rotate_by(2);
			renderer.submit(base_size, trans.to_transform_matrix(), data[i % 3]);
			i++;
		}

		renderer.end();
		renderer.render(program);

		// Update the window
		window->update();

		frames++;
		if(watch.get_elapsed_time(cbn::Seconds) > 1)
		{
			std::cout << frames << std::endl;
			frames = 0;
			watch.restart();
		}

	}
	
	return 0;
}