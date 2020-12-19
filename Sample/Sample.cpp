#define CBN_DISABLE_ASSERTS

#include <Graphics/Window.hpp>
#include <Carbon.hpp>
#include <Diagnostics/Stopwatch.hpp>

#include <Maths/Maths.hpp>

#include <algorithm>
#include <iostream>
#include <thread>

#include <Graphics/SpriteRenderer.hpp>
#include <Graphics/Resources/Shader.hpp>
#include <Graphics/Resources/ShaderProgram.hpp>
#include <Graphics/OpenGL/VertexArrayObject.hpp>

#include <Data/Identity/Identifier.hpp>

#include <Graphics/Resources/Texture.hpp>
#include <Graphics/Resources/TextureAtlas.hpp>

bool runflag = true;
cbn::URes<cbn::Window> window;

using namespace cbn;

SRes<TextureAtlas> build_atlas()
{
	const std::filesystem::path path = "res/";

	IdentityMap<SRes<Image>> images;

	for(const auto& entry : std::filesystem::directory_iterator(path))
	{
		std::filesystem::path image_path = entry.path().string();

		SRes<Image> img = Image::Open(image_path);
		if(!img)
		{
			std::cout << "Could not load image " << image_path.string() << std::endl;
			continue;
		}

		images.emplace(entry.path().filename().string(), std::move(img));
	}

	std::cout << "Packing... " << images.size() << " textures" << std::endl;

	auto atlas = cbn::TextureAtlas::Pack(4096, 4096, images);
	return atlas;
}

int main()
{

	cbn::Window::Properties props{};

	// Window properties
	props.title = "Carbon Sample";
	props.resolution = {1920,1080};
	props.display_mode = cbn::Window::DisplayMode::WINDOWED;

	// Window graphics API properties
	props.opengl_version = {3,3,0};

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

	std::string gpu = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
	std::cout << "GPU: " << gpu << std::endl;

	int i = 0;
	glGetIntegerv(GL_MAX_TEXTURE_BUFFER_SIZE, &i);
	std::cout << "Max TBO Texels: " << std::to_string(i) << std::endl;

	auto [vertex_sh, error_log1] = cbn::Shader::Open("res/VertexShader.glsl", cbn::Shader::Stage::VERTEX);
	if(!vertex_sh)
	{
		std::cout << error_log1 << std::endl;
	}

	auto [frag_sh, error_log2] = cbn::Shader::Open("res/FragmentShader.glsl", cbn::Shader::Stage::FRAGMENT);
	if(!frag_sh)
	{
		std::cout << error_log2 << std::endl;
	}

	auto [program, error_log3] = cbn::ShaderProgram::Create(vertex_sh, nullptr, frag_sh);
	
	if(!program)
	{
		std::cout << error_log3 << std::endl;
	}
	CBN_Assert(program != nullptr, "Failed shader creation");

	//TODO: find a better way to do this
	program->bind();
	if(program->has_uniform({"samplers[0]"}))
		for(int i = 0; i < 15; i++)
			program->set_uniform("samplers[" + String{std::to_string(i)} + "]", i + 1);


	cbn::SpriteRendererProperties prop;
	prop.buffer_allocation_bias = 32;
	prop.sprites_per_batch = 4096;

	//TODO: fix having to do this
	glViewport(0, 0, 1920, 1080);

	cbn::SpriteRenderer renderer(window->get_opengl_version(),{}, prop);
	cbn::TexturePack texture_pack(window->get_opengl_version());

	cbn::SRes<cbn::Texture> rock_texture = cbn::Texture::Open("res/rock.png");
	cbn::SRes<cbn::Texture> ground_texture = cbn::Texture::Open("res/ground.png");

	CBN_Assert(rock_texture != nullptr && ground_texture != nullptr, "Big fail");

	texture_pack = {
		TexturePackEntry{"rock", rock_texture},
		TexturePackEntry{"ground", ground_texture}
	};

	renderer.set_texture_pack(texture_pack);

	//const glm::vec2 size = {1,1};
	const glm::vec2 size = {3.5f,3.5f};
	//const glm::vec2 size = {8,8};
	//const glm::vec2 size = {32,32};
	const glm::vec2 padding = {1,1};

	const int max_sprites = 100000;

	std::vector<BoundingBox> sprites;
	std::vector<Identifier> textures;
	for(float x = size.x / 2; x < 1920; x += size.x + padding.x)
	{
		for(float y = size.y / 2; y < 1080; y += size.y + padding.y)
		{
			auto& sprite = sprites.emplace_back();
			sprite.size = size;
			sprite.transform.translate_to({x,y});

			textures.push_back(rand() % 2 == 0 ? "rock" : "ground");

			if(sprites.size() == max_sprites)
				break;
		}
		if(sprites.size() == max_sprites)
			break;
	}

	window->set_title("Carbon Sample | Sprites: " + std::to_string(sprites.size()));
	
	cbn::Camera scene_camera(1920, 1080);
	cbn::Stopwatch watch;
	watch.start();

	int frames = 0;

	while(runflag)
	{
		glClear(GL_COLOR_BUFFER_BIT);

		int i = 0;
		while(i < sprites.size())
		{
			renderer.begin_batch(scene_camera);
			const int amount = prop.sprites_per_batch < sprites.size() - i ? prop.sprites_per_batch : sprites.size() - i;
			for(int j = 0; j < amount; j++)
			{
				renderer.submit(sprites[i], textures[j]);
				i++;
			}
			renderer.end_batch();
			renderer.render(program);
		}

		// Update the window
		window->update();

		frames++;
		if(watch.get_elapsed_time(cbn::Seconds) > 1)
		{
			window->set_title("Carbon Sample  |  Sprites: " + std::to_string(sprites.size()) + "  |  " + std::to_string(frames) + " fps  |  " + std::to_string(1 / (float)frames) + "ms");
			frames = 0;
			watch.restart();
		}

	}

	return 0;
}