#define CBN_DISABLE_ASSERTS

#include <algorithm>
#include <iostream>
#include <thread>

#include <Carbon.hpp>
#include <glm/gtx/norm.hpp>

#ifdef _WIN32
#include <Windows.h>
#endif


using namespace cbn;

//-------------------------------------------------------------------------------------

enum class Scene
{
	STATIC_RENDER,
	DYNAMIC_RENDER,
	BOUNDS_TEST
};

void print_info();

void print(const String& str);

URes<Window> create_window();

SRes<ShaderProgram> load_program(const String& vertex_name, const String& fragment_name);

TexturePack load_textures(const URes<Window>& window, const std::map<Identifier, String> textures);

Scene main_menu_scene(URes<Window>& window, bool& runflag);

std::vector<cbn::Rectangle> create_screen_sprites(const Camera& camera, const uint64_t sprite_count);

void static_render_scene(URes<Window>& window, bool& runflag);

void move_sprite(cbn::Rectangle& sprite, const glm::vec2& mouse_pos);

void dynamic_render_scene(URes<Window>& window, bool& runflag);

//void bounds_test_scene(URes<Window>& window, bool& runflag);

//-------------------------------------------------------------------------------------

constexpr bool DEBUG = false;

int main()
{
	// Create the render window
	URes<Window> window = create_window();
	if(window)
		print("Successfully created window");
	else
		print("Failed to create window");

	// If debugging is enabled, subscribe to the error event
	Subscription error_subscription;
	if(window->is_debug_enabled())
	{
		print("Subscribing to OpenGL error event");
		error_subscription = window->ErrorEvent.subscribe(
			[](std::string msg, GLenum source, GLenum severity)
			{
				if(severity != GL_DEBUG_SEVERITY_NOTIFICATION)
					print(msg);
			}
		);
	}

	// Print common capabilities & hardware info
	print_info();

	// The loops and scenes will run while the runflag is asserted. 
	// Subscribe to the close event so that we can stop the sample
	// from running when the window is requested to close. 
	bool runflag = true;
	Subscription close_subscription = window->CloseEvent.subscribe([&]()
	{
		runflag = false;
	});

	// Render a main menu that allows the user to pick what scene to test
	// When the scene is being used, we go fall back to the main menu.
	while(runflag)
	{
		// Run the main menu
		auto next_scene = main_menu_scene(window, runflag);
		
		// Start and run the correct scene.
		switch(next_scene)
		{
			case Scene::STATIC_RENDER:
				static_render_scene(window, runflag);
				break;
			case Scene::DYNAMIC_RENDER:
				dynamic_render_scene(window, runflag);
				break;
			case Scene::BOUNDS_TEST:
				std::cout << "BOUNDS TEST IS DISABLED" << std::endl;
				//bounds_test_scene(window, runflag);
				break;
		}

	}

}

//-------------------------------------------------------------------------------------

void print(const String& str)
{
	std::cout << Time::Timestamp() << " | " << str << std::endl;
}

//-------------------------------------------------------------------------------------

void print_info()
{
	// Print OpenGL version being used
	String version = reinterpret_cast<const char*>(glGetString(GL_VERSION));
	print("OpenGL Version: " + version);

	// Print GPU being used
	String gpu = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
	print("GPU: " + gpu);

	// Print TBO support
	int i = 0;
	glGetIntegerv(GL_MAX_TEXTURE_BUFFER_SIZE, &i);
	print("Max TBO Texels: " + std::to_string(i));
}

//-------------------------------------------------------------------------------------

URes<Window> create_window()
{
	// Set up the window's properties
	Window::Properties properties;
	properties.title = "Carbon Sample";
	properties.resolution = {1920,1080};
	properties.display_mode = Window::DisplayMode::WINDOWED;
	properties.opengl_version = {4,6,0};
	properties.opengl_debug = DEBUG;
	properties.vsync = false;

	return Window::Create(properties);
}

//-------------------------------------------------------------------------------------

SRes<ShaderProgram> load_program(const String& vertex_name, const String& fragment_name)
{
	// Load the vertex shader
	const std::string vertex_shader_path = "res/shaders/" + vertex_name;
	auto [vertex_shader, log_1] = Shader::Open(vertex_shader_path, Shader::Stage::VERTEX);
	if(!vertex_shader)
		print("Failed to load " + vertex_name + " due to:\n\t" + log_1);

	// Load the fragment shader
	const std::string fragment_shader_path = "res/shaders/" + fragment_name;
	auto [fragment_shader, log_2] = Shader::Open(fragment_shader_path, Shader::Stage::FRAGMENT);
	if(!fragment_shader)
		print("Failed to load " + fragment_name + " due to:\n\t" + log_2);

	// Create the program
	auto [program, log_3] = ShaderProgram::Create(vertex_shader, nullptr, fragment_shader);
	if(!program)
		print("Failed to create program that uses " + vertex_name + "/" + fragment_name + " due to:\n\t" + log_3);

	//TODO: find a better way to do this
	// Initialize the program's samplers if it has them
	program->bind();
	if(program->has_uniform({"samplers[0]"}))
		for(int i = 0; i < 15; i++)
			program->set_uniform("samplers[" + String{std::to_string(i)} + "]", i + 1);

	return program;
}

//-------------------------------------------------------------------------------------

TexturePack load_textures(const URes<Window>& window, const std::map<Identifier, String> textures)
{
	// Load each texture into an array
	std::array<TexturePackEntry, TexturePack::SupportedTextureCount> entries{};

	for(int i = 0; const auto& [id, name] : textures)
	{
		if(i <= entries.size() - 1)
		{
			const std::string full_path = "res/textures/" + name;

			entries[i].identifier = id;
			entries[i].texture = Texture::Open(full_path);
		}
		else print("Could not fit texture with ID: " + id.alias() + " and name: " + name);
		
		i++;
	}

	return TexturePack{entries, window->get_opengl_version()};
}

//-------------------------------------------------------------------------------------

struct SceneButton
{
	cbn::Rectangle bounding_box;
	Identifier texture_id;
	Scene linked_scene;

	SceneButton(const glm::vec2& pos, const glm::vec2& size, const Identifier& texture_id, const Scene scene)
		: bounding_box(Transform{pos}, size),
		  texture_id(texture_id),
		  linked_scene(scene) {}
};

//-------------------------------------------------------------------------------------

Scene main_menu_scene(URes<Window>& window, bool& runflag)
{
	// Create the standard renderer and scene camera
	// Note that the camera is centred at (0,0). 
	SpriteRenderer renderer(window->get_opengl_version());
	Camera camera(window->get_resolution());

	// Create the buttons
	const glm::vec2 button_size{256, 128};
	std::array<SceneButton, 2> buttons
	{
		SceneButton{{0, 128}, button_size, "SRTButton", Scene::STATIC_RENDER},
		SceneButton{{0,-128}, button_size, "DRTButton", Scene::DYNAMIC_RENDER}
	};

	// Load button textures and shaders
	auto tint_program = load_program("TintVertShader.glsl", "TintFragShader.glsl");
	static const auto texture_pack = load_textures(window, {
		{buttons[0].texture_id, buttons[0].texture_id.alias() + ".png"},
		{buttons[1].texture_id, buttons[1].texture_id.alias() + ".png"},
	});
	renderer.set_texture_pack(texture_pack);

	window->set_vsync(true);

	const glm::uvec4 select_colour{Red.red, Red.green, Red.blue, Red.alpha};
	const glm::uvec4 normal_colour{White.red, White.green, White.blue, White.alpha};

	while(runflag)
	{
		window->clear();

		// Get mouse position. Note that we need to offset it as screen coordinates
		// have (0,0) top left, while its in the middle for us. 
		double pos_x, pos_y;
		glfwGetCursorPos(window->TEMP_HANDLE(), &pos_x, &pos_y);
		const glm::vec2 mouse_pos{pos_x - (camera.resolution().x * 0.5f), (camera.resolution().y * 0.5f) - pos_y};

		// Render the buttons
		renderer.begin_batch(camera);

		for(const auto& button : buttons)
		{
			const bool contains_mouse = contains(button.bounding_box, mouse_pos);
			const auto colour = contains_mouse ? select_colour : normal_colour;
			const auto& button_mesh = button.bounding_box.mesh();

			renderer.submit(button_mesh, button.texture_id, colour);
		}

		renderer.end_batch();
		renderer.render(tint_program);

		// If the mouse is clicked, then we need to test if any of the buttons where clicked
		if(GLFW_PRESS == glfwGetMouseButton(window->TEMP_HANDLE(), GLFW_MOUSE_BUTTON_LEFT))
		{
			for(const auto& button : buttons)
			{
				// If a button was clicked, change the scene
				if(contains(button.bounding_box,mouse_pos))
					return button.linked_scene;
			}
		}

		window->update();
	}
}

//-------------------------------------------------------------------------------------

std::vector<cbn::Rectangle> create_screen_sprites(const Camera& camera, const uint64_t sprite_count)
{
	std::vector<cbn::Rectangle> sprites;
	sprites.reserve(sprite_count);

	// We want to fit the given sprite count perfectly in the resolution of the camera
	// To do this, we first find the amount of sprites vertically and horizontally
	// that we need to meet the sprite_count while matching the aspect ratio of the 
	// camera.
	const float aspect_ratio = camera.resolution().x / camera.resolution().y;
	const float vertical_sprites = std::sqrtf(sprite_count / aspect_ratio);
	const float horizontal_sprites = sprite_count / vertical_sprites;

	// Knowing the amount of sprites, we then determine the size that the sprites
	// would need to have in order to take up the entire resolution.
	const glm::vec2 sprite_size{camera.resolution().x / horizontal_sprites, camera.resolution().y / vertical_sprites};
	const glm::vec2 half_size = sprite_size * 0.5f;
	const auto half_res = camera.resolution() * 0.5f;
	for(float y = -half_res.y; y < half_res.y; y += sprite_size.y)
	{
		for(float x = -half_res.x; x < half_res.x; x += sprite_size.x)
		{
			const Transform pos{x, y};

			sprites.emplace_back(pos,3.0f*sprite_size);
		}
	}
	return sprites;
}

//-------------------------------------------------------------------------------------

void static_render_scene(URes<Window>& window, bool& runflag)
{
	// Set up the renderer & camera. By making the batches larger
	// we minimise the number of draw calls and the amount of sprites
	// which get rendered with the unrolled loop section of our render 
	// loop below.
	constexpr uint16_t batch_size = 16384 * 2;
	SpriteRenderer renderer(window->get_opengl_version(), {batch_size, 16});
	Camera camera(window->get_resolution());

	// Load the texture shader
	auto texture_program = load_program("TextureVertShader.glsl", "TextureFragShader.glsl");

	// Load textures
	std::array<Identifier, 3> texture_ids{
		Identifier{"star1"}, Identifier{"star2"}, Identifier{"star3"}
	};
	const auto texture_pack = load_textures(window, {
		{texture_ids[0], texture_ids[0].alias() + ".png"},
		{texture_ids[1], texture_ids[1].alias() + ".png"},
		{texture_ids[2], texture_ids[2].alias() + ".png"},
	});
	renderer.set_texture_pack(texture_pack);

	// Create 100k static meshes which will be rendered
	// Note that the camera is centred at (0,0)
	const auto sprites = create_screen_sprites(camera, 1000000);
	std::vector<StaticMesh<4>> meshes;
	meshes.reserve(sprites.size());
	for(const auto& sprite : sprites)
		meshes.push_back(sprite.mesh());

	// Remove Vsync as we want it to run as quick as possible
	window->set_vsync(false);

	AutoTimer timer;
	float frames = 0;
	auto subscription = timer.TimerEvent.subscribe([&]()
	{
		window->set_title("Carbon Sample | Textured Sprites: " + std::to_string(sprites.size()) + " | FPS: " + std::to_string(frames) + ";  Frame Time: " + std::to_string(1 / frames) + "ms");
		frames = 0;
	});
	timer.start(Time::Seconds(1));

	// Render all the sprites
	while(runflag)
	{
		window->clear();

		// Return back to the menu if escape is pressed
		if(glfwGetKey(window->TEMP_HANDLE(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
		{
			window->set_title("Carbon Sample");
			return;
		}

		uint64_t total_submitted = 0;
		while(total_submitted != meshes.size())
		{
			const auto sprites_left = meshes.size() - total_submitted;
			renderer.begin_batch(camera);

			if(sprites_left >= batch_size)
			{
				for(auto i = 0; i < batch_size; i++)
				{
					renderer.submit(meshes[total_submitted], texture_ids[i % texture_ids.size()]);
					total_submitted++;
				}
			}
			else
			{
				for(auto i = 0; i < sprites_left; i++)
				{
					renderer.submit(meshes[total_submitted], texture_ids[i % texture_ids.size()]);
					total_submitted++;
				}
			}
			renderer.end_batch();
			renderer.render(texture_program);
		}
		
		window->update();
		frames++;
	}
}

//-------------------------------------------------------------------------------------

void move_sprite(cbn::Rectangle& sprite, const glm::vec2& mouse_pos)
{
	const float distance = glm::distance2(sprite.centre(), mouse_pos);

	if(distance >= 10000)
		sprite.translate_towards(mouse_pos.x, mouse_pos.y, 4);
	else
		sprite.translate_by(rand() % 3500 - 1500, rand() % 3500 - 1500);
}

//-------------------------------------------------------------------------------------

void dynamic_render_scene(URes<Window>& window, bool& runflag)
{
	// Set up the renderer & camera. By making the batches larger
	// we minimise the number of draw calls and the amount of sprites
	// which get rendered with the unrolled loop section of our render 
	// loop below.
	constexpr uint16_t batch_size = 16384 * 2;
	SpriteRenderer renderer(window->get_opengl_version(), {batch_size, 16});
	Camera camera(window->get_resolution());

	// Load the texture shader
	auto texture_program = load_program("TextureVertShader.glsl", "TextureFragShader.glsl");

	// Load textures
	std::array<Identifier, 3> texture_ids{
		Identifier{"star1"}, Identifier{"star2"}, Identifier{"star3"}
	};
	const auto texture_pack = load_textures(window, {
		{texture_ids[0], texture_ids[0].alias() + ".png"},
		{texture_ids[1], texture_ids[1].alias() + ".png"},
		{texture_ids[2], texture_ids[2].alias() + ".png"},
	});
	renderer.set_texture_pack(texture_pack);

	// Create 100k static meshes which will be rendered
	// Note that the camera is centred at (0,0)
	auto sprites = create_screen_sprites(camera, 100000);

	// Remove Vsync as we want it to run as quick as possible
	window->set_vsync(false);

	AutoTimer timer;
	float frames = 0;
	auto subscription = timer.TimerEvent.subscribe([&]()
	{
		window->set_title("Carbon Sample | Textured Sprites: " + std::to_string(sprites.size()) + " | FPS: " + std::to_string(frames) + ";  Frame Time: " + std::to_string(1 / frames) + "ms");
		frames = 0;
	});
	timer.start(Time::Seconds(1));

	// Render all the sprites
	while(runflag)
	{
		window->clear();

		// Return back to the menu if escape is pressed
		if(glfwGetKey(window->TEMP_HANDLE(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
		{
			window->set_title("Carbon Sample");
			return;
		}

		// Get mouse position. Note that we need to offset it as screen coordinates
        // have (0,0) top left, while its in the middle for us. 
		double pos_x, pos_y;
		glfwGetCursorPos(window->TEMP_HANDLE(), &pos_x, &pos_y);
		const glm::vec2 mouse_pos{pos_x - (camera.resolution().x * 0.5f), (camera.resolution().y * 0.5f) - pos_y};

		bool space_pressed = glfwGetKey(window->TEMP_HANDLE(), GLFW_KEY_SPACE) == GLFW_PRESS;

		// Render and move the sprites
		uint64_t total_submitted = 0;
		while(total_submitted != sprites.size())
		{
			const auto sprites_left = sprites.size() - total_submitted;
			renderer.begin_batch(camera);

			if(sprites_left >= batch_size)
			{
				for(auto i = 0; i < batch_size; i++)
				{

					move_sprite(sprites[total_submitted], mouse_pos);

					renderer.submit(sprites[total_submitted].mesh(), texture_ids[i % texture_ids.size()]);
					total_submitted++;
				}
			}
			else
			{
				for(auto i = 0; i < sprites_left; i++)
				{
					move_sprite(sprites[total_submitted], mouse_pos);

					renderer.submit(sprites[total_submitted].mesh(), texture_ids[i % texture_ids.size()]);
					total_submitted++;
				}
			}
			renderer.end_batch();
			renderer.render(texture_program);
		}

		window->update();
		frames++;
	}
}

//-------------------------------------------------------------------------------------

//void bounds_test_scene(URes<Window>& window, bool& runflag)
//{
//	// Create the standard renderer and scene camera
//	SpriteRenderer renderer(window->get_opengl_version());
//
//	// Note that the camera is centred at (0,0). 
//	// In order to make it a bit easier to position the 
//	// colliders. We will move the camera up by half
//	// its resolution to put (0,0) on its bottom left. 
//	Camera camera(window->get_resolution());
//	camera.translate_by(camera.resolution() * 0.5f);
//
//	// Load shader & textures
//	auto tint_program = load_program("TintVertShader.glsl", "TintFragShader.glsl");
//
//	const Identifier circle_texture_id = "Circle";
//	const Identifier rectangle_texture_id = "Rectangle";
//	const Identifier triangle_1_texture_id = "Triangle1";
//	const Identifier triangle_2_texture_id = "Triangle2";
//
//	const auto texture_pack = load_textures(window, {
//		{circle_texture_id, "Circle.png"},
//		{rectangle_texture_id, "Square.png"},
//		{triangle_1_texture_id, "Triangle1.png"},
//		{triangle_2_texture_id, "Triangle2.png"}
//	});
//	renderer.set_texture_pack(texture_pack);
//
//	// Create tint colours
//	constexpr float alpha = 256 * (9.0f / 10.0f);
//	const glm::uvec4 red_colour = {Red.red, Red.green, Red.blue, alpha};
//	const glm::uvec4 blue_colour = {Blue.red, Blue.green, Blue.blue, alpha};
//	const glm::uvec4 cyan_colour = {Cyan.red, Cyan.green, Cyan.blue, alpha};
//	const glm::uvec4 white_colour = {White.red, White.green, White.blue, alpha};
//	const glm::uvec4 yellow_colour = {Yellow.red, Yellow.green, Yellow.blue, alpha};
//	const glm::uvec4 orange_colour = {Orange.red, Orange.green, Orange.blue, alpha};
//	const glm::uvec4 magenta_colour = {Magenta.red, Magenta.green, Magenta.blue, alpha};
//
//	// Create the colliders
//	cbn::Rectangle rect_1{Transform{800.0f, 600.0f}, {32, 32}};
//	cbn::Rectangle rect_2{Transform{600.0f, 800.0f, 25}, {128, 64}};
//	cbn::Circle circle_1{Transform{1200, 900}, 16.0f};
//	cbn::Circle circle_2{Transform{1200, 200}, 64.0f};
//
//	std::array<glm::vec2, 3> triangle_1_vertices = {
//		glm::vec2{0, 32},
//		glm::vec2{-16, 0},
//		glm::vec2{16, 0}
//	};
//
//	std::array<glm::vec2, 3> triangle_2_vertices = {
//		glm::vec2{0, 108},
//		glm::vec2{237, 0},
//		glm::vec2{420, 26}
//	};
//
//	// The triangles cannot be rendered by the sprite renderer directly so
//	// we use a object alligned bounding box which mimics the triangle 
//	// and render that instead.
//	static cbn::StaticMesh triangle_1{Transform{200, 300}, triangle_1_vertices};
//	static cbn::Rectangle triangle_1_obb = triangle_1.wrap_axis_alligned();
//
//	static cbn::StaticMesh triangle_2{Transform{800, 300}, triangle_2_vertices};
//	static cbn::Rectangle triangle_2_obb = triangle_2.wrap_axis_alligned();
//
//	// Using bounding boxes, create lines to act as rays, segments and rays
//	// TODO: implement a line renderer instead
//	 const cbn::Rectangle line_bb{Transform{300, 600, 275}, {2000,1}};
//	 const Line line{line_bb.mesh().vertices()[0], line_bb.mesh().vertices()[3]};
//	
//	 const cbn::Rectangle segment_bb{Transform{800,700, 32}, {100, 1}};
//	 const Segment segment{segment_bb.mesh().vertices()[0], segment_bb.mesh().vertices()[3]};
//
//	 const cbn::Rectangle ray_bb{Transform{1850, 1200, 50}, {2000,1}};
//	 const Ray ray{ray_bb.mesh().vertices()[0], glm::normalize(ray_bb.mesh().vertices()[3] - ray_bb.mesh().vertices()[0])};
//
//	// Store all colliders and their associated data in arrays to allow processing them with a loop
//	std::array<Collider*, 6> colliders = {
//		&rect_1, &rect_2, &circle_1, &circle_2, &triangle_1, &triangle_2 
//	};
//	std::array<Transformable<Translatable2D, Rotatable2D>*, 6> transformables = {
//	&rect_1, &rect_2, &circle_1, &circle_2, &triangle_1, &triangle_2 
//	};
//	std::array<glm::uvec4, 6> tint_colours = {
//	     white_colour, white_colour, white_colour, white_colour, white_colour, white_colour
//	};
//	std::array<bool, 6> held = {
//		 false, false, false, false, false, false
//	};
//
//	// Run the test & render the shapes, we want to enable vsync to limit it
//	window->set_vsync(true);
//	while(runflag)
//	{
//		window->clear();
//
//		// Return back to the menu if escape is pressed
//		if(glfwGetKey(window->TEMP_HANDLE(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
//		{
//			window->set_title("Carbon Sample");
//			return;
//		}
//
//		bool mouse_clicked = GLFW_PRESS == glfwGetMouseButton(window->TEMP_HANDLE(), GLFW_MOUSE_BUTTON_LEFT);
//		bool space_pressed = glfwGetKey(window->TEMP_HANDLE(), GLFW_KEY_SPACE) == GLFW_PRESS;
//
//		// Get mouse position. Note that we need to offset it as screen coordinates
//	    // have (0,0) top left, while for us its currently bottom left (as we moved the camera). 
//		double pos_x, pos_y;
//		glfwGetCursorPos(window->TEMP_HANDLE(), &pos_x, &pos_y);
//		const glm::vec2 mouse_pos{pos_x, camera.resolution().y - pos_y};
//
//		// handle the colliders
//		for(auto i = 0; i < colliders.size(); i++)
//		{
//			bool any_held = std::any_of(held.begin(), held.end(), [](auto v) { return v; });
//			auto& trans = transformables[i];
//			auto& colour = tint_colours[i];
//			auto& curr = *colliders[i];
//			auto& is_held = held[i];
//
//			bool overlaps = false;
//			bool encloses = false;
//			bool enclosed = false;
//
//			// Consider being held as also being moused over. Since app is not
//			// running at the same rate that the mouse is being polled, its possible
//			// for the mouse to run away from the collider causing it to no longer count
//			// as being moused over when it is being held. Thus causing its tint to
//			// flash between the held not held colours. 
//			bool moused_over = curr.contains(mouse_pos) || is_held;
//
//			// If no other collider is already held, the mouse is clicked and the 
//			// mouse is hovering over the collider. Then we want to pick it up. 
//			if(mouse_clicked)
//			{
//				if(!any_held && moused_over)
//				{
//					const auto world_origin_offset = mouse_pos - curr.centre();
//
//					// When we pick it up, we want to flag that were holding it and move its origin to where the mouse is. 
//					curr.specify_origin(world_origin_offset);
//					is_held = true;
//				}
//			}
//			else is_held = false;
//
//			// If collider is held, then we want to move it to where the mouse is
//		    // and rotate it if space is held. 
//			if(is_held)
//			{
//				trans->translate_to(mouse_pos);
//
//				if(space_pressed)
//					trans->rotate_by(5);
//			}
//
//
//			// Check for any overlap or enclosing between colliders
//			for(auto j = 0; j < colliders.size(); j++)
//			{
//				if(i == j) continue;
//
//				const auto& target = *colliders[j];
//
//				if(curr.overlaps(target))
//					overlaps = true;
//
//				if(curr.enclosed_by(target))
//					enclosed = true;
//
//				if(target.enclosed_by(curr))
//					encloses = true;
//			}
//
//			if(curr.intersected_by(line) || curr.intersected_by(ray) || curr.intersected_by(segment))
//				overlaps = true;
//
//			if(enclosed)
//				tint_colours[i] = yellow_colour;
//			else if(encloses)
//				tint_colours[i] = orange_colour;
//			else if(overlaps && moused_over)
//				tint_colours[i] = magenta_colour;
//			else if(overlaps)
//				tint_colours[i] = blue_colour;
//			else if(moused_over)
//				tint_colours[i] = red_colour;
//			else
//				tint_colours[i] = white_colour;
//		}
//
//		// Render the bounds
//		renderer.begin_batch(camera);
//
//		renderer.submit(rect_1.mesh().vertices, rectangle_texture_id, tint_colours[0]);
//		renderer.submit(rect_2.mesh().vertices, rectangle_texture_id, tint_colours[1]);
//
//		renderer.submit(circle_1.wrap_axis_alligned().mesh().vertices, circle_texture_id, tint_colours[2]);
//		renderer.submit(circle_2.wrap_axis_alligned().mesh().vertices, circle_texture_id, tint_colours[3]);
//
//		renderer.submit(triangle_1.wrap_object_alligned().mesh().vertices, triangle_1_texture_id, tint_colours[4]);
//		renderer.submit(triangle_2.wrap_object_alligned().mesh().vertices, triangle_2_texture_id, tint_colours[5]);
//
//		renderer.submit(line_bb.mesh().vertices, rectangle_texture_id, yellow_colour);
//		renderer.submit(segment_bb.mesh().vertices, rectangle_texture_id, orange_colour);
//		renderer.submit(ray_bb.mesh().vertices, rectangle_texture_id, cyan_colour);
//
//		renderer.end_batch();
//		renderer.render(tint_program);
//
//		window->update();
//	}
//}

//-------------------------------------------------------------------------------------
