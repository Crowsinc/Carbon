#define CBN_DISABLE_ASSERTS

#include <Graphics/Window.hpp>
#include <Carbon.hpp>
#include <Diagnostics/Stopwatch.hpp>

#include <glm/gtx/norm.hpp>

#include <Maths/Maths.hpp>

#include <algorithm>
#include <iostream>
#include <thread>

#include <Maths/Models/BoundingCircle.hpp>
#include <Maths/Models/BoundingBox.hpp>
#include <Maths/Models/BoundingTriangle.hpp>

#include <Graphics/SpriteRenderer.hpp>
#include <Graphics/Resources/Shader.hpp>
#include <Graphics/Resources/ShaderProgram.hpp>
#include <Graphics/OpenGL/VertexArrayObject.hpp>

#include <Data/Identity/Identifier.hpp>

#include <Graphics/Resources/Texture.hpp>
#include <Graphics/Resources/TextureAtlas.hpp>

#include <Graphics/OpenGL/OpenGL.hpp>

bool runflag = true;

using namespace cbn;

#include <gl/GL.h>

enum class SampleStates
{
	CHOOSE_STATE,
	STATIC_RENDER_STATE,
	DYNAMIC_RENDER_STATE,
	BOUNDS_TEST_STATE
};

void choose_state_scene(SampleStates& state, URes<Window>& window, SpriteRenderer& renderer, Camera& camera);

void static_render_scene(SampleStates& state, URes<Window>& window, SpriteRenderer& renderer, Camera& camera);

void dynamic_render_scene(SampleStates& state, URes<Window>& window, SpriteRenderer& renderer, Camera& camera);

void bounds_test_scene(SampleStates& state, URes<Window>& window, SpriteRenderer& renderer, Camera& camera);


int main()
{
	// Create render window
	cbn::Window::Properties props;
	props.title = "Carbon Sample";
	props.resolution = {1920,1080};
	props.display_mode = cbn::Window::DisplayMode::WINDOWED;
	props.opengl_version = {4,6,0};

#ifdef _DEBUG
	props.opengl_debug = false;
	props.vsync = true;

#else
	props.opengl_debug = false;
	props.vsync = false;
#endif

	auto window = cbn::Window::Create(props);
	if(window)
		std::cout << "Window Created!" << std::endl;
	else
		std::cout << "Window Failed To Create!" << std::endl;


	// Subscribe to the error handling event
	window->ErrorEvent.subscribe([](std::string msg, GLenum source, GLenum severity)
	{
			std::cout << msg << std::endl;
	});

	// Subscribe to the close requested event in order to stop the sample when the window is closed.
	window->CloseRequestEvent.subscribe([&]()
	{
		runflag = false;
	});

	//TODO: fix having to do this
	glViewport(0, 0, 1920, 1080);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Print relevant information
	std::string gpu = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
	std::cout << "GPU: " << gpu << std::endl;

	int i = 0;
	glGetIntegerv(GL_MAX_TEXTURE_BUFFER_SIZE, &i);
	std::cout << "Max TBO Texels: " << std::to_string(i) << std::endl;

	// Create sprite renderer and texture pack
	cbn::SpriteRendererProperties prop;
	prop.buffer_allocation_bias = 32;
	prop.sprites_per_batch = 4096;

	cbn::Camera scene_camera(1920, 1080);
	cbn::TexturePack texture_pack(window->get_opengl_version());
	cbn::SpriteRenderer renderer(window->get_opengl_version(),{}, prop);


	cbn::SRes<cbn::Texture> rock_texture = cbn::Texture::Open("res/rock.png");
	cbn::SRes<cbn::Texture> ground_texture = cbn::Texture::Open("res/ground.png");
	cbn::SRes<cbn::Texture> bt_button_texture = cbn::Texture::Open("res/BTButton.png");
	cbn::SRes<cbn::Texture> drt_button_texture = cbn::Texture::Open("res/DRTButton.png");
	cbn::SRes<cbn::Texture> srt_button_texture = cbn::Texture::Open("res/SRTButton.png");
	cbn::SRes<cbn::Texture> sqr_texture = cbn::Texture::Open("res/square.png");
	cbn::SRes<cbn::Texture> circle_texture = cbn::Texture::Open("res/circle.png");
	cbn::SRes<cbn::Texture> tri_texture = cbn::Texture::Open("res/tri.png");
	cbn::SRes<cbn::Texture> tri2_texture = cbn::Texture::Open("res/tri2.png");

	texture_pack = {
		TexturePackEntry{"rock", rock_texture},
		TexturePackEntry{"ground", ground_texture},
        TexturePackEntry{"bt_button", bt_button_texture},
		TexturePackEntry{"drt_button", drt_button_texture},
		TexturePackEntry{"srt_button", srt_button_texture},
		TexturePackEntry{"circle", circle_texture},
		TexturePackEntry{"square", sqr_texture},
		TexturePackEntry{"tri", tri_texture},
		TexturePackEntry{"tri2", tri2_texture}
	};

	renderer.set_texture_pack(texture_pack);

	// Prepare the main loop
	uint64_t frames = 0;
	cbn::Stopwatch watch;

	SampleStates state = SampleStates::CHOOSE_STATE;
	window->set_vsync(true);

	watch.start();
	while(runflag)
	{
		glClear(GL_COLOR_BUFFER_BIT);

		switch(state)
		{
			case SampleStates::CHOOSE_STATE:
				choose_state_scene(state, window, renderer, scene_camera);
				break;
			case SampleStates::STATIC_RENDER_STATE:
				static_render_scene(state, window, renderer, scene_camera);
				break;
			case SampleStates::DYNAMIC_RENDER_STATE:
				dynamic_render_scene(state, window, renderer, scene_camera);
				break;
			case SampleStates::BOUNDS_TEST_STATE:
				bounds_test_scene(state, window, renderer, scene_camera);
				break;
		}
		
		if(glfwGetKey(window->TEMP_HANDLE(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
		{
			state = SampleStates::CHOOSE_STATE;
			window->set_vsync(true);
		}

		frames++;
		
		// Update the window
		window->update();
		if(watch.get_elapsed_time(cbn::Seconds) > 1.0)
		{
			window->set_title("Carbon Sample  |  Frames Per Second: " + std::to_string(frames) + " fps  |  Frame Time: " + std::to_string(1 / (float)frames) + "ms");
			watch.restart();
			frames = 0;
		}
	}
	return 0;
}


SRes<ShaderProgram> load_tint_shader()
{
	std::cout << "Loading tint shader\n";

	auto [vertex_sh, error_log1] = cbn::Shader::Open("res/ButtonVShader.glsl", cbn::Shader::Stage::VERTEX);
	if(!vertex_sh)
		std::cout << error_log1 << std::endl;

	auto [frag_sh, error_log2] = cbn::Shader::Open("res/ButtonFShader.glsl", cbn::Shader::Stage::FRAGMENT);
	if(!frag_sh)
		std::cout << error_log2 << std::endl;

	auto [program, error_log3] = cbn::ShaderProgram::Create(vertex_sh, nullptr, frag_sh);
	if(!program)
		std::cout << error_log3 << std::endl;

	//TODO: find a better way to do this
	program->bind();
	if(program->has_uniform({"samplers[0]"}))
		for(int i = 0; i < 15; i++)
			program->set_uniform("samplers[" + String{std::to_string(i)} + "]", i + 1);

	return program;
}

void choose_state_scene(SampleStates& state, URes<Window>& window, SpriteRenderer& renderer, Camera& camera)
{
	static const float button_x = window->get_resolution().x / 2.0f;
	static const float mid_y = window->get_resolution().y / 2.0f;

	static const BoundingBox BTButtonBB{Transform{button_x,mid_y + 256}, {256, 128}};
	static const Identifier BTButtonTextureID = "bt_button";
	static const BoundingBox SRTButtonBB{Transform{button_x,mid_y}, {256, 128}};
	static const Identifier SRTButtonTextureID = "srt_button";
	static const BoundingBox DRTButtonBB{Transform{button_x,mid_y - 256}, {256, 128}};
	static const Identifier DRTButtonTextureID = "drt_button";
	static const glm::uvec4 red_ud = {Red.red, Red.green, Red.blue, Red.alpha};
	static const glm::uvec4 white_ud = {White.red, White.green, White.blue, White.alpha};
	static SRes<ShaderProgram> program = load_tint_shader();
	
	// Determine whether the mouse was clicked
	bool mouse_clicked = GLFW_PRESS == glfwGetMouseButton(window->TEMP_HANDLE(), GLFW_MOUSE_BUTTON_LEFT);
	
	// Get mouse position
	double pos_x, pos_y;
	glfwGetCursorPos(window->TEMP_HANDLE(), &pos_x, &pos_y);
	const glm::vec2 mouse_pos = {pos_x, window->get_resolution().y - pos_y};

	renderer.begin_batch(camera);
	renderer.submit(BTButtonBB, BTButtonTextureID, BTButtonBB.contains(mouse_pos) ? red_ud : white_ud);
	renderer.submit(SRTButtonBB, SRTButtonTextureID, SRTButtonBB.contains(mouse_pos) ? red_ud : white_ud);
	renderer.submit(DRTButtonBB, DRTButtonTextureID, DRTButtonBB.contains(mouse_pos) ? red_ud : white_ud);
	renderer.end_batch();

	renderer.render(program);

	// If the mouse is clicked, check if its on a bounding box and dispatch the correct event
	if(mouse_clicked)
	{
		if(BTButtonBB.contains(mouse_pos))
			state = SampleStates::BOUNDS_TEST_STATE;

		if(SRTButtonBB.contains(mouse_pos))
		{
			state = SampleStates::STATIC_RENDER_STATE;
			window->set_vsync(false);
		}

		if(DRTButtonBB.contains(mouse_pos))
		{
			state = SampleStates::DYNAMIC_RENDER_STATE;
			window->set_vsync(false);
		}
	}
}

/*
	TODO:
		- Fix circle origin issue (DONE)
			^^ The issue is just that the sample wasn't sending the new origin as a local space offset, instead as a transformed space offset. 
			^^ fixed by adding the direction vector. 
		- Triangle - centroid & centre ?
		- Determine how culling will work, and whether sending the bounding box is really a good solution or not.
			^ BoundingCircle for camera, BoundingBox for sprite. 
		- Implement transform / mesh caching (Use transformables?, check whether the dynamic render boosts are worth it vs. the static losses
			- Consider that scaling is now not a thing, yet the Transform solution can still allow trying to put scaling
		- Implement culling and test
		- Remake camera. 
		- Linear maths + Optimise. 

	TODO: allow up to 31 textures in texture pack, it should be upto the user to ensure they use the correct amount. 
		- also make the static maps just be arrays
*/

void bounds_test_scene(SampleStates& state, URes<Window>& window, SpriteRenderer& renderer, Camera& camera)
{
	static SRes<ShaderProgram> program = load_tint_shader();

	static BoundingBox Rect{{800.0f, 600.0f}, {32, 32}};
	static Identifier RectTextureID = "square";
	static bool RectHeld = false;

	static BoundingBox Rect2{{600.0f, 800.0f}, {128, 64}};
	static bool RectHeld2 = false;

	static BoundingCircle Circle{{1200, 900}, 16.0f};
	static Identifier CircleTextureID = "circle";
	static bool CircleHeld = false;

	static BoundingCircle Circle2{{1200, 200}, 64.0f};
	static bool CircleHeld2 = false;

	static std::array<glm::vec2, 3> Tri1Verts = {
		glm::vec2{0, 32}, 
		glm::vec2{-16, 0}, 
		glm::vec2{16, 0}
	};
	static BoundingTriangle Tri{{200, 300}, Tri1Verts};
	static BoundingBox TriBB = Tri.wrap_axis_alligned();
	static Identifier TriTextureID = "tri";
	static bool TriHeld = false;

	static std::array<glm::vec2, 3> Tri2Verts = {
        glm::vec2{0, 108}, 
		glm::vec2{237, 0}, 
		glm::vec2{420, 26}
	};
	static BoundingTriangle Tri2{{800, 300}, Tri2Verts};
	static BoundingBox TriBB2 = Tri2.wrap_axis_alligned();
	static Identifier Tri2TextureID = "tri2";
	static bool TriHeld2 = false;


	static const glm::uvec4 red_ud = {Red.red, Red.green, Red.blue, Red.alpha};
	static const glm::uvec4 blue_ud = {Blue.red, Blue.green, Blue.blue, Blue.alpha};
	static const glm::uvec4 white_ud = {White.red, White.green, White.blue, White.alpha};
	static const glm::uvec4 magenta_ud = {Magenta.red, Magenta.green, Magenta.blue, Magenta.alpha};

	static const glm::uvec4 yellow_ud = {Yellow.red, Yellow.green, Yellow.blue, Yellow.alpha};
	static const glm::uvec4 orange_ud = {Orange.red, Orange.green, Orange.blue, Orange.alpha};
	static const glm::uvec4 cyan_ud = {Cyan.red, Cyan.green, Cyan.blue, Cyan.alpha};
	static const glm::uvec4 cyan_trans_ud = {Cyan.red, Cyan.green, Cyan.blue, Cyan.alpha / 4};


	// Determine whether the mouse was clicked
	bool mouse_clicked = GLFW_PRESS == glfwGetMouseButton(window->TEMP_HANDLE(), GLFW_MOUSE_BUTTON_LEFT);

	// Get mouse position
	double pos_x, pos_y;
	glfwGetCursorPos(window->TEMP_HANDLE(), &pos_x, &pos_y);
	const glm::vec2 mouse_pos = {pos_x, window->get_resolution().y - pos_y};
	//std::cout << "x: " << mouse_pos.x << " \ty: " << mouse_pos.y << "\n";

	bool space_pressed = glfwGetKey(window->TEMP_HANDLE(), GLFW_KEY_SPACE) == GLFW_PRESS;

	bool any_held = RectHeld || RectHeld2 || CircleHeld || CircleHeld2 || TriHeld || TriHeld2;

	if(mouse_clicked)
	{
		// 1st Rect
		if(Rect.contains(mouse_pos) && !any_held)
		{
			Rect.specify_origin(mouse_pos - Rect.centre());
			RectHeld = true;
		}
		if(RectHeld)
		{
			Rect.translate_to(mouse_pos);
			if(space_pressed)
				Rect.rotate_by(5);
		}

		// 2nd Rect
		if(Rect2.contains(mouse_pos) && !any_held)
		{
			Rect2.specify_origin(mouse_pos - Rect2.centre());
			RectHeld2 = true;
		}
		if(RectHeld2)
		{
			Rect2.translate_to(mouse_pos);
			if(space_pressed)
				Rect2.rotate_by(5);
		}

		// 1st Circle
		if(Circle.contains(mouse_pos) && !any_held)
		{
			Circle.specify_origin(mouse_pos - Circle.centre());
			CircleHeld = true;
		}
		if(CircleHeld)
		{
			Circle.translate_to(mouse_pos);
			if(space_pressed)
				Circle.rotate_by(5);
		}

		// 2nd Circle
		if(Circle2.contains(mouse_pos) && !any_held)
		{
			Circle2.specify_origin(mouse_pos - Circle2.centre());
			CircleHeld2 = true;
		}
		if(CircleHeld2)
		{
			Circle2.translate_to(mouse_pos);
			if(space_pressed)
				Circle2.rotate_by(5);
		}

		if(Tri.contains(mouse_pos) && !any_held)
		{
			const auto tri_offset = mouse_pos - Tri.centre();
			const auto centre_offset = Tri.centre() - TriBB.centre();
			Tri.specify_origin(tri_offset);
			TriBB.specify_origin(tri_offset + centre_offset);
			TriHeld = true;
		}
		if(TriHeld)
		{
			Tri.translate_to(mouse_pos);
			TriBB.translate_to(mouse_pos);

			if(space_pressed)
			{
				Tri.rotate_by(5);
				TriBB.rotate_by(5);
			}
		}


		if(Tri2.contains(mouse_pos) && !any_held)
		{
			const auto tri_offset = mouse_pos - Tri2.centre();
			const auto centre_offset = Tri2.centre() - TriBB2.centre();
			Tri2.specify_origin(tri_offset);
			TriBB2.specify_origin(tri_offset + centre_offset);
			TriHeld2 = true;
		}
		if(TriHeld2)
		{
			Tri2.translate_to(mouse_pos);
			TriBB2.translate_to(mouse_pos);

			if(space_pressed)
			{
				Tri2.rotate_by(5);
				TriBB2.rotate_by(5);
			}
		}
	}
	else
	{
		TriHeld = false;
		TriHeld2 = false;
		RectHeld = false;
		RectHeld2 = false;
		CircleHeld = false;
		CircleHeld2 = false;
	}
	

	BoundingBox line{{300, 600, 275}, {2000,1}};
	Line l{line.mesh().vertex_1, line.mesh().vertex_4};

	BoundingBox segment{{800,700, 32}, {100, 1}};
	Segment s{segment.mesh().vertex_1, segment.mesh().vertex_4};

	BoundingBox ray{Transform{1850, 1200, 50}, {2000,1}};
	Ray r{ray.mesh().vertex_1, glm::normalize(ray.mesh().vertex_4 - ray.mesh().vertex_1)};

	std::array<Collider*, 6> colliders = {
		&Rect, &Rect2, &Circle, &Circle2, &Tri, &Tri2
	};

	std::array<glm::uvec4, 6> tint_colours = {
		white_ud, white_ud, white_ud, white_ud, white_ud, white_ud
	};

	for(auto i = 0; i < colliders.size(); i++)
	{
		bool overlaps = false;
		bool encloses = false;
		bool enclosed = false;
		
		const auto& curr = *colliders[i];
		for(auto j = 0; j < colliders.size(); j++)
		{
			if(i == j) continue;

			const auto& target = *colliders[j];

			if(curr.overlaps(target))
				overlaps = true;

			if(curr.encloses(target))
				encloses = true;

			if(target.encloses(curr))
				enclosed = true;
		}

		if(curr.intersected_by(l) || curr.intersected_by(r) || curr.intersected_by(s))
			overlaps = true;

		if(enclosed)
			tint_colours[i] = yellow_ud;
		else if(encloses)
			tint_colours[i] = orange_ud;
		else if(overlaps && curr.contains(mouse_pos))
			tint_colours[i] = magenta_ud;
		else if(overlaps)
			tint_colours[i] = blue_ud;
		else if(curr.contains(mouse_pos))
			tint_colours[i] = red_ud;
		else
			tint_colours[i] = white_ud;
	}

	renderer.begin_batch(camera);

	renderer.submit(Rect, RectTextureID, tint_colours[0]);
	renderer.submit(Rect2, RectTextureID, tint_colours[1]);

	renderer.submit(Circle.wrap_axis_alligned(), CircleTextureID, tint_colours[2]);
	renderer.submit(Circle2.wrap_axis_alligned(), CircleTextureID, tint_colours[3]);

	renderer.submit(TriBB, TriTextureID, tint_colours[4]);
	
	renderer.submit(Tri2.wrap_axis_alligned(), RectTextureID, cyan_trans_ud);
	renderer.submit(TriBB2, Tri2TextureID, tint_colours[5]);


	renderer.submit(line, RectTextureID, yellow_ud);
	renderer.submit(segment, RectTextureID, orange_ud);
	renderer.submit(ray, RectTextureID, cyan_ud);

	renderer.end_batch();
	renderer.render(program);
}


SRes<ShaderProgram> load_texture_shader()
{
	std::cout << "Creating texture shader\n";

	auto [vertex_sh, error_log1] = cbn::Shader::Open("res/TextureVShader.glsl", cbn::Shader::Stage::VERTEX);
	if(!vertex_sh)
		std::cout << error_log1 << std::endl;

	auto [frag_sh, error_log2] = cbn::Shader::Open("res/TextureFShader.glsl", cbn::Shader::Stage::FRAGMENT);
	if(!frag_sh)
		std::cout << error_log2 << std::endl;

	auto [program, error_log3] = cbn::ShaderProgram::Create(vertex_sh, nullptr, frag_sh);
	if(!program)
		std::cout << error_log3 << std::endl;

	//TODO: find a better way to do this
	program->bind();
	if(program->has_uniform({"samplers[0]"}))
		for(int i = 0; i < 15; i++)
			program->set_uniform("samplers[" + String{std::to_string(i)} + "]", i + 1);

	return program;
}

std::vector<BoundingBox> create_static_sprites()
{
	constexpr int SPRITE_COUNT = 100000;

	constexpr float padding = 1.056f;
	constexpr glm::vec2 size = {3.5f, 3.5f};

	std::vector<BoundingBox> sprites;
	sprites.reserve(SPRITE_COUNT);

	for(float x = size.x / 2.0f; x < 1920; x += padding + size.x)
	{
		for(float y = size.y / 2.0f; y < 1080; y += padding + size.y)
		{
			sprites.emplace_back(Transform{x, y}, size);


			if(sprites.size() == SPRITE_COUNT)
				break;
		}
		if(sprites.size() == SPRITE_COUNT)
			break;
	}
	return sprites;
}

void static_render_scene(SampleStates& state, URes<Window>& window, SpriteRenderer& renderer, Camera& camera)
{
	static BoundingCircle cam(glm::distance({0,0}, camera.get_resolution()));

	static SRes<ShaderProgram> program = load_texture_shader();
	static std::vector<BoundingBox> sprites = create_static_sprites();
	static Identifier textures[2] = {"rock", "ground"};
	
	uint64_t total_submitted = 0;
	while(total_submitted != sprites.size())
	{
		const int sprites_left = sprites.size() - total_submitted;
		renderer.begin_batch(camera);
		if(sprites_left >= 4096)
		{
			for(auto i = 0; i < 4096; i++)
			{
				const auto& mesh = sprites[total_submitted].mesh();

			//	if(cam.overlaps(sprites[total_submitted]))
					renderer.submit(mesh, textures[i % 2]);
				total_submitted++;
			}
		}
		else
		{
			for(auto i = 0; i < sprites_left; i++)
			{
				const auto& mesh = sprites[total_submitted].mesh();

			//	if(cam.overlaps(sprites[total_submitted]))
					renderer.submit(mesh, textures[i % 2]);
				total_submitted++;
			}
		}
		renderer.end_batch();
		renderer.render(program);
	}

}

void handle_dynamic_sprite(BoundingBox& sprite, const glm::vec2& mouse_pos)
{
	const float distance = glm::distance2(sprite.origin(), mouse_pos);

	if(distance >= 10000)
	{
		sprite.translate_towards(mouse_pos.x, mouse_pos.y, 4);
	}
	else
	{
		sprite.translate_to(rand() % 3500 - 1500, rand() % 3500 - 1500);
	}
}

void dynamic_render_scene(SampleStates& state, URes<Window>& window, SpriteRenderer& renderer, Camera& camera)
{
	static BoundingCircle cam(glm::distance({0,0}, camera.get_resolution()));

	static SRes<ShaderProgram> program = load_texture_shader();
	static Identifier textures[2] = {"rock", "ground"};
	static auto sprites = create_static_sprites();

	// Get mouse position
	double pos_x, pos_y;
	glfwGetCursorPos(window->TEMP_HANDLE(), &pos_x, &pos_y);
	const glm::vec2 mouse_pos = {pos_x, window->get_resolution().y - pos_y};

	uint64_t total_submitted = 0;
	while(total_submitted != sprites.size())
	{
		const int sprites_left = sprites.size() - total_submitted;
		renderer.begin_batch(camera);

		if(sprites_left >= 4096)
		{
			for(auto i = 0; i < 4096; i++)
			{
				const auto& mesh = sprites[total_submitted].mesh();

				handle_dynamic_sprite(sprites[total_submitted], mouse_pos);
				if(cam.overlaps(sprites[total_submitted]))
					renderer.submit(mesh, textures[i % 2]);
				total_submitted++;
			}
		}
		else
		{
			for(auto i = 0; i < sprites_left; i++)
			{
				const auto& mesh = sprites[total_submitted].mesh();

				handle_dynamic_sprite(sprites[total_submitted], mouse_pos);
				if(cam.overlaps(sprites[total_submitted]))
					renderer.submit(mesh, textures[i % 2]);
				total_submitted++;
			}
		}
		renderer.end_batch();
		renderer.render(program);
	}

}

/*
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

*/