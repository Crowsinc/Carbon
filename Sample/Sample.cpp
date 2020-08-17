
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

#include <Graphics/Resources/Texture.hpp>
#include <Graphics/Resources/TextureAtlas.hpp>

bool runflag = true;
cbn::URes<cbn::Window> window;

const char* vertex_source = "#version 400 core\n"
"layout(location = 0) in vec4 VertexData;\n"
"layout(location = 1) in vec2 uvs;\n"
"out vec2 _uvs;\n"
"void main(void)\n"
"{\n"
"	gl_Position = vec4(VertexData.xy, 0.0, 1.0);\n"
"	_uvs = uvs;\n"
"}\n"
";";

const char* fragment_source = "#version 400 core\n"
"in vec2 _uvs;\n"
"out vec4 fragColour;\n"
"uniform sampler2D text;"
"void main(void)\n"
"{\n"
"	fragColour = texture(text, _uvs);\n"
"   //fragColour = vec4(1, 1, 1, 1);\n"
"}\n"
";";

struct VertexData
{
	glm::vec2 uv;
};


void test_atlas()
{
	const std::filesystem::path path = "res/";

	std::vector<cbn::SRes<cbn::Image>> images;

	for(const auto& entry : std::filesystem::directory_iterator(path))
	{
		std::filesystem::path image_path = entry.path().string();

		cbn::SRes<cbn::Image> img = cbn::Image::Open(image_path);
		if(!img)
		{
			std::cout << "Could not load image " << image_path.string() << std::endl;
			continue;
		}

		images.emplace_back(std::move(img));
	}

	std::cout << "Packing... " << images.size() << " textures" << std::endl;

	auto atlas = cbn::TextureAtlas::Pack(4096, 4096, {}, images);

}

int main()
{


	cbn::Window::Properties props{};
	
	// Window properties
	props.title = "Carbon Sample";
	props.resolution = {1920,1080};
	props.display_mode = cbn::Window::DisplayMode::WINDOWED;
	
	// Window graphics API properties
	props.opengl_version = {4,6,0};

#ifdef _DEBUG
	props.opengl_debug = true;
	props.vsync = true;

#else
	props.opengl_debug = false;
	props.vsync = true;
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
	if(!vertex_sh)
	{
		std::cout << error_log << std::endl;
	}

	const auto frag_sh = cbn::Shader::Compile(fragment_source, cbn::Shader::Stage::FRAGMENT, error_log);
	if(!frag_sh)
	{
		std::cout << error_log << std::endl;
	}

	auto program = cbn::ShaderProgram::Create(vertex_sh, nullptr, frag_sh, error_log);
	if(!program)
	{
		std::cout << error_log << std::endl;
	}


	std::vector<cbn::Transform> transforms;
	//for(int x = 0; x < props.resolution.x; x += 32)
	//{
	//	for(int y = 0; y < props.resolution.y; y += 32)
	//	{
	//		transforms.emplace_back(glm::vec2{x+ 2,y + 2});
	//	}
	//}

	transforms.emplace_back(glm::vec2{4096/2,4096/2});


	VertexData tmp = {};
	
	cbn::VertexDataDescriptor descriptor;
	descriptor.add_attribute(tmp.uv, false);
	
	cbn::QuadRenderer<VertexData> renderer(descriptor);

	glm::vec2 base_size(4096, 4096);

	VertexData ul = {{0, 1}};
	VertexData ur = {{1, 1}};
	VertexData ll = {{0, 0}};
	VertexData lr = {{1, 0}};

	cbn::Texture::Properties tprops{};
	tprops.swizzle = cbn::Swizzle::RGBA;

	//cbn::Res<cbn::Texture> texture = cbn::Texture::Open("test.png", tprops);
	//if(!texture.exists())
	//{
	//	std::cout << "Texture load failed" << std::endl;
	//	std::cin.get();
	//	return 0;
	//}

	test_atlas();


	cbn::Camera cam(4096, 4096);

	cbn::Stopwatch watch;
	watch.start();

	int frames = 0;

	//texture->bind();

	while(runflag)
	{
		glClear(GL_COLOR_BUFFER_BIT);

		renderer.begin(cam);
	 
		int i = 0;
		for(auto& trans : transforms)
		{
		//	trans.rotate_by(1);
			renderer.submit(base_size, trans.to_transform_matrix(), {ul, ll, lr, ur});
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