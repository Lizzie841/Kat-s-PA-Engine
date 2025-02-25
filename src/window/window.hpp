#pragma once

// #include "system_state.hpp"
#include "constants.hpp"

#ifdef _WIN32

typedef struct HWND__* HWND;
typedef struct HDC__* HDC;

namespace window {
	class window_data_impl {
		public:
		HWND hwnd = nullptr;
		HDC opengl_window_dc = nullptr;
		HCURSOR cursors[8] = { HCURSOR(NULL) };

		int32_t creation_x_size = 600;
		int32_t creation_y_size = 400;

		bool in_fullscreen = false;
		bool left_mouse_down = false;
	};
} // namespace window
#else
struct GLFWwindow;

namespace window {
	class window_data_impl {
		public:
		GLFWwindow* window = nullptr;

		int32_t creation_x_size = 600;
		int32_t creation_y_size = 400;

		bool in_fullscreen = false;
		bool left_mouse_down = false;
	};
} // namespace window
#endif

namespace sys {
	struct state;
}

namespace window {
	enum class window_state : uint8_t { normal, maximized, minimized };
	struct creation_parameters {
		int32_t size_x = 1024;
		int32_t size_y = 768;
		window_state initial_state = window_state::maximized;
		bool borderless_fullscreen = false;
	};

	void create_window(sys::state& game_state, creation_parameters const& params);
	/* This function will run the main window loop and will not return until the window is closed */
	void initialize_window(sys::state& game_state);
	void close_window(sys::state& game_state); // close the main window
	void set_borderless_full_screen(sys::state& game_state, bool fullscreen);
	bool is_in_fullscreen(sys::state const& game_state);
	bool is_key_depressed(sys::state const& game_state, sys::virtual_key key); // why not cheer it up then?

	enum class cursor_type : uint8_t {
		normal,
		busy,
		drag_select,
		hostile_move,
		friendly_move,
		no_move
	};
	void change_cursor(sys::state const& state, cursor_type type);

	void get_window_size(sys::state const& game_state, int& width, int& height);

	void emit_error_message(std::string const& content, bool fatal); // also terminates the program if fatal
} // namespace window
