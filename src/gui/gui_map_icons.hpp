 #pragma once

#include "dcon_generated.hpp"
#include "gui_element_types.hpp"
#include "gui_graphics.hpp"
#include "province.hpp"
#include "text.hpp"
#include "unit_tooltip.hpp"
#include "gui_land_combat.hpp"
#include "gui_naval_combat.hpp"
#include "gui_unit_grid_box.hpp"
#include "map_state.hpp"

namespace ui {

inline constexpr float big_counter_cutoff = 15.0f;
inline constexpr float prov_details_cutoff = 18.0f;

struct toggle_unit_grid {
	bool with_shift;
};

template<bool IsNear>
class port_ex_bg : public button_element_base {
	bool visible = false;

	void on_update(sys::state& state) noexcept override {
		visible = retrieve<int32_t>(state, parent) > 0;
		frame = int32_t(retrieve<outline_color>(state, parent));
	}

	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(visible && (state.map_state.get_zoom() >= big_counter_cutoff) == IsNear)
			button_element_base::render(state, x, y);
	}
	mouse_probe impl_probe_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override {
		if(visible && (state.map_state.get_zoom() >= big_counter_cutoff) == IsNear)
			return button_element_base::impl_probe_mouse(state, x, y, type);
		else
			return mouse_probe{ nullptr, ui::xy_pair{} };
	}
	void button_shift_action(sys::state& state) noexcept override {
		if(visible && (state.map_state.get_zoom() >= big_counter_cutoff) == IsNear)
			send(state, parent, toggle_unit_grid{ true });
	}
	void button_action(sys::state& state) noexcept override {
		if(visible && (state.map_state.get_zoom() >= big_counter_cutoff) == IsNear)
			send(state, parent, toggle_unit_grid{ false });
	}
};

template<bool IsNear>
class port_sm_bg : public image_element_base {
	bool visible = false;

	void on_update(sys::state& state) noexcept override {
		visible = retrieve<int32_t>(state, parent) == 0;
		frame = int32_t(retrieve<outline_color>(state, parent));
	}

	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(visible && (state.map_state.get_zoom() >= big_counter_cutoff) == IsNear)
			image_element_base::render(state, x, y);
	}
};

class port_level_bar : public image_element_base {
public:
	bool visible = false;
	int32_t level = 1;

	void on_update(sys::state& state) noexcept override {
		auto prov = retrieve<dcon::province_id>(state, parent);
		auto port_level = state.world.province_get_building_level(prov, economy::province_building_type::naval_base);
		visible = port_level >= level;
		frame = int32_t(retrieve<outline_color>(state, parent));
	}
	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(visible && state.map_state.get_zoom() >= big_counter_cutoff)
			image_element_base::render(state, x, y);
	}
};

template<bool IsNear>
class port_ship_count : public color_text_element {
public:
	void on_update(sys::state& state) noexcept override {
		int32_t count = retrieve<int32_t>(state, parent);
		color = text::text_color::white;
		if(count <= 0) {
			set_text(state, "");
		} else {
			set_text(state, std::to_string(count));
		}
	}
	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if((state.map_state.get_zoom() >= big_counter_cutoff) == IsNear)
			color_text_element::render(state, x, y);
	}
};

class port_window : public window_element_base {
public:
	bool visible = true;
	bool populated = false;
	float map_x = 0;
	float map_y = 0;
	dcon::province_id port_for;
	outline_color color = outline_color::gray;
	int32_t displayed_count = 0;

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "level1") {
			auto ptr = make_element_by_type<port_level_bar>(state, id);
			ptr->level = 1;
			return ptr;
		} else if(name == "level2") {
			auto ptr = make_element_by_type<port_level_bar>(state, id);
			ptr->level = 2;
			return ptr;
		} else if(name == "level3") {
			auto ptr = make_element_by_type<port_level_bar>(state, id);
			ptr->level = 3;
			return ptr;
		} else if(name == "level4") {
			auto ptr = make_element_by_type<port_level_bar>(state, id);
			ptr->level = 4;
			return ptr;
		} else if(name == "level5") {
			auto ptr = make_element_by_type<port_level_bar>(state, id);
			ptr->level = 5;
			return ptr;
		} else if(name == "level6") {
			auto ptr = make_element_by_type<port_level_bar>(state, id);
			ptr->level = 6;
			return ptr;
		} else if(name == "ship_count") {
			return make_element_by_type<port_ship_count<true>>(state, id);
		} else if(name == "port_minimized") {
			return make_element_by_type<port_sm_bg<true>>(state, id);
		} else if(name == "port_expanded") {
			return make_element_by_type<port_ex_bg<true>>(state, id);
		} else if(name == "port_collapsed_small") {
			return make_element_by_type<port_ex_bg<false>>(state, id);
		} else if(name == "port_collapsed_small_icon") {
			return make_element_by_type<port_sm_bg<false>>(state, id);
		} else if(name == "collapsed_ship_count") {
			return make_element_by_type<port_ship_count<false>>(state, id);
		} else {
			return nullptr;
		}
	}

	void set_province(sys::state& state, dcon::province_id p) {
		port_for = p;
		glm::vec2 map_size = glm::vec2(state.map_state.map_data.size_x, state.map_state.map_data.size_y);
		glm::vec2 v = map::get_port_location(state, p) / map_size;
		map_x = v.x;
		map_y = v.y;
	}

	void on_update(sys::state& state) noexcept override {

		auto navies = state.world.province_get_navy_location(port_for);
		if(state.world.province_get_building_level(port_for, economy::province_building_type::naval_base) == 0 && navies.begin() == navies.end()) {
			populated = false;
			return;
		}

		populated = true;
		displayed_count = 0;

		if(navies.begin() == navies.end()) {
			auto controller = state.world.province_get_nation_from_province_control(port_for);
			if(controller == state.local_player_nation) {
				color = outline_color::blue;
			} else if(!controller || military::are_at_war(state, controller, state.local_player_nation)) {
				color = outline_color::red;
			} else if(military::are_allied_in_war(state, controller, state.local_player_nation)) {
				color = outline_color::cyan;
			} else {
				color = outline_color::gray;
			}
		} else {
			bool player_navy = false;
			bool allied_navy = false;
			bool enemy_navy = false;
			bool selected_navy = false;
			for(auto n : navies) {
				auto controller = n.get_navy().get_controller_from_navy_control();
				if(state.is_selected(n.get_navy())) {
					selected_navy = true;
				} else if(controller == state.local_player_nation) {
					player_navy = true;
				} else if(!controller || military::are_at_war(state, controller, state.local_player_nation)) {
					enemy_navy = true;
				} else if(military::are_allied_in_war(state, controller, state.local_player_nation)) {
					allied_navy = true;;
				}

				auto srange = n.get_navy().get_navy_membership();
				int32_t num_ships = int32_t(srange.end() - srange.begin());
				displayed_count += num_ships;
			}

			if(selected_navy) {
				color = outline_color::gold;
			} else if(player_navy) {
				color = outline_color::blue;
			} else if(enemy_navy) {
				color = outline_color::red;
			} else if(allied_navy) {
				color = outline_color::cyan;
			} else {
				color = outline_color::gray;
			}
		}

	}

	void impl_render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(populated) {
			glm::vec2 map_pos(map_x, 1.0f - map_y);
			auto screen_size = glm::vec2{ float(state.x_size / state.user_settings.ui_scale), float(state.y_size / state.user_settings.ui_scale) };
			glm::vec2 screen_pos;
			visible = false;
			if(!state.map_state.map_to_screen(state, map_pos, screen_size, screen_pos))
				return;
			if(!state.map_state.visible_provinces[province::to_map_id(port_for)])
				return;
			visible = true;
			//
			auto new_position = xy_pair{ int16_t(screen_pos.x), int16_t(screen_pos.y) };
			base_data.position = new_position;
			window_element_base::impl_render(state, new_position.x, new_position.y);
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::province_id>()) {
			payload.emplace<dcon::province_id>(port_for);
			return message_result::consumed;
		} else if(payload.holds_type<outline_color>()) {
			payload.emplace<outline_color>(color);
			return message_result::consumed;
		} else if(payload.holds_type<int32_t>()) {
			payload.emplace<int32_t>(displayed_count);
			return message_result::consumed;
		}
		return message_result::unseen;
	}

	mouse_probe impl_probe_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override {
		if(visible)
			return window_element_base::impl_probe_mouse(state, x, y, type);
		return mouse_probe{ nullptr, ui::xy_pair{} };
	}
};

struct top_display_parameters {
	float top_left_value = 0.0f;
	float top_right_value = 0.0f;
	float top_left_org_value = 0.0f;
	float top_right_org_value = 0.0f;
	float battle_progress = 0.0f;
	dcon::nation_id top_left_nation;
	dcon::nation_id top_right_nation;
	dcon::rebel_faction_id top_left_rebel;
	dcon::rebel_faction_id top_right_rebel;
	int8_t top_left_status = 0;
	int8_t top_dig_in = -1;
	int8_t top_right_dig_in = -1;
	int8_t right_frames = 0;
	int8_t colors_used = 0;
	int8_t common_unit_1 = -1;
	int8_t common_unit_2 = -1;
	std::array<outline_color, 5> colors;
	bool is_army = false;
	float attacker_casualties = 0.0f;
	float defender_casualties = 0.0f;
	bool player_involved_battle = false;
	bool player_is_attacker = false;
};

class prov_map_siege_bar : public progress_bar {
public:
	void on_update(sys::state& state) noexcept override {
		auto prov = retrieve<dcon::province_id>(state, parent);
		progress = state.world.province_get_siege_progress(prov);
	}
};

class map_siege : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "progress") {
			return make_element_by_type<prov_map_siege_bar>(state, id);
		} else {
			return nullptr;
		}
	}

	mouse_probe impl_probe_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override {
		if(state.map_state.get_zoom() >= big_counter_cutoff)
			return window_element_base::impl_probe_mouse(state, x, y, type);
		return window_element_base::impl_probe_mouse(state, x, y + 23, type);
	}
	void impl_render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(state.map_state.get_zoom() >= big_counter_cutoff)
			window_element_base::impl_render(state, x, y);
		else
			window_element_base::impl_render(state, x, y - 23);
	}
};

class prov_map_battle_bar : public progress_bar {
public:
	void on_update(sys::state& state) noexcept override {
		top_display_parameters* params = retrieve<top_display_parameters*>(state, parent);
		progress = params->battle_progress;
	}
	message_result test_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override {
		return opaque_element_base::test_mouse(state, x, y, type);
	}
	message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override {
		sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume * state.user_settings.master_volume);

		auto prov = retrieve<dcon::province_id>(state, parent);

		dcon::land_battle_id lbattle;
		for(auto b : state.world.province_get_land_battle_location(prov)) {
			auto w = b.get_battle().get_war_from_land_battle_in_war();
			if(!w || military::get_role(state, w, state.local_player_nation) != military::war_role::none) {
				lbattle = b.get_battle();
				break;
			}
		}
		if(!lbattle) {
			auto lb = state.world.province_get_land_battle_location(prov);
			if(lb.begin() != lb.end()) {
				lbattle = (*lb.begin()).get_battle();
			}
		}
		if(lbattle) {
			if(!state.ui_state.army_combat_window) {
				auto new_elm = ui::make_element_by_type<ui::land_combat_window>(state, "alice_land_combat");
				state.ui_state.army_combat_window = new_elm.get();
				state.ui_state.root->add_child_to_front(std::move(new_elm));
			}
			land_combat_window* win = static_cast<land_combat_window*>(state.ui_state.army_combat_window);
			win->battle = lbattle;
			//
			game_scene::deselect_units(state);
			state.map_state.set_selected_province(dcon::province_id{});
			game_scene::open_province_window(state, dcon::province_id{});
			if(state.ui_state.army_status_window) {
				state.ui_state.army_status_window->set_visible(state, false);
			}
			if(state.ui_state.navy_status_window) {
				state.ui_state.navy_status_window->set_visible(state, false);
			}
			if(state.ui_state.multi_unit_selection_window) {
				state.ui_state.multi_unit_selection_window->set_visible(state, false);
			}
			if(state.ui_state.army_reorg_window) {
				state.ui_state.army_reorg_window->set_visible(state, false);
			}
			if(state.ui_state.navy_reorg_window) {
				state.ui_state.navy_reorg_window->set_visible(state, false);
			}
			//
			if(state.ui_state.army_combat_window->is_visible()) {
				state.ui_state.army_combat_window->impl_on_update(state);
			} else {
				state.ui_state.army_combat_window->set_visible(state, true);
				if(state.ui_state.naval_combat_window) {
					state.ui_state.naval_combat_window->set_visible(state, false);
				}
			}
			return message_result::consumed;
		}
		dcon::naval_battle_id nbattle;
		for(auto b : state.world.province_get_naval_battle_location(prov)) {
			auto w = b.get_battle().get_war_from_naval_battle_in_war();
			if(military::get_role(state, w, state.local_player_nation) != military::war_role::none) {
				nbattle = b.get_battle();
				break;
			}
		}
		if(!nbattle) {
			auto lb = state.world.province_get_naval_battle_location(prov);
			if(lb.begin() != lb.end()) {
				nbattle = (*lb.begin()).get_battle();
			}
		}
		if(nbattle) {
			if(!state.ui_state.naval_combat_window) {
				auto new_elm = ui::make_element_by_type<ui::naval_combat_window>(state, "alice_naval_combat");
				state.ui_state.naval_combat_window = new_elm.get();
				state.ui_state.root->add_child_to_front(std::move(new_elm));
			}
			naval_combat_window* win = static_cast<naval_combat_window*>(state.ui_state.naval_combat_window);
			win->battle = nbattle;
			//
			game_scene::deselect_units(state);
			state.map_state.set_selected_province(dcon::province_id{});
			game_scene::open_province_window(state, dcon::province_id{});
			if(state.ui_state.army_status_window) {
				state.ui_state.army_status_window->set_visible(state, false);
			}
			if(state.ui_state.navy_status_window) {
				state.ui_state.navy_status_window->set_visible(state, false);
			}
			if(state.ui_state.multi_unit_selection_window) {
				state.ui_state.multi_unit_selection_window->set_visible(state, false);
			}
			if(state.ui_state.army_reorg_window) {
				state.ui_state.army_reorg_window->set_visible(state, false);
			}
			if(state.ui_state.navy_reorg_window) {
				state.ui_state.navy_reorg_window->set_visible(state, false);
			}
			//
			if(state.ui_state.naval_combat_window->is_visible()) {
				state.ui_state.naval_combat_window->impl_on_update(state);
			} else {
				state.ui_state.naval_combat_window->set_visible(state, true);
				if(state.ui_state.army_combat_window) {
					state.ui_state.army_combat_window->set_visible(state, false);
				}
			}
		}
		return message_result::consumed;
	}
};

class prov_map_br_overlay : public image_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		image_element_base::on_create(state);
		frame = 1;
	}
 };

class tl_attacker_casualties : public color_text_element {
public:
	void on_update(sys::state& state) noexcept override {
		top_display_parameters* params = retrieve<top_display_parameters*>(state, parent);
		color = text::text_color::red;

		auto cas = params->attacker_casualties * state.defines.pop_size_per_regiment;
		cas = floor(cas);

		if(cas < 5) {
			set_text(state, "");
		} else {
			set_text(state, '-' + std::to_string(int32_t(cas)));
		}
		
	}
 	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		top_display_parameters* params = retrieve<top_display_parameters*>(state, parent);
		auto player_involved_battle = params->player_involved_battle;
		auto player_is_attacker = params->player_is_attacker;

		if(state.map_state.get_zoom() >= big_counter_cutoff) {
			if(player_involved_battle && player_is_attacker) {
				color_text_element::render(state, x - 14, y - 80);
			} else if (player_involved_battle && !player_is_attacker) {
				color_text_element::render(state, x + 55, y - 80);
			}
		} else {
			if(player_involved_battle && player_is_attacker) {
				color_text_element::render(state, x - 14, y - 38);
			} else if(player_involved_battle && !player_is_attacker) {
				color_text_element::render(state, x + 55, y - 38);
			}
		}
		
	}
};

class tl_defender_casualties : public color_text_element {
public:
	void on_update(sys::state& state) noexcept override {
		top_display_parameters* params = retrieve<top_display_parameters*>(state, parent);
		color = text::text_color::red;

		auto cas = params->defender_casualties * state.defines.pop_size_per_regiment;
		cas = floor(cas);

		if (cas < 5) {
			set_text(state, "");
		} else {
			set_text(state, '-' + std::to_string(int32_t(cas)));
		}

	}
	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		top_display_parameters* params = retrieve<top_display_parameters*>(state, parent);
		auto player_involved_battle = params->player_involved_battle;
		auto player_is_attacker = params->player_is_attacker;

		if(state.map_state.get_zoom() >= big_counter_cutoff) {
			if (player_involved_battle && player_is_attacker)
				color_text_element::render(state, x + 55, y - 80);
			else if (player_involved_battle && !player_is_attacker)
				color_text_element::render(state, x - 14, y - 80);
		} else {
			if (player_involved_battle && player_is_attacker)
				color_text_element::render(state, x + 55, y - 38);
			else if (player_involved_battle && !player_is_attacker)
				color_text_element::render(state, x - 14, y - 38);
		}
	}
};

class map_battle : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "progress") {
			return make_element_by_type<prov_map_battle_bar>(state, id);
		} else if(name == "overlay_right") {
			return make_element_by_type<prov_map_br_overlay>(state, id);
		} else if(name == "defender_casualties") {
			return make_element_by_type <tl_defender_casualties>(state, id);
		} else if(name == "attacker_casualties") {
			return make_element_by_type <tl_attacker_casualties>(state, id);
		} else {
			return nullptr;
		}
	}

	mouse_probe impl_probe_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override {
		if(state.map_state.get_zoom() >= big_counter_cutoff)
			return window_element_base::impl_probe_mouse(state, x, y, type);
		return window_element_base::impl_probe_mouse(state, x, y + 23, type);
	}
	void impl_render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(state.map_state.get_zoom() >= big_counter_cutoff)
			window_element_base::impl_render(state, x, y);
		else
			window_element_base::impl_render(state, x, y - 22);
	}
};

class tr_frame_bg : public button_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		button_element_base::on_create(state);
		frame = int32_t(outline_color::red);
	}
};

class tr_edge : public image_element_base {
public:
	int32_t number = 0;
	bool visible = false;

	void on_update(sys::state& state) noexcept override {
		top_display_parameters* params = retrieve<top_display_parameters*>(state, parent);
		visible = params->right_frames > number;
	}

	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(visible)
			image_element_base::render(state, x, y);
	}
};

class tr_org_bar : public progress_bar {
public:
	void on_update(sys::state& state) noexcept override {
		top_display_parameters* params = retrieve<top_display_parameters*>(state, parent);
		progress = params->top_right_org_value;
	}
};

class tr_status : public image_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		image_element_base::on_create(state);
		frame = 6;
	}
};

class tr_dig_in : public image_element_base {
public:
	bool visible = false;

	void on_update(sys::state& state) noexcept override {
		top_display_parameters* params = retrieve<top_display_parameters*>(state, parent);
		if(params->top_right_dig_in >= 0) {
			frame = params->top_right_dig_in;
			visible = true;
		} else {
			visible = false;
		}
	}

	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(visible)
			image_element_base::render(state, x, y);
	}
};

class tr_strength : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		top_display_parameters* params = retrieve<top_display_parameters*>(state, parent);
		auto strength = params->top_right_value;
		if(params->is_army) {
			strength *= state.defines.pop_size_per_regiment;
			strength = floor(strength);
		}
		set_text(state, text::prettify(int32_t(strength)));
	}
};

class tr_controller_flag : public flag_button2 {
public:
	mouse_probe impl_probe_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override {
		if(type == mouse_probe_type::tooltip)
			return flag_button2::impl_probe_mouse(state, x, y, type);
		return mouse_probe{ nullptr, ui::xy_pair{} };
	}
	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::nation_id>()) {
			top_display_parameters* params = retrieve<top_display_parameters*>(state, parent);
			if(params)
				payload.emplace<dcon::nation_id>(params->top_right_nation);
			else 
				payload.emplace<dcon::nation_id>(dcon::nation_id{});
			return message_result::consumed;
		} else if(payload.holds_type<dcon::rebel_faction_id>()) {
			top_display_parameters* params = retrieve<top_display_parameters*>(state, parent);
			if(params)
				payload.emplace<dcon::rebel_faction_id>(params->top_right_rebel);
			else
				payload.emplace<dcon::rebel_faction_id>(dcon::rebel_faction_id{});
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};

class top_right_unit_icon : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "controller_flag") {
			return make_element_by_type<tr_controller_flag>(state, id);
		} else if(name == "strength") {
			return make_element_by_type<tr_strength>(state, id);
		} else if(name == "dig_in") {
			return make_element_by_type<tr_dig_in>(state, id);
		} else if(name == "status") {
			return make_element_by_type<tr_status>(state, id);
		} else if(name == "org_bar") {
			return make_element_by_type<tr_org_bar>(state, id);
		} else if(name == "edge1") {
			auto ptr = make_element_by_type<tr_edge>(state, id);
			ptr->number = 1;
			return ptr;
		} else if(name == "edge2") {
			auto ptr = make_element_by_type<tr_edge>(state, id);
			ptr->number = 2;
			return ptr;
		} else if(name == "edge3") {
			auto ptr = make_element_by_type<tr_edge>(state, id);
			ptr->number = 3;
			return ptr;
		} else if(name == "edge4") {
			auto ptr = make_element_by_type<tr_edge>(state, id);
			ptr->number = 4;
			return ptr;
		} else if(name == "frame_bg") {
			return make_element_by_type<tr_frame_bg>(state, id);
		} else {
			return nullptr;
		}
	}

	mouse_probe impl_probe_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override {
		if(state.map_state.get_zoom() >= big_counter_cutoff)
			return window_element_base::impl_probe_mouse(state, x, y, type);
		else
			return mouse_probe{ nullptr, ui::xy_pair{0,0} };
	}
	void impl_render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(state.map_state.get_zoom() >= big_counter_cutoff)
			window_element_base::impl_render(state, x, y);
	}
};

class small_top_right_unit_icon : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "controller_flag") {
			return make_element_by_type<tr_controller_flag>(state, id);
		} else if(name == "strength") {
			return make_element_by_type<tr_strength>(state, id);
		} else if(name == "frame_bg") {
			return make_element_by_type<tr_frame_bg>(state, id);
		} else {
			return nullptr;
		}
	}

	mouse_probe impl_probe_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override {
		if(state.map_state.get_zoom() < big_counter_cutoff)
			return window_element_base::impl_probe_mouse(state, x, y, type);
		else
			return mouse_probe{ nullptr, ui::xy_pair{0,0}};
	}
	void impl_render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(state.map_state.get_zoom() < big_counter_cutoff)
			window_element_base::impl_render(state, x, y);
	}
};

class tl_edge : public image_element_base {
public:
	int32_t number = 0;
	bool visible = false;

	void on_update(sys::state& state) noexcept override {
		top_display_parameters* params = retrieve<top_display_parameters*>(state, parent);
		if(number >= params->colors_used) {
			visible = false;
		} else {
			frame = int32_t(params->colors[number]);
			visible = true;
		}
	}

	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(visible)
			image_element_base::render(state, x, y);
	}
};

class tl_frame_bg : public button_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		top_display_parameters* params = retrieve<top_display_parameters*>(state, parent);
		frame = int32_t(params->colors[0]);
	}
	void button_action(sys::state& state) noexcept override {
		send(state, parent, toggle_unit_grid{ false });
	}
	void button_shift_action(sys::state& state) noexcept override {
		send(state, parent, toggle_unit_grid{ true });
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		populate_unit_tooltip(state, contents, retrieve<dcon::province_id>(state, parent));
	}
};

class tl_org_bar : public progress_bar {
public:
	void on_update(sys::state& state) noexcept override {
		top_display_parameters* params = retrieve<top_display_parameters*>(state, parent);
		progress = params->top_left_org_value;
	}
};

class tl_status : public image_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		top_display_parameters* params = retrieve<top_display_parameters*>(state, parent);

		if(params->top_left_status >= 0) {
			frame = params->top_left_status;
		} else {
			frame = 0;
		}
	}
};

class tl_strength : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		top_display_parameters* params = retrieve<top_display_parameters*>(state, parent);
		auto strength = params->top_left_value;
		if(params->is_army) {
			strength *= state.defines.pop_size_per_regiment;
			strength = floor(strength);
		}
		set_text(state, text::prettify(int32_t(strength)));

	}
};

class tl_dig_in : public image_element_base {
public:
	bool visible = false;

	void on_update(sys::state& state) noexcept override {
		top_display_parameters* params = retrieve<top_display_parameters*>(state, parent);
		if(params->top_dig_in >= 0) {
			frame = params->top_dig_in;
			visible = true;
		} else {
			visible = false;
		}
	}

	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(visible)
			image_element_base::render(state, x, y);
	}
};

class tl_unit_1 : public image_element_base {
public:
	bool visible = true;

	void on_update(sys::state& state) noexcept override {
		top_display_parameters* params = retrieve<top_display_parameters*>(state, parent);
		if(params->common_unit_1 >= 0) {
			frame = params->common_unit_1;
			visible = true;
		} else {
			visible = false;
		}
	}
	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(visible)
			image_element_base::render(state, x, y);
	}
};

class tl_unit_2 : public image_element_base {
public:
	bool visible = true;

	void on_update(sys::state& state) noexcept override {
		top_display_parameters* params = retrieve<top_display_parameters*>(state, parent);
		if(params->common_unit_2 >= 0) {
			frame = params->common_unit_2;
			visible = true;
		} else {
			visible = false;
		}
	}
	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(visible)
			image_element_base::render(state, x, y);
	}
};



class tl_controller_flag : public flag_button2 {
public:
	bool visible = true;
	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::nation_id>()) {
			top_display_parameters* params = retrieve<top_display_parameters*>(state, parent);
			if(params)
				payload.emplace<dcon::nation_id>(params->top_left_nation);
			else
				payload.emplace<dcon::nation_id>(dcon::nation_id{});
			return message_result::consumed;
		} else if(payload.holds_type<dcon::rebel_faction_id>()) {
			top_display_parameters* params = retrieve<top_display_parameters*>(state, parent);
			if(params)
				payload.emplace<dcon::rebel_faction_id>(params->top_left_rebel);
			else
				payload.emplace<dcon::rebel_faction_id>(dcon::rebel_faction_id{});
			return message_result::consumed;
		}
		return message_result::unseen;
	}

	void on_update(sys::state& state) noexcept override {
		top_display_parameters* params = retrieve<top_display_parameters*>(state, parent);

		visible =  params->top_left_nation != state.local_player_nation;
		if(visible)
			flag_button2::on_update(state);
	}
	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(visible)
			flag_button2::render(state, x, y);
	}
	mouse_probe impl_probe_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override {
		if(visible && type == mouse_probe_type::tooltip)
			return flag_button2::impl_probe_mouse(state, x, y, type);
		return mouse_probe{ nullptr, ui::xy_pair{} };
	}
};

class tl_sm_controller_flag : public flag_button {
public:
	mouse_probe impl_probe_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override {
		if(type == mouse_probe_type::tooltip)
			return flag_button::impl_probe_mouse(state, x, y, type);
		return mouse_probe{ nullptr, ui::xy_pair{} };
	}
	dcon::national_identity_id get_current_nation(sys::state& state) noexcept override {
		top_display_parameters* params = retrieve<top_display_parameters*>(state, parent);
		if(params)
			return state.world.nation_get_identity_from_identity_holder(params->top_left_nation);
		return dcon::national_identity_id{};
	}
};

class top_unit_icon : public window_element_base {
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "controller_flag") {
			return make_element_by_type<tl_controller_flag>(state, id);
		} else if(name == "strength") {
			return make_element_by_type<tl_strength>(state, id);
		} else if(name == "unit_1") {
			return make_element_by_type<tl_unit_1>(state, id);
		} else if(name == "unit_2") {
			return make_element_by_type<tl_unit_2>(state, id);
		} else if(name == "dig_in") {
			return make_element_by_type<tl_dig_in>(state, id);
		} else if(name == "status") {
			return make_element_by_type<tl_status>(state, id);
		} else if(name == "org_bar") {
			return make_element_by_type<tl_org_bar>(state, id);
		} else if(name == "edge1") {
			auto ptr = make_element_by_type<tl_edge>(state, id);
			ptr->number = 1;
			return ptr;
		} else if(name == "edge2") {
			auto ptr = make_element_by_type<tl_edge>(state, id);
			ptr->number = 2;
			return ptr;
		} else if(name == "edge3") {
			auto ptr = make_element_by_type<tl_edge>(state, id);
			ptr->number = 3;
			return ptr;
		} else if(name == "edge4") {
			auto ptr = make_element_by_type<tl_edge>(state, id);
			ptr->number = 4;
			return ptr;
		} else if(name == "frame_bg") {
			return make_element_by_type<tl_frame_bg>(state, id);
		} else {
			return nullptr;
		}
	}

	mouse_probe impl_probe_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override {
		if(state.map_state.get_zoom() >= big_counter_cutoff)
			return window_element_base::impl_probe_mouse(state, x, y, type);
		else
			return mouse_probe{ nullptr, ui::xy_pair{0,0} };
	}
	void impl_render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(state.map_state.get_zoom() >= big_counter_cutoff)
			window_element_base::impl_render(state, x, y);
	}
};

class small_top_unit_icon : public window_element_base {
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "controller_flag") {
			return make_element_by_type<tl_sm_controller_flag>(state, id);
		} else if(name == "strength") {
			return make_element_by_type<tl_strength>(state, id);
		} else if(name == "frame_bg") {
			return make_element_by_type<tl_frame_bg>(state, id);
		} else {
			return nullptr;
		}
	}

	mouse_probe impl_probe_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override {
		if(state.map_state.get_zoom() < big_counter_cutoff)
			return window_element_base::impl_probe_mouse(state, x, y, type);
		return mouse_probe{ nullptr, ui::xy_pair{0,0} };
	}
	void impl_render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(state.map_state.get_zoom() < big_counter_cutoff)
			window_element_base::impl_render(state, x, y);
	}
};

class select_army_group_button : public button_element_base {
	void button_action(sys::state& state) noexcept override {
		send(state, parent, int32_t(1));
	}
};

class army_group_icon : public window_element_base {
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "frame_bg") {
			return make_element_by_type<select_army_group_button>(state, id);
		} else {
			return nullptr;
		}
	}

	mouse_probe impl_probe_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override {
		if(state.map_state.get_zoom() >= big_counter_cutoff)
			return window_element_base::impl_probe_mouse(state, x, y, type);
		else
			return mouse_probe{ nullptr, ui::xy_pair{0,0} };
	}
	void impl_render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(state.map_state.get_zoom() >= big_counter_cutoff)
			window_element_base::impl_render(state, x, y);
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<int32_t>()) {
			send(state, parent, 1);
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};

class army_group_counter_window : public window_element_base {
public:
	sys::army_group* data = nullptr;
	element_base* main_icon = nullptr;
	bool populated = false;
	bool visible = true;
	dcon::province_id prov;
	outline_color color;

	xy_pair base_size;

	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);
		base_size = window_element_base::base_data.size;
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "main_army_group_icon") {
			auto ptr = make_element_by_type<army_group_icon>(state, id);
			main_icon = ptr.get();
			return ptr;
		} else {
			return nullptr;
		}
	}

	void impl_on_update(sys::state& state) noexcept override {
		on_update(state);

		for(auto& c : children) {
			if(c->is_visible()) {
				c->impl_on_update(state);
			}
		}
	}

	void on_update(sys::state& state) noexcept override {
		populated = false;
		for(auto & item : state.army_groups) {
			if(item.hq == prov) {
				data = &item;
				populated = true;
			}
		}

		if(state.selected_army_group != nullptr) {
			if(populated) {
				if(state.selected_army_group->hq == data->hq) {
					// make it distinct from the others in some way
				} else {
					// set it back to default
				}
			}
		}			
	}

	void impl_render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(populated) {
			auto mid_point = state.world.province_get_mid_point(prov);
			auto map_pos = state.map_state.normalize_map_coord(mid_point);
			auto screen_size = glm::vec2{ float(state.x_size / state.user_settings.ui_scale), float(state.y_size / state.user_settings.ui_scale) };
			glm::vec2 screen_pos;
			if(!state.map_state.map_to_screen(state, map_pos, screen_size, screen_pos)) {
				visible = false;
				return;
			}
			if(!state.map_state.visible_provinces[province::to_map_id(prov)]) {
				visible = false;
				return;
			}
			visible = true;

			auto new_position = xy_pair{ int16_t(screen_pos.x), int16_t(screen_pos.y) };
			base_data.position = new_position;
			window_element_base::impl_render(state, new_position.x, new_position.y);
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<int32_t>()) {
			if(populated)
				state.smart_select_army_group(data);
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};

class unit_counter_org_bar : public vertical_progress_bar {
public:
	void on_update(sys::state& state) noexcept override {
		float total = 0.f;
		float value = 0.f;
		auto prov = retrieve<dcon::province_id>(state, parent);
		if(bool(retrieve<dcon::army_id>(state, parent))) {
			for(const auto al : state.world.province_get_army_location_as_location(prov)) {
				for(const auto memb : al.get_army().get_army_membership()) {
					value += memb.get_regiment().get_org();
					total += 1.f;
				}
			}
		}
		if(bool(retrieve<dcon::navy_id>(state, parent))) {
			for(const auto al : state.world.province_get_navy_location_as_location(prov)) {
				for(const auto memb : al.get_navy().get_navy_membership()) {
					value += memb.get_ship().get_org();
					total += 1.f;
				}
			}
		}
		progress = (total == 0.f) ? 0.f : value / total;
	}
};

class unit_counter_flag : public flag_button2 {
public:
	mouse_probe impl_probe_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override {
		if(type == mouse_probe_type::tooltip)
			return flag_button2::impl_probe_mouse(state, x, y, type);
		return mouse_probe{ nullptr, ui::xy_pair{} };
	}
};

class unit_counter_strength : public simple_text_element_base {
	void on_update(sys::state& state) noexcept override {
		auto prov = retrieve<dcon::province_id>(state, parent);
		float value = 0.f;
		bool has_navy = bool(retrieve<dcon::navy_id>(state, parent));
		if(!has_navy) {
			for(const auto al : state.world.province_get_army_location_as_location(prov)) {
				for(const auto memb : al.get_army().get_army_membership()) {
					value += memb.get_regiment().get_strength() * 3.f;
				}
			}
		} else {
			for(const auto al : state.world.province_get_navy_location_as_location(prov)) {
				for(const auto memb : al.get_navy().get_navy_membership()) {
					value += memb.get_ship().get_strength();
				}
			}
		}
		set_text(state, text::prettify(int64_t(value)));
	}
};

class unit_counter_attrition : public image_element_base {
	bool visible = false;
public:
	void on_update(sys::state& state) noexcept override {
		visible = false;
		auto n = retrieve<dcon::navy_id>(state, parent);
		if(n && military::will_recieve_attrition(state, n)) {
			visible = true;
		}
		auto a = retrieve<dcon::army_id>(state, parent);
		if(a && military::will_recieve_attrition(state, a)) {
			visible = true;
		}
	}
	void impl_render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(visible)
			image_element_base::impl_render(state, x, y);
	}
};

class unit_counter_color_bg : public image_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto prov = retrieve<dcon::province_id>(state, parent);
		frame = 0;
		auto n = retrieve<dcon::navy_id>(state, parent);
		if(n) {
			if(state.world.navy_get_controller_from_navy_control(n) != state.local_player_nation) {
				frame = 2;
				if(military::are_at_war(state, state.world.navy_get_controller_from_navy_control(n), state.local_player_nation)) {
					frame = 1;
				} else if(military::are_allied_in_war(state, state.world.navy_get_controller_from_navy_control(n), state.local_player_nation)) {
					frame = 3;
				}
			}
			return;
		}
		auto a = retrieve<dcon::army_id>(state, parent);
		if(state.world.army_get_controller_from_army_control(a) != state.local_player_nation) {
			frame = 2;
			if(military::are_at_war(state, state.world.army_get_controller_from_army_control(a), state.local_player_nation)
			|| state.world.army_get_controller_from_army_rebel_control(a)) {
				frame = 1;
			} else if(military::are_allied_in_war(state, state.world.army_get_controller_from_army_control(a), state.local_player_nation)) {
				frame = 3;
			}
		}
	}
};

class unit_counter_bg : public button_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto prov = retrieve<dcon::province_id>(state, parent);
		auto n = retrieve<dcon::navy_id>(state, parent);
		frame = n ? 1 : 0;
	}

	void button_action(sys::state& state) noexcept override {
		auto prov = retrieve<dcon::province_id>(state, parent);
		state.selected_armies.clear();
		state.selected_navies.clear();
		if(bool(retrieve<dcon::navy_id>(state, parent))) {
			for(const auto al : state.world.province_get_navy_location_as_location(prov)) {
				if(al.get_navy().get_controller_from_navy_control() == state.local_player_nation)
					state.select(al.get_navy());
			}
		}
		if(state.selected_navies.empty() && bool(retrieve<dcon::army_id>(state, parent))) {
			for(const auto al : state.world.province_get_army_location_as_location(prov)) {
				if(al.get_army().get_controller_from_army_control() == state.local_player_nation)
					state.select(al.get_army());
			}
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		populate_unit_tooltip(state, contents, retrieve<dcon::province_id>(state, parent));
	}
};

template<bool PosAtPort>
class unit_counter_window : public window_element_base {
public:
	bool visible = true;
	bool populated = false;
	dcon::province_id prov;
	dcon::army_id army;
	dcon::navy_id navy;

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "unit_panel_bg") {
			return make_element_by_type<unit_counter_bg>(state, id);
		} else if(name == "unit_panel_color") {
			return make_element_by_type<unit_counter_color_bg>(state, id);
		} else if(name == "unit_strength") {
			return make_element_by_type<unit_counter_strength>(state, id);
		} else if(name == "unit_panel_org_bar") {
			return make_element_by_type<unit_counter_org_bar>(state, id);
		} else if(name == "unit_panel_country_flag") {
			auto ptr = make_element_by_type<unit_counter_flag>(state, id);
			ptr->base_data.position.y -= 1; //nudge
			return ptr;
		} else if(name == "unit_panel_attr") {
			return make_element_by_type<unit_counter_attrition>(state, id);
		} else {
			return nullptr;
		}
	}

	void impl_on_update(sys::state& state) noexcept override {
		on_update(state);
		if(!populated)
			return;

		for(auto& c : children) {
			if(c->is_visible()) {
				c->impl_on_update(state);
			}
		}
	}

	void on_update(sys::state& state) noexcept override {
		if constexpr(PosAtPort) {
			army = dcon::army_id{};
			navy = dcon::navy_id{};
			for(auto al : state.world.province_get_navy_location_as_location(prov)) {
				if(al.get_navy()) {
					navy = al.get_navy();
					if(al.get_navy().get_controller_from_navy_control() == state.local_player_nation)
						break;
				}
			}
		} else {
			army = dcon::army_id{};
			for(auto al : state.world.province_get_army_location_as_location(prov)) {
				if(al.get_army()) {
					army = al.get_army();
					if(al.get_army().get_controller_from_army_control() == state.local_player_nation)
						break;
				}
			}
			navy = dcon::navy_id{};
			if(prov.index() >= state.province_definitions.first_sea_province.index()) {
				for(auto al : state.world.province_get_navy_location_as_location(prov)) {
					if(al.get_navy()) {
						navy = al.get_navy();
						if(al.get_navy().get_controller_from_navy_control() == state.local_player_nation)
							break;
					}
				}
			}
		}
		populated = bool(army) || bool(navy);
	}

	void impl_render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(populated) {
			glm::vec2 map_pos;
			if constexpr(PosAtPort) {
				glm::vec2 map_size = glm::vec2(state.map_state.map_data.size_x, state.map_state.map_data.size_y);
				glm::vec2 v = map::get_port_location(state, prov) / map_size;
				map_pos = glm::vec2(v.x, 1.f - v.y);
			} else {
				auto mid_point = state.world.province_get_mid_point(prov);
				map_pos = state.map_state.normalize_map_coord(mid_point);
			}
			auto screen_size = glm::vec2{ float(state.x_size / state.user_settings.ui_scale), float(state.y_size / state.user_settings.ui_scale) };
			glm::vec2 screen_pos;
			if(!state.map_state.map_to_screen(state, map_pos, screen_size, screen_pos)) {
				visible = false;
				return;
			}
			if(!state.map_state.visible_provinces[province::to_map_id(prov)]) {
				visible = false;
				return;
			}
			visible = true;
			auto new_position = xy_pair{ int16_t(screen_pos.x), int16_t(screen_pos.y) };
			new_position.x += 7 - base_data.size.x / 2;
			new_position.y -= 22;
			base_data.position = new_position;
			base_data.flags &= ~ui::element_data::orientation_mask; //position upperleft
			window_element_base::impl_render(state, new_position.x, new_position.y);
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::province_id>()) {
			payload.emplace<dcon::province_id>(prov);
			return message_result::consumed;
		} else if(payload.holds_type<dcon::army_id>()) {
			payload.emplace<dcon::army_id>(army);
			return message_result::consumed;
		} else if(payload.holds_type<dcon::navy_id>()) {
			payload.emplace<dcon::navy_id>(navy);
			return message_result::consumed;
		} else if(payload.holds_type<dcon::rebel_faction_id>()) {
			payload.emplace<dcon::rebel_faction_id>(state.world.army_get_controller_from_army_rebel_control(army));
			return message_result::consumed;
		} else if(payload.holds_type<dcon::nation_id>()) {
			payload.emplace<dcon::nation_id>(state.world.army_get_controller_from_army_control(army));
			return message_result::consumed;
		}
		return message_result::unseen;
	}

	mouse_probe impl_probe_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override {
		if(visible && populated)
			return window_element_base::impl_probe_mouse(state, x, y, type);
		return mouse_probe{ nullptr, ui::xy_pair{} };
	}
};


class siege_counter_progress : public progress_bar {
public:
	void on_update(sys::state& state) noexcept override {
		auto prov = retrieve<dcon::province_id>(state, parent);
		progress = state.world.province_get_siege_progress(prov);
	}
};

class siege_counter_window : public window_element_base {
public:
	bool visible = true;
	bool populated = false;
	dcon::province_id prov;
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "siege_progress_bar") {
			auto ptr = make_element_by_type<siege_counter_progress>(state, id);
			ptr->base_data.position.y -= 1;
			return ptr;
		} else {
			return nullptr;
		}
	}

	void impl_on_update(sys::state& state) noexcept override {
		on_update(state);
		if(!populated)
			return;

		for(auto& c : children) {
			if(c->is_visible()) {
				c->impl_on_update(state);
			}
		}
	}

	void on_update(sys::state& state) noexcept override {
		populated = state.world.province_get_siege_progress(prov) > 0.f;
	}

	void impl_render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(populated) {
			auto mid_point = state.world.province_get_mid_point(prov);
			auto map_pos = state.map_state.normalize_map_coord(mid_point);
			auto screen_size = glm::vec2{ float(state.x_size / state.user_settings.ui_scale), float(state.y_size / state.user_settings.ui_scale) };
			glm::vec2 screen_pos;
			if(!state.map_state.map_to_screen(state, map_pos, screen_size, screen_pos)) {
				visible = false;
				return;
			}
			if(!state.map_state.visible_provinces[province::to_map_id(prov)]) {
				visible = false;
				return;
			}
			visible = true;
			auto new_position = xy_pair{ int16_t(screen_pos.x), int16_t(screen_pos.y) };
			new_position.x += 7 - base_data.size.x / 2;
			new_position.y += -4;
			base_data.position = new_position;
			window_element_base::impl_render(state, new_position.x, new_position.y);
		}
	}
	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::province_id>()) {
			payload.emplace<dcon::province_id>(prov);
			return message_result::consumed;
		}
		return message_result::unseen;
	}
	mouse_probe impl_probe_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override {
		if(visible && populated)
			return window_element_base::impl_probe_mouse(state, x, y, type);
		return mouse_probe{ nullptr, ui::xy_pair{} };
	}
};

class battle_panel_bg : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		auto lbattle = retrieve<dcon::land_battle_id>(state, parent);
		auto nbattle = retrieve<dcon::naval_battle_id>(state, parent);
		if(lbattle || nbattle) {
			game_scene::deselect_units(state);
			state.map_state.set_selected_province(dcon::province_id{});
			game_scene::open_province_window(state, dcon::province_id{});
			if(state.ui_state.army_status_window) {
				state.ui_state.army_status_window->set_visible(state, false);
			}
			if(state.ui_state.navy_status_window) {
				state.ui_state.navy_status_window->set_visible(state, false);
			}
			if(state.ui_state.multi_unit_selection_window) {
				state.ui_state.multi_unit_selection_window->set_visible(state, false);
			}
			if(state.ui_state.army_reorg_window) {
				state.ui_state.army_reorg_window->set_visible(state, false);
			}
			if(state.ui_state.navy_reorg_window) {
				state.ui_state.navy_reorg_window->set_visible(state, false);
			}
		} else {
			return;
		}
		//
		if(lbattle) {
			if(!state.ui_state.army_combat_window) {
				auto new_elm = ui::make_element_by_type<ui::land_combat_window>(state, "alice_land_combat");
				state.ui_state.army_combat_window = new_elm.get();
				state.ui_state.root->add_child_to_front(std::move(new_elm));
			}
			land_combat_window* win = static_cast<land_combat_window*>(state.ui_state.army_combat_window);
			win->battle = lbattle;
			//
			if(state.ui_state.army_combat_window->is_visible()) {
				state.ui_state.army_combat_window->impl_on_update(state);
			} else {
				state.ui_state.army_combat_window->set_visible(state, true);
				if(state.ui_state.naval_combat_window) {
					state.ui_state.naval_combat_window->set_visible(state, false);
				}
			}
		} else if(nbattle) {
			if(!state.ui_state.naval_combat_window) {
				auto new_elm = ui::make_element_by_type<ui::naval_combat_window>(state, "alice_naval_combat");
				state.ui_state.naval_combat_window = new_elm.get();
				state.ui_state.root->add_child_to_front(std::move(new_elm));
			}
			naval_combat_window* win = static_cast<naval_combat_window*>(state.ui_state.naval_combat_window);
			win->battle = nbattle;
			//
			if(state.ui_state.naval_combat_window->is_visible()) {
				state.ui_state.naval_combat_window->impl_on_update(state);
			} else {
				state.ui_state.naval_combat_window->set_visible(state, true);
				if(state.ui_state.army_combat_window) {
					state.ui_state.army_combat_window->set_visible(state, false);
				}
			}
		}
	}
};

template<bool IsAttacker>
class battle_counter_strength : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto lbattle = retrieve<dcon::land_battle_id>(state, parent);
		if(lbattle) {
			float value = 0.f;
			auto w = state.world.land_battle_get_war_from_land_battle_in_war(lbattle);
			auto is_attacker = state.world.land_battle_get_war_attacker_is_attacker(lbattle);
			for(const auto a : state.world.land_battle_get_army_battle_participation(lbattle)) {
				auto const controller = a.get_army().get_army_control().get_controller();
				auto const role = military::get_role(state, w, controller);
				if((role == military::war_role::attacker && (is_attacker == IsAttacker))
				|| (role == military::war_role::defender && !(is_attacker == IsAttacker))
				|| (!w && IsAttacker == bool(controller))) {
					for(const auto memb : a.get_army().get_army_membership()) {
						value += memb.get_regiment().get_strength() * 3.f;
					}
				}
			}
			set_text(state, text::prettify(int64_t(value)));
		} else {
			auto nbattle = retrieve<dcon::naval_battle_id>(state, parent);
			float value = 0.f;
			auto w = state.world.naval_battle_get_war_from_naval_battle_in_war(nbattle);
			auto is_attacker = state.world.naval_battle_get_war_attacker_is_attacker(nbattle);
			for(const auto a : state.world.naval_battle_get_navy_battle_participation(nbattle)) {
				auto const role = military::get_role(state, w, a.get_navy().get_navy_control().get_controller());
				if((role == military::war_role::attacker && (is_attacker == IsAttacker))
				|| (role == military::war_role::defender && !(is_attacker == IsAttacker))) {
					for(const auto memb : a.get_navy().get_navy_membership()) {
						value += memb.get_ship().get_strength();
					}
				}
			}
			set_text(state, text::prettify(int64_t(value)));
		}
	}
};

template<bool IsAttacker>
class battle_counter_org_bar : public vertical_progress_bar {
public:
	void on_update(sys::state& state) noexcept override {
		float total = 0.f;
		float value = 0.f;
		//
		auto lbattle = retrieve<dcon::land_battle_id>(state, parent);
		if(lbattle) {
			auto w = state.world.land_battle_get_war_from_land_battle_in_war(lbattle);
			auto is_attacker = state.world.land_battle_get_war_attacker_is_attacker(lbattle);
			for(const auto a : state.world.land_battle_get_army_battle_participation(lbattle)) {
				auto const controller = a.get_army().get_army_control().get_controller();
				auto const role = military::get_role(state, w, controller);
				if((role == military::war_role::attacker && (is_attacker == IsAttacker))
				|| (role == military::war_role::defender && !(is_attacker == IsAttacker))
				|| (!w && IsAttacker == bool(controller))) {
					for(const auto memb : a.get_army().get_army_membership()) {
						value += memb.get_regiment().get_org();
						total += 1.f;
					}
				}
			}
		} else {
			auto nbattle = retrieve<dcon::naval_battle_id>(state, parent);
			auto w = state.world.naval_battle_get_war_from_naval_battle_in_war(nbattle);
			auto is_attacker = state.world.naval_battle_get_war_attacker_is_attacker(nbattle);
			for(const auto a : state.world.naval_battle_get_navy_battle_participation(nbattle)) {
				auto const role = military::get_role(state, w, a.get_navy().get_navy_control().get_controller());
				if((role == military::war_role::attacker && (is_attacker == IsAttacker))
				|| (role == military::war_role::defender && !(is_attacker == IsAttacker))) {
					for(const auto memb : a.get_navy().get_navy_membership()) {
						value += memb.get_ship().get_org();
						total += 1.f;
					}
				}
			}
		}
		progress = (total == 0.f) ? 0.f : value / total;
	}
};

template<bool IsAttacker>
class battle_counter_flag : public flag_button {
public:
	dcon::national_identity_id get_current_nation(sys::state& state) noexcept override {
		auto lbattle = retrieve<dcon::land_battle_id>(state, parent);
		if(lbattle) {
			if(auto leader = IsAttacker
				? state.world.land_battle_get_general_from_attacking_general(lbattle)
				: state.world.land_battle_get_general_from_defending_general(lbattle); leader) {
				auto const controller = state.world.leader_get_nation_from_leader_loyalty(leader);
				if(!controller)
					return state.national_definitions.rebel_id;
				return state.world.nation_get_identity_from_identity_holder(controller);
			}
			auto w = state.world.land_battle_get_war_from_land_battle_in_war(lbattle);
			auto is_attacker = state.world.land_battle_get_war_attacker_is_attacker(lbattle);
			for(const auto a : state.world.land_battle_get_army_battle_participation(lbattle)) {
				auto const controller = a.get_army().get_army_control().get_controller();
				auto const role = military::get_role(state, w, controller);
				if((role == military::war_role::attacker && (is_attacker == IsAttacker))
				|| (role == military::war_role::defender && !(is_attacker == IsAttacker))
				|| (!w && IsAttacker == bool(controller))) {
					return state.world.nation_get_identity_from_identity_holder(controller);
				}
			}
		} else {
			auto nbattle = retrieve<dcon::naval_battle_id>(state, parent);
			if(auto leader = IsAttacker
				? state.world.naval_battle_get_admiral_from_attacking_admiral(nbattle)
				: state.world.naval_battle_get_admiral_from_defending_admiral(nbattle); leader) {
				return state.world.nation_get_identity_from_identity_holder(state.world.leader_get_nation_from_leader_loyalty(leader));
			}
			auto w = state.world.naval_battle_get_war_from_naval_battle_in_war(nbattle);
			auto is_attacker = state.world.naval_battle_get_war_attacker_is_attacker(nbattle);
			for(const auto a : state.world.naval_battle_get_navy_battle_participation(nbattle)) {
				auto const controller = a.get_navy().get_navy_control().get_controller();
				auto const role = military::get_role(state, w, controller);
				if((role == military::war_role::attacker && (is_attacker == IsAttacker))
				|| (role == military::war_role::defender && !(is_attacker == IsAttacker))) {
					return state.world.nation_get_identity_from_identity_holder(controller);
				}
			}
		}
		return state.national_definitions.rebel_id;
	}
	mouse_probe impl_probe_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override {
		if(type == mouse_probe_type::tooltip)
			return flag_button::impl_probe_mouse(state, x, y, type);
		return mouse_probe{ nullptr, ui::xy_pair{} };
	}
};

class battle_counter_window : public window_element_base {
public:
	bool visible = true;
	bool populated = false;
	dcon::province_id prov;
	dcon::land_battle_id land_battle;
	dcon::naval_battle_id naval_battle;

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "combat_panel_bg") {
			return make_element_by_type<battle_panel_bg>(state, id);
		} else if(name == "unit_strengthl") {
			return make_element_by_type<battle_counter_strength<false>>(state, id);
		} else if(name == "unit_strengthr") {
			return make_element_by_type<battle_counter_strength<true>>(state, id);
		} else if(name == "combat_country_flagl") {
			auto ptr = make_element_by_type<battle_counter_flag<false>>(state, id);
			ptr->base_data.position.y -= 1; //nudge
			return ptr;
		} else if(name == "combat_country_flagr") {
			auto ptr = make_element_by_type<battle_counter_flag<true>>(state, id);
			ptr->base_data.position.y -= 1; //nudge
			return ptr;
		} else if(name == "unit_panel_org_barl") {
			return make_element_by_type<battle_counter_org_bar<false>>(state, id);
		} else if(name == "unit_panel_org_barr") {
			return make_element_by_type<battle_counter_org_bar<true>>(state, id);
		} else {
			return nullptr;
		}
	}

	void impl_on_update(sys::state& state) noexcept override {
		on_update(state);
		if(!populated)
			return;

		for(auto& c : children) {
			if(c->is_visible()) {
				c->impl_on_update(state);
			}
		}
	}

	void on_update(sys::state& state) noexcept override {
		land_battle = dcon::land_battle_id{ };
		for(const auto lb : state.world.province_get_land_battle_location(prov)) {
			land_battle = lb.get_battle();
		}
		naval_battle = dcon::naval_battle_id{ };
		for(const auto lb : state.world.province_get_naval_battle_location(prov)) {
			naval_battle = lb.get_battle();
		}
		populated = bool(land_battle) || bool(naval_battle);
	}

	void impl_render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(populated) {
			auto mid_point = state.world.province_get_mid_point(prov);
			auto map_pos = state.map_state.normalize_map_coord(mid_point);
			auto screen_size = glm::vec2{ float(state.x_size / state.user_settings.ui_scale), float(state.y_size / state.user_settings.ui_scale) };
			glm::vec2 screen_pos;
			if(!state.map_state.map_to_screen(state, map_pos, screen_size, screen_pos)) {
				visible = false;
				return;
			}
			if(!state.map_state.visible_provinces[province::to_map_id(prov)]) {
				visible = false;
				return;
			}
			visible = true;
			auto new_position = xy_pair{ int16_t(screen_pos.x), int16_t(screen_pos.y) };
			new_position.x += 7 - base_data.size.x / 2; //114/2 = 57
			new_position.y -= 24;
			base_data.position = new_position;
			base_data.flags &= ~ui::element_data::orientation_mask; //position upperleft
			window_element_base::impl_render(state, new_position.x, new_position.y);
		}
	}
	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::province_id>()) {
			payload.emplace<dcon::province_id>(prov);
			return message_result::consumed;
		} else if(payload.holds_type<dcon::land_battle_id>()) {
			payload.emplace<dcon::land_battle_id>(land_battle);
			return message_result::consumed;
		} else if(payload.holds_type<dcon::naval_battle_id>()) {
			payload.emplace<dcon::naval_battle_id>(naval_battle);
			return message_result::consumed;
		}
		return message_result::unseen;
	}
	mouse_probe impl_probe_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override {
		if(visible && populated)
			return window_element_base::impl_probe_mouse(state, x, y, type);
		return mouse_probe{ nullptr, ui::xy_pair{} };
	}
};

class mobilization_progress_bar : public vertical_progress_bar {
public:
	void on_update(sys::state& state) noexcept override {
		auto prov = retrieve<dcon::province_id>(state, parent);
		int32_t remaining = 0;
		for(const auto p : state.world.nation_get_mobilization_schedule(state.local_player_nation)) {
			if(p.where == prov) {
				remaining++;
			}
		}
		int32_t total = military::mobilized_regiments_possible_from_province(state, prov);
		progress = (total == 0) ? 0 : float(remaining) / float(total);
	}
};

class mobilization_units_left : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto prov = retrieve<dcon::province_id>(state, parent);
		int32_t remaining = 0;
		for(const auto p : state.world.nation_get_mobilization_schedule(state.local_player_nation)) {
			if(p.where == prov) {
				remaining++;
			}
		}
		set_text(state, text::prettify(remaining));
	}
};

class mobilization_counter_window : public window_element_base {
public:
	bool visible = true;
	bool populated = false;
	dcon::province_id prov;
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "mobilization_progress_bar") {
			return make_element_by_type<mobilization_progress_bar>(state, id);
		} else if(name == "units_left") {
			return make_element_by_type<mobilization_units_left>(state, id);
		} else {
			return nullptr;
		}
	}

	void impl_on_update(sys::state& state) noexcept override {
		on_update(state);
		if(!populated)
			return;

		for(auto& c : children) {
			if(c->is_visible()) {
				c->impl_on_update(state);
			}
		}
	}

	void on_update(sys::state& state) noexcept override {
		populated = false;
		for(const auto p : state.world.nation_get_mobilization_schedule(state.local_player_nation)) {
			if(p.where == prov) {
				populated = true;
				break;
			}
		}
	}

	void impl_render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(populated) {
			auto mid_point = state.world.province_get_mid_point(prov);
			auto map_pos = state.map_state.normalize_map_coord(mid_point);
			auto screen_size = glm::vec2{ float(state.x_size / state.user_settings.ui_scale), float(state.y_size / state.user_settings.ui_scale) };
			glm::vec2 screen_pos;
			if(!state.map_state.map_to_screen(state, map_pos, screen_size, screen_pos)) {
				visible = false;
				return;
			}
			if(!state.map_state.visible_provinces[province::to_map_id(prov)]) {
				visible = false;
				return;
			}
			visible = true;
			auto new_position = xy_pair{ int16_t(screen_pos.x), int16_t(screen_pos.y) };
			new_position.x += 7 - base_data.size.x / 2; //114/2 = 57
			new_position.y -= 24 * 3;
			base_data.position = new_position;
			base_data.flags &= ~ui::element_data::orientation_mask; //position upperleft
			window_element_base::impl_render(state, new_position.x, new_position.y);
		}
	}
	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::province_id>()) {
			payload.emplace<dcon::province_id>(prov);
			return message_result::consumed;
		}
		return message_result::unseen;
	}
	mouse_probe impl_probe_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override {
		if(visible && populated)
			return window_element_base::impl_probe_mouse(state, x, y, type);
		return mouse_probe{ nullptr, ui::xy_pair{} };
	}
};

class map_pv_rail_dots : public image_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto p = retrieve<dcon::province_id>(state, parent);
		frame = 6 - state.world.province_get_building_level(p, economy::province_building_type::railroad);
	}
};
class map_pv_fort_dots : public image_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto p = retrieve<dcon::province_id>(state, parent);
		frame = 6 - state.world.province_get_building_level(p, economy::province_building_type::fort);
	}
};

class map_pv_bank : public image_element_base {
public:
	sys::date last_update;
	char cached_level = 0;

	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(last_update != state.ui_date) {
			cached_level = '0' + state.world.province_get_building_level(retrieve<dcon::province_id>(state, parent), economy::province_building_type::bank);
			last_update = state.ui_date;
		}
		image_element_base::render(state, x, y);
		ogl::color3f color{ 0.f, 0.f, 0.f };
		//ogl::render_text(state, &cached_level, 1, ogl::color_modification::none, float(x + 16 + 1.0f), float(y + 1.0f), color, 1);
	}
};

class map_pv_university : public image_element_base {
public:
	sys::date last_update;
	char cached_level = 0;

	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(last_update != state.ui_date) {
			cached_level = '0' + state.world.province_get_building_level(retrieve<dcon::province_id>(state, parent), economy::province_building_type::university);
			last_update = state.ui_date;
		}
		image_element_base::render(state, x, y);
		ogl::color3f color{ 0.f, 0.f, 0.f };
		//ogl::render_text(state, &cached_level, 1, ogl::color_modification::none, float(x + 16 + 1.0f), float(y + 1.0f), color, 1);
	}
};

class province_details_container : public window_element_base {
public:
	dcon::province_id prov;
	sys::date last_update;
	bool visible = false;
	
	element_base* capital_icon = nullptr;
	element_base* rails_icon = nullptr;
	element_base* rails_dots = nullptr;
	element_base* fort_icon = nullptr;
	element_base* fort_dots = nullptr;
	element_base* bank_icon = nullptr;
	element_base* unv_icon = nullptr;

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "capital_icon") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			capital_icon = ptr.get();
			return ptr;
		} else if(name == "rail_icon") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			rails_icon = ptr.get();
			return ptr;
		} else if(name == "rail_dots") {
			auto ptr = make_element_by_type<map_pv_rail_dots>(state, id);
			rails_dots = ptr.get();
			return ptr;
		} else if(name == "fort_icon") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			fort_icon = ptr.get();
			return ptr;
		} else if(name == "fort_dots") {
			auto ptr = make_element_by_type<map_pv_fort_dots>(state, id);
			fort_dots = ptr.get();
			return ptr;
		} else if(name == "bank_icon") {
			auto ptr = make_element_by_type<map_pv_bank>(state, id);
			bank_icon = ptr.get();
			return ptr;
		} else if(name == "university_icon") {
			auto ptr = make_element_by_type<map_pv_university>(state, id);
			unv_icon = ptr.get();
			return ptr;
		} else {
			return nullptr;
		}
	}

	void impl_on_update(sys::state& state) noexcept override {
		if(!visible)
			return;
		if(last_update && state.ui_date == last_update)
			return;

		last_update = state.ui_date;
		on_update(state);

		for(auto& c : children) {
			if(c->is_visible()) {
				c->impl_on_update(state);
			}
		}
	}

	void on_update(sys::state& state) noexcept override {
		int32_t rows = 0;
		if(state.world.nation_get_capital(state.world.province_get_nation_from_province_ownership(prov)) == prov) {
			capital_icon->set_visible(state, true);
			++rows;
		} else {
			capital_icon->set_visible(state, false);
		}
		if(state.world.province_get_building_level(prov, economy::province_building_type::railroad) != 0) {
			++rows;
			rails_icon->set_visible(state, true);
			rails_dots->set_visible(state, true);
		} else {
			rails_icon->set_visible(state, false);
			rails_dots->set_visible(state, false);
		}
		if(state.world.province_get_building_level(prov, economy::province_building_type::fort) != 0) {
			++rows;
			fort_icon->set_visible(state, true);
			fort_dots->set_visible(state, true);
		} else {
			fort_icon->set_visible(state, false);
			fort_dots->set_visible(state, false);
		}
		if((state.economy_definitions.building_definitions[uint32_t(economy::province_building_type::university)].defined && state.world.province_get_building_level(prov, economy::province_building_type::university) != 0)
			|| (state.economy_definitions.building_definitions[uint32_t(economy::province_building_type::bank)].defined && state.world.province_get_building_level(prov, economy::province_building_type::bank) != 0)) {
			++rows;
		} else {
			bank_icon->set_visible(state, false);
			unv_icon->set_visible(state, false);
		}

		auto top = (-16 * rows) / 2;
		if(state.world.nation_get_capital(state.world.province_get_nation_from_province_ownership(prov)) == prov) {
			capital_icon->base_data.position.y = int16_t(top - 2);
			capital_icon->base_data.position.x = int16_t(-10);
			top += 16;
		}
		if(state.world.province_get_building_level(prov, economy::province_building_type::railroad) != 0) {
			rails_icon->base_data.position.y = int16_t(top );
			rails_dots->base_data.position.y = int16_t(top);
			int32_t total_width = 18 + 2 + 3 + 4 * state.world.province_get_building_level(prov, economy::province_building_type::railroad);
			rails_icon->base_data.position.x = int16_t(-total_width / 2);
			rails_dots->base_data.position.x = int16_t(20 -total_width / 2);
			top += 16;
		}
		if(state.world.province_get_building_level(prov, economy::province_building_type::fort) != 0) {
			fort_icon->base_data.position.y = int16_t(top);
			fort_dots->base_data.position.y = int16_t(top);
			int32_t total_width = 18 + 2 + 3 + 4 * state.world.province_get_building_level(prov, economy::province_building_type::fort);
			fort_icon->base_data.position.x = int16_t(-total_width / 2);
			fort_dots->base_data.position.x = int16_t(20 - total_width / 2);
			top += 16;
		}
		if((state.economy_definitions.building_definitions[uint32_t(economy::province_building_type::university)].defined && state.world.province_get_building_level(prov, economy::province_building_type::university) != 0)
			|| (state.economy_definitions.building_definitions[uint32_t(economy::province_building_type::bank)].defined && state.world.province_get_building_level(prov, economy::province_building_type::bank) != 0)) {


			if((state.economy_definitions.building_definitions[uint32_t(economy::province_building_type::university)].defined && state.world.province_get_building_level(prov, economy::province_building_type::university) != 0)
			&& (state.economy_definitions.building_definitions[uint32_t(economy::province_building_type::bank)].defined && state.world.province_get_building_level(prov, economy::province_building_type::bank) != 0)) {

				unv_icon->base_data.position.y = int16_t(top);
				unv_icon->base_data.position.x = int16_t(0);
				bank_icon->base_data.position.y = int16_t(top);
				bank_icon->base_data.position.x = int16_t(-32);
				bank_icon->set_visible(state, true);
				unv_icon->set_visible(state, true);
			} else if(state.economy_definitions.building_definitions[uint32_t(economy::province_building_type::university)].defined && state.world.province_get_building_level(prov, economy::province_building_type::university) != 0) {

				unv_icon->base_data.position.y = int16_t(top);
				unv_icon->base_data.position.x = int16_t(-16);
				bank_icon->set_visible(state, false);
				unv_icon->set_visible(state, true);
			} else if(state.economy_definitions.building_definitions[uint32_t(economy::province_building_type::bank)].defined && state.world.province_get_building_level(prov, economy::province_building_type::bank) != 0) {

				bank_icon->base_data.position.y = int16_t(top);
				bank_icon->base_data.position.x = int16_t(-16);
				bank_icon->set_visible(state, true);
				unv_icon->set_visible(state, false);
			}
		}
	}

	void impl_render(sys::state& state, int32_t x, int32_t y) noexcept override {
		auto mid_point = state.world.province_get_mid_point(prov);
		auto map_pos = state.map_state.normalize_map_coord(mid_point);
		auto screen_size = glm::vec2{ float(state.x_size / state.user_settings.ui_scale), float(state.y_size / state.user_settings.ui_scale) };
		glm::vec2 screen_pos;

		if(!state.map_state.map_to_screen(state, map_pos, screen_size, screen_pos)) {
			visible = false;
			return;
		}

		if(screen_pos.x < -32 || screen_pos.y < -32 || screen_pos.x > state.ui_state.root->base_data.size.x + 32 || screen_pos.y > state.ui_state.root->base_data.size.y + 32) {
			visible = false;
			return;
		}
		if(visible == false) {
			visible = true;
			impl_on_update(state);
		}
			

		auto new_position = xy_pair{ int16_t(screen_pos.x), int16_t(screen_pos.y) };
		base_data.position = new_position;
		window_element_base::impl_render(state, new_position.x, new_position.y);
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::province_id>()) {
			payload.emplace<dcon::province_id>(prov);
			return message_result::consumed;
		}
		return message_result::unseen;
	}

	mouse_probe impl_probe_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override {
		return mouse_probe{ nullptr, ui::xy_pair{} };
	}
};

class rgo_icon : public image_element_base {
public:
	dcon::province_id content{};
	void impl_render(sys::state& state, int32_t x, int32_t y) noexcept override {
		auto mid_point = state.world.province_get_mid_point(content);
		auto map_pos = state.map_state.normalize_map_coord(mid_point);
		auto screen_size = glm::vec2{float(state.x_size / state.user_settings.ui_scale), float(state.y_size / state.user_settings.ui_scale)};
		glm::vec2 screen_pos;
		if(!state.map_state.map_to_screen(state, map_pos, screen_size, screen_pos))
			return;
		auto new_position = xy_pair{int16_t(screen_pos.x - base_data.size.x / 2), int16_t(screen_pos.y - base_data.size.y / 2)};
		image_element_base::base_data.position = new_position;
		image_element_base::impl_render(state, new_position.x, new_position.y);
	}
	void on_update(sys::state& state) noexcept override {
		auto cid = state.world.province_get_rgo(content).id;
		frame = int32_t(state.world.commodity_get_icon(cid));
	}
};
} // namespace ui
