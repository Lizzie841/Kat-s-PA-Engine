#pragma once

#include <string>
#include <string_view>

#include "glad.h"

#include "container_types.hpp"
#include "texture.hpp"
#include "fonts.hpp"

namespace ogl {
	namespace parameters {
		inline constexpr GLuint screen_width = 0;
		inline constexpr GLuint screen_height = 1;
		inline constexpr GLuint drawing_rectangle = 2;

		inline constexpr GLuint border_size = 6;
		inline constexpr GLuint inner_color = 7;
		inline constexpr GLuint subrect = 10;

		inline constexpr GLuint enabled = 4;
		inline constexpr GLuint disabled = 3;
		inline constexpr GLuint border_filter = 0;
		inline constexpr GLuint filter = 1;
		inline constexpr GLuint no_filter = 2;
		inline constexpr GLuint sub_sprite = 5;
		inline constexpr GLuint use_mask = 6;
		inline constexpr GLuint progress_bar = 7;
		inline constexpr GLuint frame_stretch = 8;
		inline constexpr GLuint piechart = 9;
		inline constexpr GLuint barchart = 10;
		inline constexpr GLuint linegraph = 11;
		inline constexpr GLuint tint = 12;
		inline constexpr GLuint interactable = 13;
		inline constexpr GLuint interactable_disabled = 14;
		inline constexpr GLuint subsprite_b = 15;
		inline constexpr GLuint alternate_tint = 16;
		inline constexpr GLuint linegraph_color = 17;
	} // namespace parameters
	enum class color_modification { none, disabled, interactable, interactable_disabled };
	struct color3f {
		float r = 0.0f;
		float g = 0.0f;
		float b = 0.0f;
	};

	struct image {
		uint8_t* data = nullptr;
		int32_t size_x = 0;
		int32_t size_y = 0;
		int32_t channels = 0;

		image() { }
		image(uint8_t* data, int32_t size_x, int32_t size_y, int32_t channels) {
			this->data = data;
			this->size_x = size_x;
			this->size_y = size_y;
			this->channels = channels;
		}
		image(image const& other) = delete;

		image(image&& other) noexcept {
			data = other.data;
			size_x = other.size_x;
			size_y = other.size_y;
			channels = other.channels;
			other.data = nullptr;
		}

		image& operator=(image&& other) noexcept {
			data = other.data;
			size_x = other.size_x;
			size_y = other.size_y;
			channels = other.channels;
			other.data = nullptr;
			return *this;
		}
		image& operator=(image const& other) = delete;

		~image() {
			if(data)
			free(data);
		}
	};

	void debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei, GLchar const* message, void const*);

	struct data {
		tagged_vector<texture, dcon::texture_id> asset_textures;

		void* context = nullptr;
		bool legacy_mode = false;
		GLuint ui_shader_program = 0;

		GLuint ui_shader_d_rect_uniform = 0;
		GLuint ui_shader_subroutines_index_uniform = 0;
		GLuint ui_shader_inner_color_uniform = 0;
		GLuint ui_shader_subrect_uniform = 0;
		GLuint ui_shader_border_size_uniform = 0;
		GLuint ui_shader_texture_sampler_uniform = 0;
		GLuint ui_shader_secondary_texture_sampler_uniform = 0;
		GLuint ui_shader_screen_width_uniform = 0;
		GLuint ui_shader_screen_height_uniform = 0;
		GLuint ui_shader_gamma_uniform = 0;

		GLuint global_square_vao = 0;
		GLuint global_square_buffer = 0;

		GLuint money_icon_tex = 0;
		GLuint cross_icon_tex = 0;
		GLuint color_blind_cross_icon_tex = 0;
		GLuint checkmark_icon_tex = 0;
		GLuint navy_icon_tex = 0;
		GLuint army_icon_tex = 0;

		GLuint msaa_framebuffer = 0;
		GLuint msaa_texcolorbuffer = 0;
		GLuint msaa_rbo = 0;
		GLuint msaa_interbuffer = 0;
		GLuint msaa_texture = 0;
		GLuint msaa_vao = 0;
		GLuint msaa_vbo = 0;
		GLuint msaa_shader_program = 0;
		GLuint msaa_uniform_screen_size = 0;
		GLuint msaa_uniform_gaussian_blur = 0;
		bool msaa_enabled = false;
	};

	void notify_user_of_fatal_opengl_error(std::string message);

	void create_opengl_context(sys::state& state); // you shouldn't call this directly; only initialize_opengl should call it
	void initialize_opengl(sys::state& state);
	void shutdown_opengl(sys::state& state);

	bool display_tag_is_valid(sys::state& state, char tag[3]);

	GLint compile_shader(std::string_view source, GLenum type);
	GLuint create_program(std::string_view vertex_shader, std::string_view fragment_shader, uint32_t flags);
	void load_shaders(sys::state& state);
	void load_global_squares(sys::state& state);

	class lines {
	private:
		float* buffer = nullptr;
		GLuint buffer_handle = 0;
		GLuint array_handle = 0;
		bool pending_data_update = true;
	public:
		uint32_t count = 0;

		lines(uint32_t c) : count(c) {
			buffer = new float[count * 4];
			set_default_y();
		}
		lines(lines&& o) noexcept : buffer(o.buffer), buffer_handle(o.buffer_handle), pending_data_update(o.pending_data_update), count(o.count) {
			o.buffer = nullptr;
		}
		lines& operator=(lines&& o) noexcept {
			count = o.count;
			buffer = o.buffer;
			buffer_handle = o.buffer_handle;
			pending_data_update = o.pending_data_update;

			o.buffer = nullptr;
			return *this;
		}
		~lines() {
			delete[] buffer;
		}
		void set_default_y();
		void set_y(float* v);
		void bind_buffer();
	};

	void render_simple_rect(sys::state const& state, float x, float y, float width, float height, ui::rotation r, bool flipped);
	void render_textured_rect(sys::state const& state, color_modification enabled, float x, float y, float width, float height,
		GLuint texture_handle, ui::rotation r, bool flipped, bool rtl);
	void render_textured_rect_direct(sys::state const& state, float x, float y, float width, float height, uint32_t handle);
	void render_linegraph(sys::state const& state, color_modification enabled, float x, float y, float width, float height, lines& l);
	void render_linegraph(sys::state const& state, color_modification enabled, float x, float y, float width, float height, float r, float g, float b, lines& l);
	void render_barchart(sys::state const& state, color_modification enabled, float x, float y, float width, float height,
		data_texture& t, ui::rotation r, bool flipped, bool rtl);
	void render_piechart(sys::state const& state, color_modification enabled, float x, float y, float size, data_texture& t);
	void render_bordered_rect(sys::state const& state, color_modification enabled, float border_size, float x, float y, float width,
		float height, GLuint texture_handle, ui::rotation r, bool flipped, bool rtl);
	void render_masked_rect(sys::state const& state, color_modification enabled, float x, float y, float width, float height,
		GLuint texture_handle, GLuint mask_texture_handle, ui::rotation r, bool flipped, bool rtl);
	void render_progress_bar(sys::state const& state, color_modification enabled, float progress, float x, float y, float width,
		float height, GLuint left_texture_handle, GLuint right_texture_handle, ui::rotation r, bool flipped, bool rtl);
	void render_tinted_textured_rect(sys::state const& state, float x, float y, float width, float height, float r, float g, float b,
		GLuint texture_handle, ui::rotation rot, bool flipped, bool rtl);
	void render_subsprite(sys::state const& state, color_modification enabled, int frame, int total_frames, float x, float y,
		float width, float height, GLuint texture_handle, ui::rotation r, bool flipped, bool rtl);
	void render_tinted_subsprite(sys::state const& state, int frame, int total_frames, float x, float y,
		float width, float height, float r, float g, float b, GLuint texture_handle, ui::rotation rot, bool flipped, bool rtl);
	void render_new_text(sys::state const& state, text::stored_glyphs const& txt, color_modification enabled, float x,
		float y, float size, color3f const& c, text::font& f);
	void render_text(sys::state& state, text::stored_glyphs const& txt, color_modification enabled, float x, float y,
		color3f const& c, uint16_t font_id);
	void render_text_icon(sys::state& state, text::embedded_icon ico, float x, float baseline_y, float font_size, text::font& f, ogl::color_modification = ogl::color_modification::none);
	void render_text_flag(sys::state& state, text::embedded_flag ico, float x, float baseline_y, float font_size, text::font& f, ogl::color_modification = ogl::color_modification::none);

	bool msaa_enabled(sys::state const& state);
	void initialize_msaa(sys::state& state, int32_t x, int32_t y);
	void deinitialize_msaa(sys::state& state);

	image load_stb_image(simple_fs::file& file);
	GLuint make_gl_texture(uint8_t* data, uint32_t size_x, uint32_t size_y, uint32_t channels);
	GLuint make_gl_texture(simple_fs::directory const& dir, native_string_view file_name);
	void set_gltex_parameters(GLuint texture_handle, GLuint texture_type, GLuint filter, GLuint wrap);
	GLuint load_texture_array_from_file(simple_fs::file& file, int32_t tiles_x, int32_t tiles_y);
} // namespace ogl
