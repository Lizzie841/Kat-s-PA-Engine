#include "system_state.hpp"
#include "nations.hpp"
#include "parsers_declarations.hpp"
#include "trigger_parsing.hpp"
#include "effect_parsing.hpp"

namespace parsers {
	void national_identity_file::any_value(std::string_view tag, association_type, std::string_view txt, error_handler& err,
		int32_t line, scenario_building_context& context) {
		if(tag.length() != 3) {
			err.accumulated_errors += err.file_name + " line " + std::to_string(line) + ": encountered a tag that was not three characters\n";
			return;
		}

		// Avoid issues that the original had, where defining a tag 'NOT' or 'AND' caused
		// crashes, here we tell the modder that she shouldn't do THAT
		if(is_fixed_token_ci(tag.data(), tag.data() + tag.length(), "not")
		|| is_fixed_token_ci(tag.data(), tag.data() + tag.length(), "and")) {
			err.accumulated_errors += err.file_name + " line " + std::to_string(line) + ": A tag which conflicts with a conditional 'NOT' or 'AND'\n";
			return;
		}

		if(is_fixed_token_ci(tag.data(), tag.data() + tag.length(), "war")
		|| is_fixed_token_ci(tag.data(), tag.data() + tag.length(), "tag")
		|| is_fixed_token_ci(tag.data(), tag.data() + tag.length(), "any")
		|| is_fixed_token_ci(tag.data(), tag.data() + tag.length(), "log")) {
			err.accumulated_errors += err.file_name + " line " + std::to_string(line) + ": A tag which conflicts with a built-in 'war', 'any', 'log' or 'tag'\n";
			return;
		}

		if(is_fixed_token_ci(tag.data(), tag.data() + tag.length(), "who")
		|| is_fixed_token_ci(tag.data(), tag.data() + tag.length(), "oob")
		|| is_fixed_token_ci(tag.data(), tag.data() + tag.length(), "yes")) {
			err.accumulated_warnings += err.file_name + " line " + std::to_string(line) + ": A tag which may conflict with a built-in 'who' or 'oob'\n";
		}

		auto as_int = nations::tag_to_int(tag[0], tag[1], tag[2]);
		auto new_ident = context.state.world.create_national_identity();
		if(is_fixed_token_ci(tag.data(), tag.data() + tag.length(), "reb")) {
			context.state.national_definitions.rebel_id = new_ident;
		}

		auto name_id = context.state.add_key_win1252(tag);
		auto adj_id = context.state.add_key_win1252(std::string(tag) + "_ADJ");
		context.state.world.national_identity_set_name(new_ident, name_id);
		context.state.world.national_identity_set_adjective(new_ident, adj_id);
		context.state.world.national_identity_set_identifying_int(new_ident, as_int);

		context.file_names_for_idents.resize(context.state.world.national_identity_size());
		context.file_names_for_idents[new_ident] = txt;
		context.map_of_ident_names.insert_or_assign(as_int, new_ident);
	}

	void triggered_modifier::finish(triggered_modifier_context& context) {
		auto name_id = text::find_or_add_key(context.outer_context.state, context.name, false);

		auto modifier_id = context.outer_context.state.world.create_modifier();

		context.outer_context.state.world.modifier_set_icon(modifier_id, uint8_t(icon_index));
		context.outer_context.state.world.modifier_set_name(modifier_id, name_id);
		context.outer_context.state.world.modifier_set_desc(modifier_id, text::find_or_add_key(context.outer_context.state, std::string(context.name) + "_desc", false));
		context.outer_context.state.world.modifier_set_national_values(modifier_id, force_national_mod());

		context.outer_context.map_of_modifiers.insert_or_assign(std::string(context.name), modifier_id);

		context.outer_context.state.national_definitions.triggered_modifiers[context.index].linked_modifier = modifier_id;
	}

	void register_trigger(token_generator& gen, error_handler& err, triggered_modifier_context& context) {
	context.outer_context.set_of_triggered_modifiers.push_back(pending_triggered_modifier_content{gen, context.index});
		gen.discard_group();
	}

	void make_triggered_modifier(std::string_view name, token_generator& gen, error_handler& err,
		scenario_building_context& context) {
		auto index = uint32_t(context.state.national_definitions.triggered_modifiers.size());
		context.state.national_definitions.triggered_modifiers.emplace_back();

	triggered_modifier_context new_context{context, index, name};
		parse_triggered_modifier(gen, err, new_context);
	}

	void make_national_value(std::string_view name, token_generator& gen, error_handler& err, scenario_building_context& context) {
		auto name_id = text::find_or_add_key(context.state, name, false);

		auto parsed_modifier = parse_modifier_base(gen, err, context);

		auto new_modifier = context.state.world.create_modifier();

		context.state.world.modifier_set_icon(new_modifier, uint8_t(context.number_of_national_values_seen));
		context.state.world.modifier_set_name(new_modifier, name_id);
		context.state.world.modifier_set_desc(new_modifier, text::find_or_add_key(context.state, std::string(name) + "_desc", false));
		context.state.world.modifier_set_national_values(new_modifier, parsed_modifier.force_national_mod());

		context.map_of_modifiers.insert_or_assign(std::string(name), new_modifier);

		++context.number_of_national_values_seen;
	}

	void m_very_easy_player(token_generator& gen, error_handler& err, scenario_building_context& context) {
		auto name_id = text::find_or_add_key(context.state, "very_easy_player", true);

		auto parsed_modifier = parse_modifier_base(gen, err, context);

		auto new_modifier = context.state.world.create_modifier();

		context.state.world.modifier_set_icon(new_modifier, uint8_t(parsed_modifier.icon_index));
		context.state.world.modifier_set_name(new_modifier, name_id);
		context.state.world.modifier_set_national_values(new_modifier, parsed_modifier.force_national_mod());

		context.map_of_modifiers.insert_or_assign(std::string("very_easy_player"), new_modifier);
		context.state.national_definitions.static_modifiers[uint8_t(nations::static_modifier::very_easy_player)] = new_modifier;
	}

	void m_easy_player(token_generator& gen, error_handler& err, scenario_building_context& context) {
		auto name_id = text::find_or_add_key(context.state, "easy_player", true);

		auto parsed_modifier = parse_modifier_base(gen, err, context);

		auto new_modifier = context.state.world.create_modifier();

		context.state.world.modifier_set_icon(new_modifier, uint8_t(parsed_modifier.icon_index));
		context.state.world.modifier_set_name(new_modifier, name_id);
		context.state.world.modifier_set_national_values(new_modifier, parsed_modifier.force_national_mod());

		context.map_of_modifiers.insert_or_assign(std::string("easy_player"), new_modifier);
		context.state.national_definitions.static_modifiers[uint8_t(nations::static_modifier::easy_player)] = new_modifier;
	}

	void m_hard_player(token_generator& gen, error_handler& err, scenario_building_context& context) {
		auto name_id = text::find_or_add_key(context.state, "hard_player", true);

		auto parsed_modifier = parse_modifier_base(gen, err, context);

		auto new_modifier = context.state.world.create_modifier();

		context.state.world.modifier_set_icon(new_modifier, uint8_t(parsed_modifier.icon_index));
		context.state.world.modifier_set_name(new_modifier, name_id);
		context.state.world.modifier_set_national_values(new_modifier, parsed_modifier.force_national_mod());

		context.map_of_modifiers.insert_or_assign(std::string("hard_player"), new_modifier);
		context.state.national_definitions.static_modifiers[uint8_t(nations::static_modifier::hard_player)] = new_modifier;
	}

	void m_very_hard_player(token_generator& gen, error_handler& err, scenario_building_context& context) {
		auto name_id = text::find_or_add_key(context.state, "very_hard_player", true);

		auto parsed_modifier = parse_modifier_base(gen, err, context);

		auto new_modifier = context.state.world.create_modifier();

		context.state.world.modifier_set_icon(new_modifier, uint8_t(parsed_modifier.icon_index));
		context.state.world.modifier_set_name(new_modifier, name_id);
		context.state.world.modifier_set_national_values(new_modifier, parsed_modifier.force_national_mod());

		context.map_of_modifiers.insert_or_assign(std::string("very_hard_player"), new_modifier);
		context.state.national_definitions.static_modifiers[uint8_t(nations::static_modifier::very_hard_player)] = new_modifier;
	}

	void m_very_easy_ai(token_generator& gen, error_handler& err, scenario_building_context& context) {
		auto name_id = text::find_or_add_key(context.state, "very_easy_ai", true);

		auto parsed_modifier = parse_modifier_base(gen, err, context);

		auto new_modifier = context.state.world.create_modifier();

		context.state.world.modifier_set_icon(new_modifier, uint8_t(parsed_modifier.icon_index));
		context.state.world.modifier_set_name(new_modifier, name_id);
		context.state.world.modifier_set_national_values(new_modifier, parsed_modifier.force_national_mod());

		context.map_of_modifiers.insert_or_assign(std::string("very_easy_ai"), new_modifier);
		context.state.national_definitions.static_modifiers[uint8_t(nations::static_modifier::very_easy_ai)] = new_modifier;
	}

	void m_easy_ai(token_generator& gen, error_handler& err, scenario_building_context& context) {
		auto name_id = text::find_or_add_key(context.state, "easy_ai", true);

		auto parsed_modifier = parse_modifier_base(gen, err, context);

		auto new_modifier = context.state.world.create_modifier();

		context.state.world.modifier_set_icon(new_modifier, uint8_t(parsed_modifier.icon_index));
		context.state.world.modifier_set_name(new_modifier, name_id);
		context.state.world.modifier_set_national_values(new_modifier, parsed_modifier.force_national_mod());

		context.map_of_modifiers.insert_or_assign(std::string("easy_ai"), new_modifier);
		context.state.national_definitions.static_modifiers[uint8_t(nations::static_modifier::easy_ai)] = new_modifier;
	}

	void m_hard_ai(token_generator& gen, error_handler& err, scenario_building_context& context) {
		auto name_id = text::find_or_add_key(context.state, "hard_ai", true);

		auto parsed_modifier = parse_modifier_base(gen, err, context);

		auto new_modifier = context.state.world.create_modifier();

		context.state.world.modifier_set_icon(new_modifier, uint8_t(parsed_modifier.icon_index));
		context.state.world.modifier_set_name(new_modifier, name_id);
		context.state.world.modifier_set_national_values(new_modifier, parsed_modifier.force_national_mod());

		context.map_of_modifiers.insert_or_assign(std::string("hard_ai"), new_modifier);
		context.state.national_definitions.static_modifiers[uint8_t(nations::static_modifier::hard_ai)] = new_modifier;
	}

	void m_very_hard_ai(token_generator& gen, error_handler& err, scenario_building_context& context) {
		auto name_id = text::find_or_add_key(context.state, "very_hard_ai", true);

		auto parsed_modifier = parse_modifier_base(gen, err, context);

		auto new_modifier = context.state.world.create_modifier();

		context.state.world.modifier_set_icon(new_modifier, uint8_t(parsed_modifier.icon_index));
		context.state.world.modifier_set_name(new_modifier, name_id);
		context.state.world.modifier_set_national_values(new_modifier, parsed_modifier.force_national_mod());

		context.map_of_modifiers.insert_or_assign(std::string("very_hard_ai"), new_modifier);
		context.state.national_definitions.static_modifiers[uint8_t(nations::static_modifier::very_hard_ai)] = new_modifier;
	}

	void m_overseas(token_generator& gen, error_handler& err, scenario_building_context& context) {
		auto name_id = text::find_or_add_key(context.state, "overseas", true);

		auto parsed_modifier = parse_modifier_base(gen, err, context);

		auto new_modifier = context.state.world.create_modifier();

		context.state.world.modifier_set_icon(new_modifier, uint8_t(parsed_modifier.icon_index));
		context.state.world.modifier_set_name(new_modifier, name_id);
		context.state.world.modifier_set_province_values(new_modifier, parsed_modifier.peek_province_mod());
		context.state.world.modifier_set_national_values(new_modifier, parsed_modifier.peek_national_mod());

		context.map_of_modifiers.insert_or_assign(std::string("overseas"), new_modifier);
		context.state.national_definitions.static_modifiers[uint8_t(nations::static_modifier::overseas)] = new_modifier;
	}

	void m_coastal(token_generator& gen, error_handler& err, scenario_building_context& context) {
		auto name_id = text::find_or_add_key(context.state, "coastal", true);

		auto parsed_modifier = parse_modifier_base(gen, err, context);

		auto new_modifier = context.state.world.create_modifier();

		context.state.world.modifier_set_icon(new_modifier, uint8_t(parsed_modifier.icon_index));
		context.state.world.modifier_set_name(new_modifier, name_id);
		context.state.world.modifier_set_province_values(new_modifier, parsed_modifier.peek_province_mod());
		context.state.world.modifier_set_national_values(new_modifier, parsed_modifier.peek_national_mod());

		context.map_of_modifiers.insert_or_assign(std::string("coastal"), new_modifier);
		context.state.national_definitions.static_modifiers[uint8_t(nations::static_modifier::coastal)] = new_modifier;
	}

	void m_non_coastal(token_generator& gen, error_handler& err, scenario_building_context& context) {
		auto name_id = text::find_or_add_key(context.state, "non_coastal", true);

		auto parsed_modifier = parse_modifier_base(gen, err, context);

		auto new_modifier = context.state.world.create_modifier();

		context.state.world.modifier_set_icon(new_modifier, uint8_t(parsed_modifier.icon_index));
		context.state.world.modifier_set_name(new_modifier, name_id);
		context.state.world.modifier_set_province_values(new_modifier, parsed_modifier.peek_province_mod());
		context.state.world.modifier_set_national_values(new_modifier, parsed_modifier.peek_national_mod());

		context.map_of_modifiers.insert_or_assign(std::string("non_coastal"), new_modifier);
		context.state.national_definitions.static_modifiers[uint8_t(nations::static_modifier::non_coastal)] = new_modifier;
	}

	void m_coastal_sea(token_generator& gen, error_handler& err, scenario_building_context& context) {
		auto name_id = text::find_or_add_key(context.state, "coastal_sea", true);

		auto parsed_modifier = parse_modifier_base(gen, err, context);

		auto new_modifier = context.state.world.create_modifier();

		context.state.world.modifier_set_icon(new_modifier, uint8_t(parsed_modifier.icon_index));
		context.state.world.modifier_set_name(new_modifier, name_id);
		context.state.world.modifier_set_province_values(new_modifier, parsed_modifier.peek_province_mod());
		context.state.world.modifier_set_national_values(new_modifier, parsed_modifier.peek_national_mod());

		context.map_of_modifiers.insert_or_assign(std::string("coastal_sea"), new_modifier);
		context.state.national_definitions.static_modifiers[uint8_t(nations::static_modifier::coastal_sea)] = new_modifier;
	}

	void m_sea_zone(token_generator& gen, error_handler& err, scenario_building_context& context) {
		auto name_id = text::find_or_add_key(context.state, "sea_zone", true);

		auto parsed_modifier = parse_modifier_base(gen, err, context);

		auto new_modifier = context.state.world.create_modifier();

		context.state.world.modifier_set_icon(new_modifier, uint8_t(parsed_modifier.icon_index));
		context.state.world.modifier_set_name(new_modifier, name_id);
		context.state.world.modifier_set_province_values(new_modifier, parsed_modifier.peek_province_mod());
		context.state.world.modifier_set_national_values(new_modifier, parsed_modifier.peek_national_mod());

		context.map_of_modifiers.insert_or_assign(std::string("sea_zone"), new_modifier);
		context.state.national_definitions.static_modifiers[uint8_t(nations::static_modifier::sea_zone)] = new_modifier;
	}

	void m_land_province(token_generator& gen, error_handler& err, scenario_building_context& context) {
		auto name_id = text::find_or_add_key(context.state, "land_province", true);

		auto parsed_modifier = parse_modifier_base(gen, err, context);

		auto new_modifier = context.state.world.create_modifier();

		context.state.world.modifier_set_icon(new_modifier, uint8_t(parsed_modifier.icon_index));
		context.state.world.modifier_set_name(new_modifier, name_id);
		context.state.world.modifier_set_province_values(new_modifier, parsed_modifier.peek_province_mod());
		context.state.world.modifier_set_national_values(new_modifier, parsed_modifier.peek_national_mod());

		context.map_of_modifiers.insert_or_assign(std::string("land_province"), new_modifier);
		context.state.national_definitions.static_modifiers[uint8_t(nations::static_modifier::land_province)] = new_modifier;
	}

	void m_blockaded(token_generator& gen, error_handler& err, scenario_building_context& context) {
		auto name_id = text::find_or_add_key(context.state, "blockaded", true);

		auto parsed_modifier = parse_modifier_base(gen, err, context);

		auto new_modifier = context.state.world.create_modifier();

		context.state.world.modifier_set_icon(new_modifier, uint8_t(parsed_modifier.icon_index));
		context.state.world.modifier_set_name(new_modifier, name_id);
		context.state.world.modifier_set_province_values(new_modifier, parsed_modifier.peek_province_mod());
		context.state.world.modifier_set_national_values(new_modifier, parsed_modifier.peek_national_mod());

		context.map_of_modifiers.insert_or_assign(std::string("blockaded"), new_modifier);
		context.state.national_definitions.static_modifiers[uint8_t(nations::static_modifier::blockaded)] = new_modifier;
	}

	void m_no_adjacent_controlled(token_generator& gen, error_handler& err, scenario_building_context& context) {
		auto name_id = text::find_or_add_key(context.state, "no_adjacent_controlled", true);

		auto parsed_modifier = parse_modifier_base(gen, err, context);

		auto new_modifier = context.state.world.create_modifier();

		context.state.world.modifier_set_icon(new_modifier, uint8_t(parsed_modifier.icon_index));
		context.state.world.modifier_set_name(new_modifier, name_id);
		context.state.world.modifier_set_province_values(new_modifier, parsed_modifier.peek_province_mod());
		context.state.world.modifier_set_national_values(new_modifier, parsed_modifier.peek_national_mod());

		context.map_of_modifiers.insert_or_assign(std::string("no_adjacent_controlled"), new_modifier);
		context.state.national_definitions.static_modifiers[uint8_t(nations::static_modifier::no_adjacent_controlled)] = new_modifier;
	}

	void m_core(token_generator& gen, error_handler& err, scenario_building_context& context) {
		auto name_id = text::find_or_add_key(context.state, "core", true);

		auto parsed_modifier = parse_modifier_base(gen, err, context);

		auto new_modifier = context.state.world.create_modifier();

		context.state.world.modifier_set_icon(new_modifier, uint8_t(parsed_modifier.icon_index));
		context.state.world.modifier_set_name(new_modifier, name_id);
		context.state.world.modifier_set_province_values(new_modifier, parsed_modifier.peek_province_mod());
		context.state.world.modifier_set_national_values(new_modifier, parsed_modifier.peek_national_mod());

		context.map_of_modifiers.insert_or_assign(std::string("core"), new_modifier);
		context.state.national_definitions.static_modifiers[uint8_t(nations::static_modifier::core)] = new_modifier;
	}

	void m_has_siege(token_generator& gen, error_handler& err, scenario_building_context& context) {
		auto name_id = text::find_or_add_key(context.state, "has_siege", true);

		auto parsed_modifier = parse_modifier_base(gen, err, context);

		auto new_modifier = context.state.world.create_modifier();

		context.state.world.modifier_set_icon(new_modifier, uint8_t(parsed_modifier.icon_index));
		context.state.world.modifier_set_name(new_modifier, name_id);
		context.state.world.modifier_set_province_values(new_modifier, parsed_modifier.peek_province_mod());
		context.state.world.modifier_set_national_values(new_modifier, parsed_modifier.peek_national_mod());

		context.map_of_modifiers.insert_or_assign(std::string("has_siege"), new_modifier);
		context.state.national_definitions.static_modifiers[uint8_t(nations::static_modifier::has_siege)] = new_modifier;
	}

	void m_occupied(token_generator& gen, error_handler& err, scenario_building_context& context) {
		auto name_id = text::find_or_add_key(context.state, "occupied", true);

		auto parsed_modifier = parse_modifier_base(gen, err, context);

		auto new_modifier = context.state.world.create_modifier();

		context.state.world.modifier_set_icon(new_modifier, uint8_t(parsed_modifier.icon_index));
		context.state.world.modifier_set_name(new_modifier, name_id);
		context.state.world.modifier_set_province_values(new_modifier, parsed_modifier.peek_province_mod());
		context.state.world.modifier_set_national_values(new_modifier, parsed_modifier.peek_national_mod());

		context.map_of_modifiers.insert_or_assign(std::string("occupied"), new_modifier);
		context.state.national_definitions.static_modifiers[uint8_t(nations::static_modifier::occupied)] = new_modifier;
	}

	void m_nationalism(token_generator& gen, error_handler& err, scenario_building_context& context) {
		auto name_id = text::find_or_add_key(context.state, "nationalism", true);

		auto parsed_modifier = parse_modifier_base(gen, err, context);

		auto new_modifier = context.state.world.create_modifier();

		context.state.world.modifier_set_icon(new_modifier, uint8_t(parsed_modifier.icon_index));
		context.state.world.modifier_set_name(new_modifier, name_id);
		context.state.world.modifier_set_province_values(new_modifier, parsed_modifier.peek_province_mod());
		context.state.world.modifier_set_national_values(new_modifier, parsed_modifier.peek_national_mod());

		context.map_of_modifiers.insert_or_assign(std::string("nationalism"), new_modifier);
		context.state.national_definitions.static_modifiers[uint8_t(nations::static_modifier::nationalism)] = new_modifier;
	}

	void m_infrastructure(token_generator& gen, error_handler& err, scenario_building_context& context) {
		auto name_id = text::find_or_add_key(context.state, "infrastructure", true);

		auto parsed_modifier = parse_modifier_base(gen, err, context);

		auto new_modifier = context.state.world.create_modifier();

		context.state.world.modifier_set_icon(new_modifier, uint8_t(parsed_modifier.icon_index));
		context.state.world.modifier_set_name(new_modifier, name_id);
		context.state.world.modifier_set_province_values(new_modifier, parsed_modifier.peek_province_mod());
		context.state.world.modifier_set_national_values(new_modifier, parsed_modifier.peek_national_mod());

		context.map_of_modifiers.insert_or_assign(std::string("infrastructure"), new_modifier);
		context.state.national_definitions.static_modifiers[uint8_t(nations::static_modifier::infrastructure)] = new_modifier;
	}

	void m_base_values(token_generator& gen, error_handler& err, scenario_building_context& context) {
		auto name_id = text::find_or_add_key(context.state, "base_values", true);

		auto parsed_modifier = parse_modifier_base(gen, err, context);

		auto new_modifier = context.state.world.create_modifier();

		context.state.world.modifier_set_icon(new_modifier, uint8_t(parsed_modifier.icon_index));
		context.state.world.modifier_set_name(new_modifier, name_id);
		context.state.world.modifier_set_national_values(new_modifier, parsed_modifier.force_national_mod());

		context.map_of_modifiers.insert_or_assign(std::string("base_values"), new_modifier);
		context.state.national_definitions.static_modifiers[uint8_t(nations::static_modifier::base_values)] = new_modifier;
	}

	void m_war(token_generator& gen, error_handler& err, scenario_building_context& context) {
		auto name_id = text::find_or_add_key(context.state, "war", true);

		auto parsed_modifier = parse_modifier_base(gen, err, context);

		auto new_modifier = context.state.world.create_modifier();

		context.state.world.modifier_set_icon(new_modifier, uint8_t(parsed_modifier.icon_index));
		context.state.world.modifier_set_name(new_modifier, name_id);
		context.state.world.modifier_set_national_values(new_modifier, parsed_modifier.force_national_mod());

		context.map_of_modifiers.insert_or_assign(std::string("war"), new_modifier);
		context.state.national_definitions.static_modifiers[uint8_t(nations::static_modifier::war)] = new_modifier;
	}

	void m_peace(token_generator& gen, error_handler& err, scenario_building_context& context) {
		auto name_id = text::find_or_add_key(context.state, "peace", true);

		auto parsed_modifier = parse_modifier_base(gen, err, context);

		auto new_modifier = context.state.world.create_modifier();

		context.state.world.modifier_set_icon(new_modifier, uint8_t(parsed_modifier.icon_index));
		context.state.world.modifier_set_name(new_modifier, name_id);
		context.state.world.modifier_set_national_values(new_modifier, parsed_modifier.force_national_mod());

		context.map_of_modifiers.insert_or_assign(std::string("peace"), new_modifier);
		context.state.national_definitions.static_modifiers[uint8_t(nations::static_modifier::peace)] = new_modifier;
	}

	void m_disarming(token_generator& gen, error_handler& err, scenario_building_context& context) {
		auto name_id = text::find_or_add_key(context.state, "disarming", true);

		auto parsed_modifier = parse_modifier_base(gen, err, context);

		auto new_modifier = context.state.world.create_modifier();

		context.state.world.modifier_set_icon(new_modifier, uint8_t(parsed_modifier.icon_index));
		context.state.world.modifier_set_name(new_modifier, name_id);
		context.state.world.modifier_set_national_values(new_modifier, parsed_modifier.force_national_mod());

		context.map_of_modifiers.insert_or_assign(std::string("disarming"), new_modifier);
		context.state.national_definitions.static_modifiers[uint8_t(nations::static_modifier::disarming)] = new_modifier;
	}

	void m_war_exhaustion(token_generator& gen, error_handler& err, scenario_building_context& context) {
		auto name_id = text::find_or_add_key(context.state, "war_exhaustion", true);

		auto parsed_modifier = parse_modifier_base(gen, err, context);

		auto new_modifier = context.state.world.create_modifier();

		context.state.world.modifier_set_icon(new_modifier, uint8_t(parsed_modifier.icon_index));
		context.state.world.modifier_set_name(new_modifier, name_id);
		context.state.world.modifier_set_national_values(new_modifier, parsed_modifier.force_national_mod());

		context.map_of_modifiers.insert_or_assign(std::string("war_exhaustion"), new_modifier);
		context.state.national_definitions.static_modifiers[uint8_t(nations::static_modifier::war_exhaustion)] = new_modifier;
	}

	void m_badboy(token_generator& gen, error_handler& err, scenario_building_context& context) {
		auto name_id = text::find_or_add_key(context.state, "badboy", true);

		auto parsed_modifier = parse_modifier_base(gen, err, context);

		auto new_modifier = context.state.world.create_modifier();

		context.state.world.modifier_set_icon(new_modifier, uint8_t(parsed_modifier.icon_index));
		context.state.world.modifier_set_name(new_modifier, name_id);
		context.state.world.modifier_set_national_values(new_modifier, parsed_modifier.force_national_mod());

		context.map_of_modifiers.insert_or_assign(std::string("badboy"), new_modifier);
		context.state.national_definitions.static_modifiers[uint8_t(nations::static_modifier::badboy)] = new_modifier;
	}

	void m_debt_default_to(token_generator& gen, error_handler& err, scenario_building_context& context) {
		auto name_id = text::find_or_add_key(context.state, "debt_default_to", true);

		auto parsed_modifier = parse_modifier_base(gen, err, context);

		auto new_modifier = context.state.world.create_modifier();

		context.state.world.modifier_set_icon(new_modifier, uint8_t(parsed_modifier.icon_index));
		context.state.world.modifier_set_name(new_modifier, name_id);
		context.state.world.modifier_set_national_values(new_modifier, parsed_modifier.force_national_mod());

		context.map_of_modifiers.insert_or_assign(std::string("debt_default_to"), new_modifier);
		context.state.national_definitions.static_modifiers[uint8_t(nations::static_modifier::debt_default_to)] = new_modifier;
	}

	void m_bad_debter(token_generator& gen, error_handler& err, scenario_building_context& context) {
		auto name_id = text::find_or_add_key(context.state, "bad_debter", true);

		auto parsed_modifier = parse_modifier_base(gen, err, context);

		auto new_modifier = context.state.world.create_modifier();

		context.state.world.modifier_set_icon(new_modifier, uint8_t(10));
		context.state.world.modifier_set_name(new_modifier, name_id);
		context.state.world.modifier_set_national_values(new_modifier, parsed_modifier.force_national_mod());

		context.map_of_modifiers.insert_or_assign(std::string("bad_debter"), new_modifier);
		context.state.national_definitions.static_modifiers[uint8_t(nations::static_modifier::bad_debter)] = new_modifier;
	}

	void m_great_power(token_generator& gen, error_handler& err, scenario_building_context& context) {
		auto name_id = text::find_or_add_key(context.state, "great_power", true);

		auto parsed_modifier = parse_modifier_base(gen, err, context);

		auto new_modifier = context.state.world.create_modifier();

		context.state.world.modifier_set_icon(new_modifier, uint8_t(parsed_modifier.icon_index));
		context.state.world.modifier_set_name(new_modifier, name_id);
		context.state.world.modifier_set_national_values(new_modifier, parsed_modifier.force_national_mod());

		context.map_of_modifiers.insert_or_assign(std::string("great_power"), new_modifier);
		context.state.national_definitions.static_modifiers[uint8_t(nations::static_modifier::great_power)] = new_modifier;
	}

	void m_second_power(token_generator& gen, error_handler& err, scenario_building_context& context) {
		auto name_id = text::find_or_add_key(context.state, "second_power", true);

		auto parsed_modifier = parse_modifier_base(gen, err, context);

		auto new_modifier = context.state.world.create_modifier();

		context.state.world.modifier_set_icon(new_modifier, uint8_t(parsed_modifier.icon_index));
		context.state.world.modifier_set_name(new_modifier, name_id);
		context.state.world.modifier_set_national_values(new_modifier, parsed_modifier.force_national_mod());

		context.map_of_modifiers.insert_or_assign(std::string("second_power"), new_modifier);
		context.state.national_definitions.static_modifiers[uint8_t(nations::static_modifier::second_power)] = new_modifier;
	}

	void m_civ_nation(token_generator& gen, error_handler& err, scenario_building_context& context) {
		auto name_id = text::find_or_add_key(context.state, "civ_nation", true);

		auto parsed_modifier = parse_modifier_base(gen, err, context);

		auto new_modifier = context.state.world.create_modifier();

		context.state.world.modifier_set_icon(new_modifier, uint8_t(parsed_modifier.icon_index));
		context.state.world.modifier_set_name(new_modifier, name_id);
		context.state.world.modifier_set_national_values(new_modifier, parsed_modifier.force_national_mod());

		context.map_of_modifiers.insert_or_assign(std::string("civ_nation"), new_modifier);
		context.state.national_definitions.static_modifiers[uint8_t(nations::static_modifier::civ_nation)] = new_modifier;
	}

	void m_unciv_nation(token_generator& gen, error_handler& err, scenario_building_context& context) {
		auto name_id = text::find_or_add_key(context.state, "unciv_nation", true);

		auto parsed_modifier = parse_modifier_base(gen, err, context);

		auto new_modifier = context.state.world.create_modifier();

		context.state.world.modifier_set_icon(new_modifier, uint8_t(parsed_modifier.icon_index));
		context.state.world.modifier_set_name(new_modifier, name_id);
		context.state.world.modifier_set_national_values(new_modifier, parsed_modifier.force_national_mod());

		context.map_of_modifiers.insert_or_assign(std::string("unciv_nation"), new_modifier);
		context.state.national_definitions.static_modifiers[uint8_t(nations::static_modifier::unciv_nation)] = new_modifier;
	}

	void m_average_literacy(token_generator& gen, error_handler& err, scenario_building_context& context) {
		auto name_id = text::find_or_add_key(context.state, "average_literacy", true);

		auto parsed_modifier = parse_modifier_base(gen, err, context);

		auto new_modifier = context.state.world.create_modifier();

		context.state.world.modifier_set_icon(new_modifier, uint8_t(parsed_modifier.icon_index));
		context.state.world.modifier_set_name(new_modifier, name_id);
		context.state.world.modifier_set_national_values(new_modifier, parsed_modifier.force_national_mod());

		context.map_of_modifiers.insert_or_assign(std::string("average_literacy"), new_modifier);
		context.state.national_definitions.static_modifiers[uint8_t(nations::static_modifier::average_literacy)] = new_modifier;
	}

	void m_plurality(token_generator& gen, error_handler& err, scenario_building_context& context) {
		auto name_id = text::find_or_add_key(context.state, "plurality", true);

		auto parsed_modifier = parse_modifier_base(gen, err, context);

		auto new_modifier = context.state.world.create_modifier();

		context.state.world.modifier_set_icon(new_modifier, uint8_t(parsed_modifier.icon_index));
		context.state.world.modifier_set_name(new_modifier, name_id);
		context.state.world.modifier_set_national_values(new_modifier, parsed_modifier.force_national_mod());

		context.map_of_modifiers.insert_or_assign(std::string("plurality"), new_modifier);
		context.state.national_definitions.static_modifiers[uint8_t(nations::static_modifier::plurality)] = new_modifier;
	}

	void m_generalised_debt_default(token_generator& gen, error_handler& err, scenario_building_context& context) {
		auto name_id = text::find_or_add_key(context.state, "generalised_debt_default", true);

		auto parsed_modifier = parse_modifier_base(gen, err, context);

		auto new_modifier = context.state.world.create_modifier();

		context.state.world.modifier_set_icon(new_modifier, uint8_t(10));
		context.state.world.modifier_set_name(new_modifier, name_id);
		context.state.world.modifier_set_national_values(new_modifier, parsed_modifier.force_national_mod());

		context.map_of_modifiers.insert_or_assign(std::string("generalised_debt_default"), new_modifier);
		context.state.national_definitions.static_modifiers[uint8_t(nations::static_modifier::generalised_debt_default)] = new_modifier;
	}

	void m_total_occupation(token_generator& gen, error_handler& err, scenario_building_context& context) {
		auto name_id = text::find_or_add_key(context.state, "total_occupation", true);

		auto parsed_modifier = parse_modifier_base(gen, err, context);

		auto new_modifier = context.state.world.create_modifier();

		context.state.world.modifier_set_icon(new_modifier, uint8_t(parsed_modifier.icon_index));
		context.state.world.modifier_set_name(new_modifier, name_id);
		context.state.world.modifier_set_national_values(new_modifier, parsed_modifier.force_national_mod());

		context.map_of_modifiers.insert_or_assign(std::string("total_occupation"), new_modifier);
		context.state.national_definitions.static_modifiers[uint8_t(nations::static_modifier::total_occupation)] = new_modifier;
	}

	void m_total_blockaded(token_generator& gen, error_handler& err, scenario_building_context& context) {
		auto name_id = text::find_or_add_key(context.state, "total_blockaded", true);

		auto parsed_modifier = parse_modifier_base(gen, err, context);

		auto new_modifier = context.state.world.create_modifier();

		context.state.world.modifier_set_icon(new_modifier, uint8_t(parsed_modifier.icon_index));
		context.state.world.modifier_set_name(new_modifier, name_id);
		context.state.world.modifier_set_national_values(new_modifier, parsed_modifier.force_national_mod());

		context.map_of_modifiers.insert_or_assign(std::string("total_blockaded"), new_modifier);
		context.state.national_definitions.static_modifiers[uint8_t(nations::static_modifier::total_blockaded)] = new_modifier;
	}

	void m_in_bankrupcy(token_generator& gen, error_handler& err, scenario_building_context& context) {
		auto name_id = text::find_or_add_key(context.state, "in_bankrupcy", true);

		auto parsed_modifier = parse_modifier_base(gen, err, context);

		auto new_modifier = context.state.world.create_modifier();

		context.state.world.modifier_set_icon(new_modifier, uint8_t(10));
		context.state.world.modifier_set_name(new_modifier, name_id);
		context.state.world.modifier_set_national_values(new_modifier, parsed_modifier.force_national_mod());

		context.map_of_modifiers.insert_or_assign(std::string("in_bankrupcy"), new_modifier);
		context.state.national_definitions.static_modifiers[uint8_t(nations::static_modifier::in_bankrupcy)] = new_modifier;
	}

	void make_event_modifier(std::string_view name, token_generator& gen, error_handler& err, scenario_building_context& context) {
		auto name_id = text::find_or_add_key(context.state, name, true);

		auto parsed_modifier = parse_modifier_base(gen, err, context);

		auto new_modifier = context.state.world.create_modifier();

		context.state.world.modifier_set_icon(new_modifier, uint8_t(parsed_modifier.icon_index));
		context.state.world.modifier_set_name(new_modifier, name_id);
		auto desc_1 = text::find_or_add_key(context.state, std::string(name) + "_desc", false);
	
		context.state.world.modifier_set_desc(new_modifier, desc_1);

		context.state.world.modifier_set_province_values(new_modifier, parsed_modifier.peek_province_mod());
		context.state.world.modifier_set_national_values(new_modifier, parsed_modifier.peek_national_mod());

		context.map_of_modifiers.insert_or_assign(std::string(name), new_modifier);
	}

	void make_party(token_generator& gen, error_handler& err, country_file_context& context) {
		auto party_id = context.outer_context.state.world.create_political_party();
		if(!(context.outer_context.state.world.national_identity_get_political_party_first(context.id))) {
			context.outer_context.state.world.national_identity_set_political_party_first(context.id, party_id);
			context.outer_context.state.world.national_identity_set_political_party_count(context.id, uint8_t(1));
		} else {
			context.outer_context.state.world.national_identity_get_political_party_count(context.id) += uint8_t(1);
		}

	party_context new_context{ context.outer_context, party_id };
		parse_party(gen, err, new_context);
	}

	void make_unit_names_list(std::string_view name, token_generator& gen, error_handler& err, country_file_context& context) {
		if(auto it = context.outer_context.map_of_unit_types.find(std::string(name));
			it != context.outer_context.map_of_unit_types.end()) {
			auto found_type = it->second;
		unit_names_context new_context{context.outer_context, context.id, found_type};
			parse_unit_names_list(gen, err, new_context);
		} else {
			err.accumulated_errors += "No unit type named " + std::string(name) + " (" + err.file_name + ")\n";
			gen.discard_group();
		}
	}

	dcon::national_variable_id scenario_building_context::get_national_variable(std::string const& name) {
		if(auto it = map_of_national_variables.find(name); it != map_of_national_variables.end()) {
			return it->second;
		} else {
			dcon::national_variable_id new_id = dcon::national_variable_id(
				dcon::national_variable_id::value_base_t(state.national_definitions.num_allocated_national_variables));
			++state.national_definitions.num_allocated_national_variables;
			map_of_national_variables.insert_or_assign(name, new_id);
			state.national_definitions.variable_names.safe_get(new_id) = text::find_or_add_key(state, name, false);
			return new_id;
		}
	}

	dcon::national_flag_id scenario_building_context::get_national_flag(std::string const& name) {
		if(auto it = map_of_national_flags.find(name); it != map_of_national_flags.end()) {
			return it->second;
		} else {
			dcon::national_flag_id new_id =
				dcon::national_flag_id(dcon::national_flag_id::value_base_t(state.national_definitions.num_allocated_national_flags));
			++state.national_definitions.num_allocated_national_flags;
			map_of_national_flags.insert_or_assign(name, new_id);
			state.national_definitions.flag_variable_names.safe_get(new_id) = text::find_or_add_key(state, name, false);
			return new_id;
		}
	}

	dcon::provincial_flag_id scenario_building_context::get_provincial_flag(std::string const& name) {
		if(auto it = map_of_provincial_flags.find(name); it != map_of_provincial_flags.end()) {
			return it->second;
		} else {
			dcon::provincial_flag_id new_id = dcon::provincial_flag_id(dcon::provincial_flag_id::value_base_t(state.province_definitions.num_allocated_provincial_flags));
			++state.province_definitions.num_allocated_provincial_flags;
			map_of_provincial_flags.insert_or_assign(name, new_id);
			state.province_definitions.flag_variable_names.safe_get(new_id) = text::find_or_add_key(state, name, false);
			return new_id;
		}
	}

	dcon::global_flag_id scenario_building_context::get_global_flag(std::string const& name) {
		if(auto it = map_of_global_flags.find(name); it != map_of_global_flags.end()) {
			return it->second;
		} else {
			dcon::global_flag_id new_id =
				dcon::global_flag_id(dcon::global_flag_id::value_base_t(state.national_definitions.num_allocated_global_flags));
			++state.national_definitions.num_allocated_global_flags;
			map_of_global_flags.insert_or_assign(name, new_id);
			state.national_definitions.global_flag_variable_names.safe_get(new_id) = text::find_or_add_key(state, name, false);
			return new_id;
		}
	}

	dcon::trigger_key read_triggered_modifier_condition(token_generator& gen, error_handler& err, scenario_building_context& context) {
		trigger_building_context t_context{context, trigger::slot_contents::nation, trigger::slot_contents::nation,
			trigger::slot_contents::empty};
		return make_trigger(gen, err, t_context);
	}

	dcon::trigger_key make_focus_limit(token_generator& gen, error_handler& err, national_focus_context& context) {
		trigger_building_context t_context{context.outer_context, trigger::slot_contents::province, trigger::slot_contents::nation,
			trigger::slot_contents::empty};
		return make_trigger(gen, err, t_context);
	}
	void make_focus(std::string_view name, token_generator& gen, error_handler& err, national_focus_context& context) {
		auto name_id = text::find_or_add_key(context.outer_context.state, name, false);
		auto new_focus = context.outer_context.state.world.create_national_focus();
		context.outer_context.state.world.national_focus_set_name(new_focus, name_id);
		context.outer_context.state.world.national_focus_set_type(new_focus, uint8_t(context.type));
		context.id = new_focus;
		auto modifier = parse_national_focus(gen, err, context);

		context.outer_context.state.world.national_focus_set_icon(new_focus, uint8_t(modifier.icon_index));

		if(modifier.next_to_add_n != 0 || modifier.next_to_add_p != 0) {
			auto new_modifier = context.outer_context.state.world.create_modifier();
			context.outer_context.state.world.modifier_set_name(new_modifier, name_id);
			context.outer_context.state.world.modifier_set_icon(new_modifier, uint8_t(modifier.icon_index));
			context.outer_context.state.world.modifier_set_province_values(new_modifier, modifier.peek_province_mod());
			context.outer_context.state.world.modifier_set_national_values(new_modifier, modifier.peek_national_mod());
			context.outer_context.state.world.national_focus_set_modifier(new_focus, new_modifier);
		}

		context.outer_context.map_of_national_focuses.insert_or_assign(std::string(name), new_focus);
	}
	void make_focus_group(std::string_view name, token_generator& gen, error_handler& err, scenario_building_context& context) {
		nations::focus_type t = nations::focus_type::unknown;

		if(is_fixed_token_ci(name.data(), name.data() + name.length(), "rail_focus"))
		t = nations::focus_type::rail_focus;
		else if(is_fixed_token_ci(name.data(), name.data() + name.length(), "immigration_focus"))
		t = nations::focus_type::immigration_focus;
		else if(is_fixed_token_ci(name.data(), name.data() + name.length(), "diplomatic_focus"))
		t = nations::focus_type::diplomatic_focus;
		else if(is_fixed_token_ci(name.data(), name.data() + name.length(), "promotion_focus"))
		t = nations::focus_type::promotion_focus;
		else if(is_fixed_token_ci(name.data(), name.data() + name.length(), "production_focus"))
		t = nations::focus_type::production_focus;
		else if(is_fixed_token_ci(name.data(), name.data() + name.length(), "party_loyalty_focus"))
		t = nations::focus_type::party_loyalty_focus;
		// non vanilla but present in some MP mods
		else if(is_fixed_token_ci(name.data(), name.data() + name.length(), "policy_focus"))
		t = nations::focus_type::policy_focus;
		else if(is_fixed_token_ci(name.data(), name.data() + name.length(), "tier_1_focus"))
		t = nations::focus_type::tier_1_focus;
		else if(is_fixed_token_ci(name.data(), name.data() + name.length(), "tier_2_focus"))
		t = nations::focus_type::tier_2_focus;
		else if(is_fixed_token_ci(name.data(), name.data() + name.length(), "tier_3_focus"))
		t = nations::focus_type::tier_3_focus;
		else if(is_fixed_token_ci(name.data(), name.data() + name.length(), "tier_4_focus"))
		t = nations::focus_type::tier_4_focus;
		else if(is_fixed_token_ci(name.data(), name.data() + name.length(), "tier_5_focus"))
		t = nations::focus_type::tier_5_focus;
		else if(is_fixed_token_ci(name.data(), name.data() + name.length(), "tier_6_focus"))
		t = nations::focus_type::tier_6_focus;
		else if(is_fixed_token_ci(name.data(), name.data() + name.length(), "tier_7_focus"))
		t = nations::focus_type::tier_7_focus;
		else if(is_fixed_token_ci(name.data(), name.data() + name.length(), "tier_8_focus"))
		t = nations::focus_type::tier_8_focus;
		else if(is_fixed_token_ci(name.data(), name.data() + name.length(), "building_focus"))
		t = nations::focus_type::building_focus;
		else if(is_fixed_token_ci(name.data(), name.data() + name.length(), "population_focus"))
		t = nations::focus_type::population_focus;
		else if(is_fixed_token_ci(name.data(), name.data() + name.length(), "heavy_industry_focus"))
		t = nations::focus_type::heavy_industry_focus;
		else if(is_fixed_token_ci(name.data(), name.data() + name.length(), "consumer_goods_focus"))
		t = nations::focus_type::consumer_goods_focus;
		else if(is_fixed_token_ci(name.data(), name.data() + name.length(), "military_goods_focus"))
		t = nations::focus_type::military_goods_focus;
		else if(is_fixed_token_ci(name.data(), name.data() + name.length(), "immigration_colonization_focus"))
		t = nations::focus_type::immigration_colonization_focus;
		else if(is_fixed_token_ci(name.data(), name.data() + name.length(), "small_promotion_focus"))
		t = nations::focus_type::small_promotion_focus;
		else if(is_fixed_token_ci(name.data(), name.data() + name.length(), "large_promotion_focus"))
		t = nations::focus_type::large_promotion_focus;
		else if(is_fixed_token_ci(name.data(), name.data() + name.length(), "massive_promotion_focus"))
		t = nations::focus_type::massive_promotion_focus;
		else
		err.accumulated_errors += "Unknown national focus group name " + std::string(name) + " (" + err.file_name + ")\n";

	national_focus_context new_context{context, dcon::national_focus_id{}, t};
		parse_focus_group(gen, err, new_context);
	}

	dcon::value_modifier_key make_decision_ai_choice(token_generator& gen, error_handler& err, decision_context& context) {
		trigger_building_context t_context{context.outer_context, trigger::slot_contents::nation, trigger::slot_contents::nation,
			trigger::slot_contents::empty};
		return make_value_modifier(gen, err, t_context);
	}
	dcon::trigger_key make_decision_trigger(token_generator& gen, error_handler& err, decision_context& context) {
		trigger_building_context t_context{context.outer_context, trigger::slot_contents::nation, trigger::slot_contents::nation,
			trigger::slot_contents::empty};
		return make_trigger(gen, err, t_context);
	}
	dcon::effect_key make_decision_effect(token_generator& gen, error_handler& err, decision_context& context) {
		effect_building_context e_context{context.outer_context, trigger::slot_contents::nation, trigger::slot_contents::nation,
			trigger::slot_contents::empty};
		e_context.effect_is_for_event = false;
		return make_effect(gen, err, e_context);
	}

	void make_decision(std::string_view name, token_generator& gen, error_handler& err, scenario_building_context& context) {
		auto new_decision = context.state.world.create_decision();

		auto name_id = text::find_or_add_key(context.state, std::string(name) + "_title", false);
		auto desc_id = text::find_or_add_key(context.state, std::string(name) + "_desc", false);

		auto root = get_root(context.state.common_fs);
		auto gfx = open_directory(root, NATIVE("gfx"));
		auto pictures = open_directory(gfx, NATIVE("pictures"));
		auto decisions = open_directory(pictures, NATIVE("decisions"));

		context.state.world.decision_set_name(new_decision, name_id);
		context.state.world.decision_set_description(new_decision, desc_id);

		decision_context new_context{context, new_decision};
		parse_decision(gen, err, new_context);

		if(!context.state.world.decision_get_image(new_decision)) {
			std::string file_name = simple_fs::remove_double_backslashes(std::string("gfx\\pictures\\decisions\\") + std::string(name) + ".tga");
			if(auto it = context.gfx_context.map_of_names.find(file_name); it != context.gfx_context.map_of_names.end()) {
				context.state.world.decision_set_image(new_decision, it->second);
			} else {
				auto gfxindex = context.state.ui_defs.gfx.size();
				context.state.ui_defs.gfx.emplace_back();
				ui::gfx_object& new_obj = context.state.ui_defs.gfx.back();
				auto new_id = dcon::gfx_object_id(uint16_t(gfxindex));

				context.gfx_context.map_of_names.insert_or_assign(file_name, new_id);

				new_obj.number_of_frames = uint8_t(1);

				if(auto itb = context.gfx_context.map_of_texture_names.find(file_name);
					itb != context.gfx_context.map_of_texture_names.end()) {
					new_obj.primary_texture_handle = itb->second;
				} else {
					auto index = context.state.ui_defs.textures.size();
					context.state.ui_defs.textures.emplace_back(context.state.add_key_win1252(file_name));
					new_obj.primary_texture_handle = dcon::texture_id(dcon::texture_id::value_base_t(index));
					context.gfx_context.map_of_texture_names.insert_or_assign(file_name, new_obj.primary_texture_handle);
				}
				new_obj.flags |= uint8_t(ui::object_type::generic_sprite);

				context.state.world.decision_set_image(new_decision, new_id);
			}
		}
	}

	dcon::trigger_key make_event_trigger(token_generator& gen, error_handler& err, event_building_context& context) {
	trigger_building_context t_context{context.outer_context, context.main_slot, context.this_slot, context.from_slot};
		return make_trigger(gen, err, t_context);
	}
	dcon::effect_key make_immediate_effect(token_generator& gen, error_handler& err, event_building_context& context) {
	effect_building_context e_context{context.outer_context, context.main_slot, context.this_slot, context.from_slot};
		e_context.effect_is_for_event = true;
		return make_effect(gen, err, e_context);
	}
	dcon::value_modifier_key make_event_mtth(token_generator& gen, error_handler& err, event_building_context& context) {
	trigger_building_context t_context{context.outer_context, context.main_slot, context.this_slot, context.from_slot};
		return make_value_modifier(gen, err, t_context);
	}
	dcon::value_modifier_key make_option_ai_chance(token_generator& gen, error_handler& err, effect_building_context& context) {
	trigger_building_context t_context{context.outer_context, context.main_slot, context.this_slot, context.from_slot};
		return make_value_modifier(gen, err, t_context);
	}
	sys::event_option make_event_option(token_generator& gen, error_handler& err, event_building_context& context) {
	effect_building_context e_context{context.outer_context, context.main_slot, context.this_slot, context.from_slot};
		e_context.effect_is_for_event = true;

		e_context.compiled_effect.push_back(uint16_t(effect::generic_scope | effect::scope_has_limit));
		e_context.compiled_effect.push_back(uint16_t(0));
		auto payload_size_offset = e_context.compiled_effect.size() - 1;
		e_context.limit_position = e_context.compiled_effect.size();
		e_context.compiled_effect.push_back(trigger::payload(dcon::trigger_key()).value);

		auto old_err_size = err.accumulated_errors.size();
		auto opt_result = parse_event_option(gen, err, e_context);

		e_context.compiled_effect[payload_size_offset] = uint16_t(e_context.compiled_effect.size() - payload_size_offset);

		if(e_context.compiled_effect.size() >= std::numeric_limits<uint16_t>::max()) {
			err.accumulated_errors += "effect " + text::produce_simple_string(context.outer_context.state, opt_result.name_) + " is " +
															std::to_string(e_context.compiled_effect.size()) +
															" cells big, which exceeds 64 KB bytecode limit (" + err.file_name + ")\n";
		return sys::event_option{opt_result.name_, opt_result.ai_chance, dcon::effect_key{0}};
		}

		if(err.accumulated_errors.size() == old_err_size) {
			auto const new_size = simplify_effect(e_context.compiled_effect.data());
			e_context.compiled_effect.resize(static_cast<size_t>(new_size));
		} else {
			e_context.compiled_effect.clear();
		}

		auto effect_id = context.outer_context.state.commit_effect_data(e_context.compiled_effect);
	return sys::event_option{opt_result.name_, opt_result.ai_chance, effect_id};
	}
	void commit_pending_events(error_handler& err, scenario_building_context& context) {
		int32_t count = 0;
		do {
			count = 0;
			auto fixed_size = context.map_of_national_events.size();
			for(auto& e : context.map_of_national_events) {
				if(!e.second.processed && e.second.text_assigned && !e.second.just_in_case_placeholder&& e.second.main_slot != trigger::slot_contents::empty) {
					e.second.processed = true;
					++count;

					if(!bool(e.second.id))
					e.second.id = context.state.world.create_national_event();

					auto data_copy = e.second;

					err.file_name = e.second.original_file + " [pending]";

					event_building_context e_context{context, data_copy.main_slot, data_copy.this_slot, data_copy.from_slot};
					auto event_result = parse_generic_event(data_copy.generator_state, err, e_context);

					auto fid = fatten(context.state.world, data_copy.id);
					fid.set_description(event_result.desc_);
					fid.set_name(event_result.title_);
					fid.set_image(event_result.picture_);
					fid.set_immediate_effect(event_result.immediate_);
					fid.set_news_title(event_result.news_title_);
					fid.set_news_picture(event_result.news_picture_);
					fid.set_news_long_desc(event_result.news_long_desc_);
					fid.set_news_medium_desc(event_result.news_medium_desc_);
					fid.set_news_short_desc(event_result.news_short_desc_);
					fid.set_is_major(event_result.major);
					fid.set_allow_multiple_instances(event_result.allow_multiple_instances);
					fid.get_options() = event_result.options;
					fid.set_legacy_id(uint32_t(event_result.id));
					fid.set_window_type(context.state.add_key_win1252(event_result.window_type));

					for(auto& r : context.state.national_definitions.on_yearly_pulse) {
						if(r.id == data_copy.id) {
							r.condition = event_result.trigger;
						}
					}
					for(auto& r : context.state.national_definitions.on_quarterly_pulse) {
						if(r.id == data_copy.id) {
							r.condition = event_result.trigger;
						}
					}
					for(auto& r : context.state.national_definitions.on_surrender) {
						if(r.id == data_copy.id) {
							r.condition = event_result.trigger;
						}
					}
					for(auto& r : context.state.national_definitions.on_new_great_nation) {
						if(r.id == data_copy.id) {
							r.condition = event_result.trigger;
						}
					}
					for(auto& r : context.state.national_definitions.on_lost_great_nation) {
						if(r.id == data_copy.id) {
							r.condition = event_result.trigger;
						}
					}
					for(auto& r : context.state.national_definitions.on_election_tick) {
						if(r.id == data_copy.id) {
							r.condition = event_result.trigger;
							r.issue_group = event_result.issue_group_;
						}
					}
					for(auto& r : context.state.national_definitions.on_colony_to_state) {
						if(r.id == data_copy.id) {
							r.condition = event_result.trigger;
						}
					}
					for(auto& r : context.state.national_definitions.on_state_conquest) {
						if(r.id == data_copy.id) {
							r.condition = event_result.trigger;
						}
					}
					for(auto& r : context.state.national_definitions.on_colony_to_state_free_slaves) {
						if(r.id == data_copy.id) {
							r.condition = event_result.trigger;
						}
					}
					for(auto& r : context.state.national_definitions.on_debtor_default) {
						if(r.id == data_copy.id) {
							r.condition = event_result.trigger;
						}
					}
					for(auto& r : context.state.national_definitions.on_debtor_default_small) {
						if(r.id == data_copy.id) {
							r.condition = event_result.trigger;
						}
					}
					for(auto& r : context.state.national_definitions.on_debtor_default_second) {
						if(r.id == data_copy.id) {
							r.condition = event_result.trigger;
						}
					}
					for(auto& r : context.state.national_definitions.on_civilize) {
						if(r.id == data_copy.id) {
							r.condition = event_result.trigger;
						}
					}
					for(auto& r : context.state.national_definitions.on_my_factories_nationalized) {
						if(r.id == data_copy.id) {
							r.condition = event_result.trigger;
						}
					}
					for(auto& r : context.state.national_definitions.on_crisis_declare_interest) {
						if(r.id == data_copy.id) {
							r.condition = event_result.trigger;
						}
					}
					for(auto& r : context.state.national_definitions.on_election_started) {
						if(r.id == data_copy.id) {
							r.condition = event_result.trigger;
						}
					}
					for(auto& r : context.state.national_definitions.on_election_finished) {
						if(r.id == data_copy.id) {
							r.condition = event_result.trigger;
						}
					}

					if(context.map_of_national_events.size() != fixed_size)
						break;
				}
			}

			fixed_size = context.map_of_provincial_events.size();
			for(auto& e : context.map_of_provincial_events) {
				if(!e.second.processed && e.second.text_assigned && !e.second.just_in_case_placeholder && e.second.main_slot != trigger::slot_contents::empty) {
					e.second.processed = true;
					++count;

					if(!bool(e.second.id))
					e.second.id = context.state.world.create_provincial_event();

					auto data_copy = e.second;

					err.file_name = e.second.original_file + " [pending]";

					event_building_context e_context{context, data_copy.main_slot, data_copy.this_slot, data_copy.from_slot};
					auto event_result = parse_generic_event(data_copy.generator_state, err, e_context);

					auto fid = fatten(context.state.world, data_copy.id);
					fid.set_description(event_result.desc_);
					fid.set_immediate_effect(event_result.immediate_);
					fid.set_news_title(event_result.news_title_);
					fid.set_news_picture(event_result.news_picture_);
					fid.set_news_long_desc(event_result.news_long_desc_);
					fid.set_news_medium_desc(event_result.news_medium_desc_);
					fid.set_news_short_desc(event_result.news_short_desc_);
					fid.set_allow_multiple_instances(event_result.allow_multiple_instances);
					fid.set_name(event_result.title_);
					fid.get_options() = event_result.options;
					fid.set_legacy_id(uint32_t(event_result.id));
					fid.set_window_type(context.state.add_key_win1252(event_result.window_type));

					for(auto& r : context.state.national_definitions.on_battle_won) {
						if(r.id == data_copy.id) {
							r.condition = event_result.trigger;
						}
					}
					for(auto& r : context.state.national_definitions.on_battle_lost) {
						if(r.id == data_copy.id) {
							r.condition = event_result.trigger;
						}
					}

					if(context.map_of_provincial_events.size() != fixed_size)
						break;
				}
			}
		} while(count > 0);

		for(auto& e : context.map_of_national_events) {
			if(!e.second.just_in_case_placeholder) {
				if(!e.second.text_assigned) {
					err.accumulated_warnings += "Event id: " + std::to_string(e.first) + " referenced but never defined. \n";
				} else if(!e.second.processed) {
					err.accumulated_warnings += "Event id: " + std::to_string(e.first) + " defined but never triggered. \n";
				}
			}
		}
		for(auto& e : context.map_of_provincial_events) {
			if(!e.second.just_in_case_placeholder) {
				if(!e.second.text_assigned) {
					err.accumulated_warnings += "Event id: " + std::to_string(e.first) + " referenced but never defined. \n";
				} else if(!e.second.processed) {
					err.accumulated_warnings += "Event id: " + std::to_string(e.first) + " defined but never triggered. \n";
				}
			}
		}
	}

	void make_oob_relationship(std::string_view tag, token_generator& gen, error_handler& err, oob_file_context& context) {
		if(tag.length() == 3) {
			if(auto it = context.outer_context.map_of_ident_names.find(nations::tag_to_int(tag[0], tag[1], tag[2]));
				it != context.outer_context.map_of_ident_names.end()) {
				auto holder = context.outer_context.state.world.national_identity_get_nation_from_identity_holder(it->second);
				if(holder) {
				oob_file_relation_context new_context{context.outer_context, context.nation_for, holder};
					parse_oob_relationship(gen, err, new_context);
				} else {
					gen.discard_group();
				}
			} else {
				err.accumulated_errors += "invalid tag " + std::string(tag) + " encountered  (" + err.file_name + ")\n";
				gen.discard_group();
			}
		} else {
			err.accumulated_errors += "invalid tag " + std::string(tag) + " encountered  (" + err.file_name + ")\n";
			gen.discard_group();
		}
	}

	void make_alliance(token_generator& gen, error_handler& err, scenario_building_context& context) {
		auto a = parse_alliance(gen, err, context);

		if(a.invalid || !a.first_ || !a.second_)
		return;

		auto rel = context.state.world.get_diplomatic_relation_by_diplomatic_pair(a.first_, a.second_);
		if(rel) {
			context.state.world.diplomatic_relation_set_are_allied(rel, true);
		} else {
			auto new_rel = context.state.world.force_create_diplomatic_relation(a.first_, a.second_);
			context.state.world.diplomatic_relation_set_are_allied(new_rel, true);
		}
	}
	void make_vassal(token_generator& gen, error_handler& err, scenario_building_context& context) {
		auto a = parse_vassal_description(gen, err, context);
		if(!a.invalid && a.second_ && a.first_) {
			context.state.world.force_create_overlord(a.second_, a.first_);
		}
	}
	void make_substate(token_generator& gen, error_handler& err, scenario_building_context& context) {
		auto a = parse_vassal_description(gen, err, context);
		if(!a.invalid && a.second_ && a.first_) {
			context.state.world.force_create_overlord(a.second_, a.first_);
			context.state.world.nation_set_is_substate(a.second_, true);
		}
	}

	void enter_country_file_dated_block(std::string_view label, token_generator& gen, error_handler& err, country_history_context& context) {
		auto ymd = parse_date(label, 0, err);
		if(sys::date(ymd, context.outer_context.state.start_date) <= context.outer_context.state.current_date) {
			context.in_dated_block = true;
			parse_country_history_file(gen, err, context);
			context.in_dated_block = false;
		} else {
			gen.discard_group();
		}
	}

} // namespace parsers
