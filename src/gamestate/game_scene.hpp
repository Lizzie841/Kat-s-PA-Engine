#pragma once
#include <functional>

namespace game_scene {

	enum class scene_id : uint8_t {
		pick_nation,
		in_game_basic,
		in_game_state_selector,
		end_screen,
		count
	};

	enum class borders_granularity : uint8_t {
		none, province, state, nation
	};

	void on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mod);
	void on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mod);
	void on_lbutton_up(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mod);

	void switch_scene(sys::state& state, scene_id ui_scene);

	void selected_units_control(sys::state& state, dcon::nation_id nation, dcon::province_id target, sys::key_modifiers mod);
	void open_diplomacy(sys::state& state, dcon::nation_id nation, dcon::province_id target, sys::key_modifiers mod);

	void select_player_nation_from_selected_province(sys::state& state);
	void select_wargoal_state_from_selected_province(sys::state& state);

	void open_province_window(sys::state& state, dcon::province_id p);

	void select_units(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mod);
	void deselect_units(sys::state& state);

	void start_dragging(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mod);

	void do_nothing_province_target(sys::state& state, dcon::nation_id nation, dcon::province_id target, sys::key_modifiers mod);
	void do_nothing_screen(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mod);
	void do_nothing(sys::state& state);

	sys::virtual_key replace_keycodes_map_movement(sys::state& state, sys::virtual_key keycode, sys::key_modifiers mod);
	sys::virtual_key replace_keycodes_identity(sys::state& state, sys::virtual_key keycode, sys::key_modifiers mod);

	void nation_picker_hotkeys(sys::state& state, sys::virtual_key keycode, sys::key_modifiers mod);
	void do_nothing_hotkeys(sys::state& state, sys::virtual_key keycode, sys::key_modifiers mod);
	void in_game_hotkeys(sys::state& state, sys::virtual_key keycode, sys::key_modifiers mod);
	void military_screen_hotkeys(sys::state& state, sys::virtual_key keycode, sys::key_modifiers mod);
	void state_selector_hotkeys(sys::state& state, sys::virtual_key keycode, sys::key_modifiers mod);

	void console_log_pick_nation(sys::state& state, std::string_view message);
	void console_log_other(sys::state& state, std::string_view message);

	void render_ui_ingame(sys::state& state);
	void render_ui_military(sys::state& state);
	void render_ui_selection_screen(sys::state& state);

	void render_map_generic(sys::state& state);

	void on_key_down(sys::state& state, sys::virtual_key keycode, sys::key_modifiers mod);

	ui::mouse_probe recalculate_mouse_probe_identity(sys::state& state, ui::mouse_probe mouse_probe, ui::mouse_probe tooltip_probe);
	ui::mouse_probe recalculate_mouse_probe_basic(sys::state& state, ui::mouse_probe mouse_probe, ui::mouse_probe tooltip_probe);
	ui::mouse_probe recalculate_tooltip_probe_basic(sys::state& state, ui::mouse_probe mouse_probe, ui::mouse_probe tooltip_probe);
	ui::mouse_probe recalculate_mouse_probe_military(sys::state& state, ui::mouse_probe mouse_probe, ui::mouse_probe tooltip_probe);
	ui::mouse_probe recalculate_tooltip_probe_units_and_details(sys::state& state, ui::mouse_probe mouse_probe, ui::mouse_probe tooltip_probe);
	ui::mouse_probe recalculate_mouse_probe_units_and_details(sys::state& state, ui::mouse_probe mouse_probe, ui::mouse_probe tooltip_probe);

	void clean_up_basic_game_scene(sys::state& state);

	void update_basic_game_scene(sys::state& state);
	void generic_map_scene_update(sys::state& state);
	void update_ui_state_basic(sys::state& state);
	void update_ui_unit_details(sys::state& state);
	void update_military_game_scene(sys::state& state);
	void update_add_units_game_scene(sys::state& state);

	void open_chat_during_game(sys::state& state);
	void open_chat_before_game(sys::state& state);

	void highlight_player_nation(sys::state& state, std::vector<uint32_t>& data, dcon::province_id selected_province);
	void highlight_given_province(sys::state& state, std::vector<uint32_t>& data, dcon::province_id selected_province);

	ui::element_base* root_end_screen(sys::state& state);
	ui::element_base* root_pick_nation(sys::state& state);
	ui::element_base* root_game_basic(sys::state& state);
	ui::element_base* root_game_battleplanner(sys::state& state);
	ui::element_base* root_game_battleplanner_unit_selection(sys::state& state);
	ui::element_base* root_game_wargoal_state_selection(sys::state& state);
	ui::element_base* root_game_battleplanner_add_army(sys::state& state);

	struct scene_properties {
		scene_id id;
		std::function<ui::element_base* (sys::state& state)> get_root;
		bool starting_scene = false;
		bool final_scene = false;
		bool enforced_pause = false;
		bool based_on_map = true;
		bool overwrite_map_tooltip = false;
		bool accept_events = false;
		bool is_lobby = false;
		bool game_in_progress = true;
		borders_granularity borders = borders_granularity::province;
		std::function<void(sys::state& state, dcon::nation_id nation, dcon::province_id target, sys::key_modifiers mod)> rbutton_selected_units;
		std::function<void(sys::state& state, dcon::nation_id nation, dcon::province_id target, sys::key_modifiers mod)> rbutton_province;
		//drag related
		bool allow_drag_selection;
		std::function<void(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mod)> on_drag_start;
		std::function<void(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mod)> drag_selection;
		std::function<void(sys::state& state)> lbutton_up;
		//key presses related
		std::function <sys::virtual_key(sys::state& state, sys::virtual_key keycode, sys::key_modifiers mod)> keycode_mapping;
		std::function <void(sys::state& state, sys::virtual_key keycode, sys::key_modifiers mod)> handle_hotkeys;
		//logger
		std::function <void(sys::state& state, std::string_view message)> console_log;
		//render
		std::function <void(sys::state& state)> render_ui = do_nothing;
		std::function <void(sys::state& state)> render_map = render_map_generic;
		//mouse probing
		std::function <ui::mouse_probe(sys::state& state, ui::mouse_probe mouse_probe, ui::mouse_probe tooltip_probe)> recalculate_mouse_probe = recalculate_mouse_probe_identity;
		std::function <ui::mouse_probe(sys::state& state, ui::mouse_probe mouse_probe, ui::mouse_probe tooltip_probe)> recalculate_tooltip_probe = recalculate_mouse_probe_identity;
		//clean up function which is supposed to keep ui "view" of the game state consistent
		std::function <void(sys::state& state)> clean_up = do_nothing;
		std::function <void(sys::state& state)> on_game_state_update = generic_map_scene_update;
		std::function <void(sys::state& state)> on_game_state_update_update_ui = do_nothing;
		//other functions:
		std::function <void(sys::state& state)> open_chat = open_chat_during_game;
		// graphics
		std::function <void(sys::state& state, std::vector<uint32_t>& data, dcon::province_id selected_province)> update_highlight_texture = highlight_given_province;
	};

	scene_properties nation_picker();
	scene_properties basic_game();
	scene_properties state_wargoal_selector();
	scene_properties end_screen();

}
