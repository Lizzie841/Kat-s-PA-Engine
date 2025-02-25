#include "dcon_generated.hpp"
#include "system_state.hpp"
#include "serialization.hpp"
#include <ctime>

#define ZSTD_STATIC_LINKING_ONLY
#define XXH_NAMESPACE ZSTD_

#include "blake2.h"
#include "zstd.h"

namespace sys {
	template<typename T>
	inline size_t serialize_size(std::vector<T> const& vec) {
		return sizeof(uint32_t) + sizeof(T) * vec.size();
	}

	template<typename T>
	inline uint8_t* serialize(uint8_t* ptr_in, std::vector<T> const& vec) {
		uint32_t length = uint32_t(vec.size());
		std::memcpy(ptr_in, &length, sizeof(uint32_t));
		std::memcpy(ptr_in + sizeof(uint32_t), vec.data(), sizeof(T) * vec.size());
		return ptr_in + sizeof(uint32_t) + sizeof(T) * vec.size();
	}

	template<typename T>
	inline uint8_t const* deserialize(uint8_t const* ptr_in, std::vector<T>& vec) {
		uint32_t length = 0;
		std::memcpy(&length, ptr_in, sizeof(uint32_t));
		vec.resize(length);
		std::memcpy(vec.data(), ptr_in + sizeof(uint32_t), sizeof(T) * length);
		return ptr_in + sizeof(uint32_t) + sizeof(T) * length;
	}

	template<typename T>
	inline uint8_t* memcpy_serialize(uint8_t* ptr_in, T const& obj) {
		std::memcpy(ptr_in, &obj, sizeof(T));
		return ptr_in + sizeof(T);
	}

	template<typename T>
	inline uint8_t const* memcpy_deserialize(uint8_t const* ptr_in, T& obj) {
		std::memcpy(&obj, ptr_in, sizeof(T));
		return ptr_in + sizeof(T);
	}

	template<typename T, typename tag_type>
	inline size_t serialize_size(tagged_vector<T, tag_type> const& vec) {
		return sizeof(uint32_t) + sizeof(T) * vec.size();
	}

	template<typename T, typename tag_type>
	inline uint8_t* serialize(uint8_t* ptr_in, tagged_vector<T, tag_type> const& vec) {
		uint32_t length = uint32_t(vec.size());
		std::memcpy(ptr_in, &length, sizeof(uint32_t));
		std::memcpy(ptr_in + sizeof(uint32_t), vec.data(), sizeof(T) * vec.size());
		return ptr_in + sizeof(uint32_t) + sizeof(T) * vec.size();
	}

	template<typename T, typename tag_type>
	inline uint8_t const* deserialize(uint8_t const* ptr_in, tagged_vector<T, tag_type>& vec) {
		uint32_t length = 0;
		std::memcpy(&length, ptr_in, sizeof(uint32_t));
		vec.resize(length);
		std::memcpy(vec.data(), ptr_in + sizeof(uint32_t), sizeof(T) * length);
		return ptr_in + sizeof(uint32_t) + sizeof(T) * length;
	}

	inline size_t serialize_size(ankerl::unordered_dense::map<dcon::text_key, uint32_t, text::vector_backed_ci_hash, text::vector_backed_ci_eq> const& vec) {
		return serialize_size(vec.values());
	}

	inline size_t serialize_size(ankerl::unordered_dense::set<dcon::text_key, text::vector_backed_ci_hash, text::vector_backed_ci_eq> const& vec) {
		return serialize_size(vec.values());
	}

	inline uint8_t* serialize(uint8_t* ptr_in, ankerl::unordered_dense::map<dcon::text_key, uint32_t, text::vector_backed_ci_hash, text::vector_backed_ci_eq> const& vec) {
		return serialize(ptr_in, vec.values());
	}
	inline uint8_t* serialize(uint8_t* ptr_in, ankerl::unordered_dense::set<dcon::text_key, text::vector_backed_ci_hash, text::vector_backed_ci_eq> const& vec) {
		return serialize(ptr_in, vec.values());
	}
	inline uint8_t const* deserialize(uint8_t const* ptr_in, ankerl::unordered_dense::map<dcon::text_key, uint32_t, text::vector_backed_ci_hash, text::vector_backed_ci_eq>& vec) {
		uint32_t length = 0;
		std::memcpy(&length, ptr_in, sizeof(uint32_t));

		std::remove_cvref_t<decltype(vec.values())> new_vec;
		new_vec.resize(length);
		std::memcpy(new_vec.data(), ptr_in + sizeof(uint32_t), sizeof(vec.values()[0]) * length);
		vec.replace(std::move(new_vec));

		return ptr_in + sizeof(uint32_t) + sizeof(vec.values()[0]) * length;
	}
	inline uint8_t const* deserialize(uint8_t const* ptr_in, ankerl::unordered_dense::set<dcon::text_key, text::vector_backed_ci_hash, text::vector_backed_ci_eq>& vec) {
		uint32_t length = 0;
		std::memcpy(&length, ptr_in, sizeof(uint32_t));

		std::remove_cvref_t<decltype(vec.values())> new_vec;
		new_vec.resize(length);
		std::memcpy(new_vec.data(), ptr_in + sizeof(uint32_t), sizeof(vec.values()[0]) * length);
		vec.replace(std::move(new_vec));

		return ptr_in + sizeof(uint32_t) + sizeof(vec.values()[0]) * length;
	}

	inline size_t serialize_size(ankerl::unordered_dense::map<dcon::modifier_id, dcon::text_key, sys::modifier_hash> const& vec) {
		return serialize_size(vec.values());
	}
	inline uint8_t* serialize(uint8_t* ptr_in, ankerl::unordered_dense::map<dcon::modifier_id, dcon::text_key, sys::modifier_hash> const& vec) {
		return serialize(ptr_in, vec.values());
	}
	inline uint8_t const* deserialize(uint8_t const* ptr_in, ankerl::unordered_dense::map<dcon::modifier_id, dcon::text_key, sys::modifier_hash>& vec) {
		uint32_t length = 0;
		std::memcpy(&length, ptr_in, sizeof(uint32_t));

		std::remove_cvref_t<decltype(vec.values())> new_vec;
		new_vec.resize(length);
		std::memcpy(new_vec.data(), ptr_in + sizeof(uint32_t), sizeof(vec.values()[0]) * length);
		vec.replace(std::move(new_vec));

		return ptr_in + sizeof(uint32_t) + sizeof(vec.values()[0]) * length;
	}

	inline size_t serialize_size(ankerl::unordered_dense::map<uint16_t, dcon::text_key> const& vec) {
		return serialize_size(vec.values());
	}

	inline uint8_t* serialize(uint8_t* ptr_in, ankerl::unordered_dense::map<uint16_t, dcon::text_key> const& vec) {
		return serialize(ptr_in, vec.values());
	}
	inline uint8_t const* deserialize(uint8_t const* ptr_in, ankerl::unordered_dense::map<uint16_t, dcon::text_key>& vec) {
		uint32_t length = 0;
		std::memcpy(&length, ptr_in, sizeof(uint32_t));

		std::remove_cvref_t<decltype(vec.values())> new_vec;
		new_vec.resize(length);
		std::memcpy(new_vec.data(), ptr_in + sizeof(uint32_t), sizeof(vec.values()[0]) * length);
		vec.replace(std::move(new_vec));

		return ptr_in + sizeof(uint32_t) + sizeof(vec.values()[0]) * length;
	}

	uint8_t const* read_scenario_header(uint8_t const* ptr_in, scenario_header& header_out) {
		uint32_t length = 0;
		std::memcpy(&length, ptr_in, sizeof(uint32_t));
		std::memcpy(&header_out, ptr_in + sizeof(uint32_t), std::min(length, uint32_t(sizeof(scenario_header))));
		return ptr_in + sizeof(uint32_t) + length;
	}

	uint8_t const* read_save_header(uint8_t const* ptr_in, save_header& header_out) {
		uint32_t length = 0;
		std::memcpy(&length, ptr_in, sizeof(uint32_t));
		std::memcpy(&header_out, ptr_in + sizeof(uint32_t), std::min(length, uint32_t(sizeof(save_header))));
		return ptr_in + sizeof(uint32_t) + length;
	}

	uint8_t* write_scenario_header(uint8_t* ptr_in, scenario_header const& header_in) {
		uint32_t length = uint32_t(sizeof(scenario_header));
		std::memcpy(ptr_in, &length, sizeof(uint32_t));
		std::memcpy(ptr_in + sizeof(uint32_t), &header_in, sizeof(scenario_header));
		return ptr_in + sizeof_scenario_header(header_in);
	}
	uint8_t* write_save_header(uint8_t* ptr_in, save_header const& header_in) {
		uint32_t length = uint32_t(sizeof(save_header));
		std::memcpy(ptr_in, &length, sizeof(uint32_t));
		std::memcpy(ptr_in + sizeof(uint32_t), &header_in, sizeof(save_header));
		return ptr_in + sizeof_save_header(header_in);
	}

	size_t sizeof_scenario_header(scenario_header const& header_in) {
		return sizeof(uint32_t) + sizeof(scenario_header);
	}

	size_t sizeof_save_header(save_header const& header_in) {
		return sizeof(uint32_t) + sizeof(save_header);
	}

	void read_mod_path(uint8_t const* ptr_in, uint8_t const* lim, native_string& path_out) {
		uint32_t length = 0;
		if(size_t(lim - ptr_in) < sizeof(uint32_t))
		return;

		std::memcpy(&length, ptr_in, sizeof(uint32_t));
		ptr_in += sizeof(uint32_t);

		if(size_t(lim - ptr_in) < sizeof(uint32_t) + length * sizeof(native_char))
		return;

		path_out = native_string(native_string_view(reinterpret_cast<native_char const*>(ptr_in), length));
	}
	uint8_t const* load_mod_path(uint8_t const* ptr_in, sys::state& state) {
		uint32_t length = 0;
		std::memcpy(&length, ptr_in, sizeof(uint32_t));
		ptr_in += sizeof(uint32_t);

		simple_fs::restore_state(state.common_fs, native_string_view(reinterpret_cast<native_char const*>(ptr_in), length));
		return ptr_in + length * sizeof(native_char);
	}
	uint8_t* write_mod_path(uint8_t* ptr_in, native_string const& path_in) {
		uint32_t length = uint32_t(path_in.length());
		std::memcpy(ptr_in, &length, sizeof(uint32_t));
		ptr_in += sizeof(uint32_t);
		std::memcpy(ptr_in, path_in.c_str(), length * sizeof(native_char));
		ptr_in += length * sizeof(native_char);
		return ptr_in;
	}
	size_t sizeof_mod_path(native_string const& path_in) {
		size_t sz = 0;
		uint32_t length = uint32_t(path_in.length());
		sz += sizeof(uint32_t);
		sz += length * sizeof(native_char);
		return sz;
	}

	mod_identifier extract_mod_information(uint8_t const* ptr_in, uint64_t file_size) {
		scenario_header h;

		auto file_end = ptr_in + file_size;

		if(file_size > sizeof_scenario_header(h)) {
			ptr_in = read_scenario_header(ptr_in, h);
		}

		if(h.version != sys::scenario_file_version) {
		return mod_identifier{ native_string{}, 0, 0 };
		}

		native_string mod_path;
		read_mod_path(ptr_in, file_end, mod_path);

	return mod_identifier{ mod_path, h.timestamp, h.count };
	}

	uint8_t* write_compressed_section(uint8_t* ptr_out, uint8_t const* ptr_in, uint32_t uncompressed_size) {
		uint32_t decompressed_length = uncompressed_size;

		uint32_t section_length = uint32_t(ZSTD_compress(ptr_out + sizeof(uint32_t) * 2, ZSTD_compressBound(uncompressed_size), ptr_in,
		uncompressed_size, 0)); // write compressed data

		std::memcpy(ptr_out, &section_length, sizeof(uint32_t));
		std::memcpy(ptr_out + sizeof(uint32_t), &decompressed_length, sizeof(uint32_t));

		return ptr_out + sizeof(uint32_t) * 2 + section_length;
	}

	template<typename T>
	uint8_t const* with_decompressed_section(uint8_t const* ptr_in, T const& function) {
		uint32_t section_length = 0;
		uint32_t decompressed_length = 0;
		std::memcpy(&section_length, ptr_in, sizeof(uint32_t));
		std::memcpy(&decompressed_length, ptr_in + sizeof(uint32_t), sizeof(uint32_t));

		uint8_t* temp_buffer = new uint8_t[decompressed_length];
		// TODO: allocate memory for decompression and decompress into it

		ZSTD_decompress(temp_buffer, decompressed_length, ptr_in + sizeof(uint32_t) * 2, section_length);

		// function(ptr_in + sizeof(uint32_t) * 2, decompressed_length);
		function(temp_buffer, decompressed_length);

		delete[] temp_buffer;
		return ptr_in + sizeof(uint32_t) * 2 + section_length;
	}

	uint8_t const* read_scenario_section(uint8_t const* ptr_in, uint8_t const* section_end, sys::state& state) {
		// hand-written contribution
		{ // map
			ptr_in = memcpy_deserialize(ptr_in, state.map_state.map_data.size_x);
			ptr_in = memcpy_deserialize(ptr_in, state.map_state.map_data.size_y);
			ptr_in = deserialize(ptr_in, state.map_state.map_data.river_vertices);
			ptr_in = deserialize(ptr_in, state.map_state.map_data.river_starts);
			ptr_in = deserialize(ptr_in, state.map_state.map_data.river_counts);
			ptr_in = deserialize(ptr_in, state.map_state.map_data.coastal_vertices);
			ptr_in = deserialize(ptr_in, state.map_state.map_data.coastal_starts);
			ptr_in = deserialize(ptr_in, state.map_state.map_data.coastal_counts);
			ptr_in = deserialize(ptr_in, state.map_state.map_data.border_vertices);
			ptr_in = deserialize(ptr_in, state.map_state.map_data.borders);
			ptr_in = deserialize(ptr_in, state.map_state.map_data.terrain_id_map);
			ptr_in = deserialize(ptr_in, state.map_state.map_data.province_id_map);
			ptr_in = deserialize(ptr_in, state.map_state.map_data.diagonal_borders);
		}
		{
			std::memcpy(&(state.defines), ptr_in, sizeof(parsing::defines));
			ptr_in += sizeof(parsing::defines);
		}
		{
			std::memcpy(&(state.economy_definitions), ptr_in, sizeof(economy::global_economy_state));
			ptr_in += sizeof(economy::global_economy_state);
		}
		{ // culture definitions
			ptr_in = deserialize(ptr_in, state.culture_definitions.party_issues);
			ptr_in = deserialize(ptr_in, state.culture_definitions.political_issues);
			ptr_in = deserialize(ptr_in, state.culture_definitions.social_issues);
			ptr_in = deserialize(ptr_in, state.culture_definitions.military_issues);
			ptr_in = deserialize(ptr_in, state.culture_definitions.economic_issues);
			ptr_in = deserialize(ptr_in, state.culture_definitions.tech_folders);
			ptr_in = deserialize(ptr_in, state.culture_definitions.crimes);
			ptr_in = memcpy_deserialize(ptr_in, state.culture_definitions.artisans);
			ptr_in = memcpy_deserialize(ptr_in, state.culture_definitions.capitalists);
			ptr_in = memcpy_deserialize(ptr_in, state.culture_definitions.farmers);
			ptr_in = memcpy_deserialize(ptr_in, state.culture_definitions.laborers);
			ptr_in = memcpy_deserialize(ptr_in, state.culture_definitions.clergy);
			ptr_in = memcpy_deserialize(ptr_in, state.culture_definitions.soldiers);
			ptr_in = memcpy_deserialize(ptr_in, state.culture_definitions.officers);
			ptr_in = memcpy_deserialize(ptr_in, state.culture_definitions.slaves);
			ptr_in = memcpy_deserialize(ptr_in, state.culture_definitions.bureaucrat);
			ptr_in = memcpy_deserialize(ptr_in, state.culture_definitions.aristocrat);
			ptr_in = memcpy_deserialize(ptr_in, state.culture_definitions.primary_factory_worker);
			ptr_in = memcpy_deserialize(ptr_in, state.culture_definitions.secondary_factory_worker);
			ptr_in = memcpy_deserialize(ptr_in, state.culture_definitions.officer_leadership_points);
			ptr_in = memcpy_deserialize(ptr_in, state.culture_definitions.bureaucrat_tax_efficiency);
			ptr_in = memcpy_deserialize(ptr_in, state.culture_definitions.conservative);
			ptr_in = memcpy_deserialize(ptr_in, state.culture_definitions.jingoism);
			ptr_in = memcpy_deserialize(ptr_in, state.culture_definitions.promotion_chance);
			ptr_in = memcpy_deserialize(ptr_in, state.culture_definitions.demotion_chance);
			ptr_in = memcpy_deserialize(ptr_in, state.culture_definitions.migration_chance);
			ptr_in = memcpy_deserialize(ptr_in, state.culture_definitions.colonialmigration_chance);
			ptr_in = memcpy_deserialize(ptr_in, state.culture_definitions.emigration_chance);
			ptr_in = memcpy_deserialize(ptr_in, state.culture_definitions.assimilation_chance);
			ptr_in = memcpy_deserialize(ptr_in, state.culture_definitions.conversion_chance);
		}
		{ // military definitions
			ptr_in = memcpy_deserialize(ptr_in, state.military_definitions.first_background_trait);
			ptr_in = memcpy_deserialize(ptr_in, state.military_definitions.no_background);
			ptr_in = memcpy_deserialize(ptr_in, state.military_definitions.no_personality);
			ptr_in = deserialize(ptr_in, state.military_definitions.unit_base_definitions);
			ptr_in = memcpy_deserialize(ptr_in, state.military_definitions.base_army_unit);
			ptr_in = memcpy_deserialize(ptr_in, state.military_definitions.base_naval_unit);
			ptr_in = memcpy_deserialize(ptr_in, state.military_definitions.standard_civil_war);
			ptr_in = memcpy_deserialize(ptr_in, state.military_definitions.standard_great_war);
			ptr_in = memcpy_deserialize(ptr_in, state.military_definitions.standard_status_quo);
			ptr_in = memcpy_deserialize(ptr_in, state.military_definitions.liberate);
			ptr_in = memcpy_deserialize(ptr_in, state.military_definitions.uninstall_communist_gov);
			ptr_in = memcpy_deserialize(ptr_in, state.military_definitions.crisis_colony);
			ptr_in = memcpy_deserialize(ptr_in, state.military_definitions.crisis_liberate);
			ptr_in = memcpy_deserialize(ptr_in, state.military_definitions.irregular);
			ptr_in = memcpy_deserialize(ptr_in, state.military_definitions.infantry);
		}
		{ // national definitions
			ptr_in = deserialize(ptr_in, state.national_definitions.flag_variable_names);
			ptr_in = deserialize(ptr_in, state.national_definitions.global_flag_variable_names);
			ptr_in = deserialize(ptr_in, state.national_definitions.variable_names);
			ptr_in = deserialize(ptr_in, state.national_definitions.triggered_modifiers);
			ptr_in = memcpy_deserialize(ptr_in, state.national_definitions.rebel_id);
			ptr_in = memcpy_deserialize(ptr_in, state.national_definitions.static_game_rules);
			ptr_in = memcpy_deserialize(ptr_in, state.national_definitions.static_modifiers);
			ptr_in = memcpy_deserialize(ptr_in, state.national_definitions.num_allocated_national_variables);
			ptr_in = memcpy_deserialize(ptr_in, state.national_definitions.num_allocated_national_flags);
			ptr_in = memcpy_deserialize(ptr_in, state.national_definitions.num_allocated_global_flags);
			ptr_in = memcpy_deserialize(ptr_in, state.national_definitions.flashpoint_focus);
			ptr_in = deserialize(ptr_in, state.national_definitions.on_yearly_pulse);
			ptr_in = deserialize(ptr_in, state.national_definitions.on_quarterly_pulse);
			ptr_in = deserialize(ptr_in, state.national_definitions.on_battle_won);
			ptr_in = deserialize(ptr_in, state.national_definitions.on_battle_lost);
			ptr_in = deserialize(ptr_in, state.national_definitions.on_surrender);
			ptr_in = deserialize(ptr_in, state.national_definitions.on_new_great_nation);
			ptr_in = deserialize(ptr_in, state.national_definitions.on_lost_great_nation);
			ptr_in = deserialize(ptr_in, state.national_definitions.on_election_tick);
			ptr_in = deserialize(ptr_in, state.national_definitions.on_colony_to_state);
			ptr_in = deserialize(ptr_in, state.national_definitions.on_state_conquest);
			ptr_in = deserialize(ptr_in, state.national_definitions.on_colony_to_state_free_slaves);
			ptr_in = deserialize(ptr_in, state.national_definitions.on_debtor_default);
			ptr_in = deserialize(ptr_in, state.national_definitions.on_debtor_default_small);
			ptr_in = deserialize(ptr_in, state.national_definitions.on_debtor_default_second);
			ptr_in = deserialize(ptr_in, state.national_definitions.on_civilize);
			ptr_in = deserialize(ptr_in, state.national_definitions.on_my_factories_nationalized);
			ptr_in = deserialize(ptr_in, state.national_definitions.on_crisis_declare_interest);
			ptr_in = deserialize(ptr_in, state.national_definitions.on_election_started);
			ptr_in = deserialize(ptr_in, state.national_definitions.on_election_finished);
		}
		{ // provincial definitions
			ptr_in = deserialize(ptr_in, state.province_definitions.flag_variable_names);
			ptr_in = memcpy_deserialize(ptr_in, state.province_definitions.num_allocated_provincial_flags);
			ptr_in = deserialize(ptr_in, state.province_definitions.canals);
			ptr_in = deserialize(ptr_in, state.province_definitions.canal_provinces);
			ptr_in = deserialize(ptr_in, state.province_definitions.map_of_gfx_terrain_object_names);
			ptr_in = memcpy_deserialize(ptr_in, state.province_definitions.first_sea_province);
			ptr_in = memcpy_deserialize(ptr_in, state.province_definitions.europe);
			ptr_in = memcpy_deserialize(ptr_in, state.province_definitions.north_america);
			ptr_in = memcpy_deserialize(ptr_in, state.province_definitions.south_america);
		}
		ptr_in = memcpy_deserialize(ptr_in, state.start_date);
		ptr_in = memcpy_deserialize(ptr_in, state.end_date);
		ptr_in = deserialize(ptr_in, state.flag_type_names);
		ptr_in = deserialize(ptr_in, state.commodity_group_names);
		ptr_in = deserialize(ptr_in, state.trigger_data);
		ptr_in = deserialize(ptr_in, state.trigger_data_indices);
		ptr_in = deserialize(ptr_in, state.effect_data);
		ptr_in = deserialize(ptr_in, state.effect_data_indices);
		ptr_in = deserialize(ptr_in, state.value_modifier_segments);
		ptr_in = deserialize(ptr_in, state.value_modifiers);
		ptr_in = deserialize(ptr_in, state.key_data);
		ptr_in = deserialize(ptr_in, state.untrans_key_to_text_sequence);

		{ // ui definitions
			ptr_in = deserialize(ptr_in, state.ui_defs.gfx);
			ptr_in = deserialize(ptr_in, state.ui_defs.textures);
			ptr_in = deserialize(ptr_in, state.ui_defs.gui);
			ptr_in = deserialize(ptr_in, state.ui_defs.emfx);
			ptr_in = deserialize(ptr_in, state.font_collection.font_names);
			ptr_in = deserialize(ptr_in, state.ui_defs.extensions);
			ptr_in = deserialize(ptr_in, state.ui_defs.terrain_gfx);
		}

		// data container

		dcon::load_record loaded;
		std::byte const* start = reinterpret_cast<std::byte const*>(ptr_in);
		state.world.deserialize(start, reinterpret_cast<std::byte const*>(section_end), loaded);

		return section_end;
	}
	uint8_t* write_scenario_section(uint8_t* ptr_in, sys::state& state) {
		// hand-written contribution
		{ // map
			ptr_in = memcpy_serialize(ptr_in, state.map_state.map_data.size_x);
			ptr_in = memcpy_serialize(ptr_in, state.map_state.map_data.size_y);
			ptr_in = serialize(ptr_in, state.map_state.map_data.river_vertices);
			ptr_in = serialize(ptr_in, state.map_state.map_data.river_starts);
			ptr_in = serialize(ptr_in, state.map_state.map_data.river_counts);
			ptr_in = serialize(ptr_in, state.map_state.map_data.coastal_vertices);
			ptr_in = serialize(ptr_in, state.map_state.map_data.coastal_starts);
			ptr_in = serialize(ptr_in, state.map_state.map_data.coastal_counts);
			ptr_in = serialize(ptr_in, state.map_state.map_data.border_vertices);
			ptr_in = serialize(ptr_in, state.map_state.map_data.borders);
			ptr_in = serialize(ptr_in, state.map_state.map_data.terrain_id_map);
			ptr_in = serialize(ptr_in, state.map_state.map_data.province_id_map);
			ptr_in = serialize(ptr_in, state.map_state.map_data.diagonal_borders);
		}
		{
			std::memcpy(ptr_in, &(state.defines), sizeof(parsing::defines));
			ptr_in += sizeof(parsing::defines);
		}
		{
			std::memcpy(ptr_in, &(state.economy_definitions), sizeof(economy::global_economy_state));
			ptr_in += sizeof(economy::global_economy_state);
		}
		{ // culture definitions
			ptr_in = serialize(ptr_in, state.culture_definitions.party_issues);
			ptr_in = serialize(ptr_in, state.culture_definitions.political_issues);
			ptr_in = serialize(ptr_in, state.culture_definitions.social_issues);
			ptr_in = serialize(ptr_in, state.culture_definitions.military_issues);
			ptr_in = serialize(ptr_in, state.culture_definitions.economic_issues);
			ptr_in = serialize(ptr_in, state.culture_definitions.tech_folders);
			ptr_in = serialize(ptr_in, state.culture_definitions.crimes);
			ptr_in = memcpy_serialize(ptr_in, state.culture_definitions.artisans);
			ptr_in = memcpy_serialize(ptr_in, state.culture_definitions.capitalists);
			ptr_in = memcpy_serialize(ptr_in, state.culture_definitions.farmers);
			ptr_in = memcpy_serialize(ptr_in, state.culture_definitions.laborers);
			ptr_in = memcpy_serialize(ptr_in, state.culture_definitions.clergy);
			ptr_in = memcpy_serialize(ptr_in, state.culture_definitions.soldiers);
			ptr_in = memcpy_serialize(ptr_in, state.culture_definitions.officers);
			ptr_in = memcpy_serialize(ptr_in, state.culture_definitions.slaves);
			ptr_in = memcpy_serialize(ptr_in, state.culture_definitions.bureaucrat);
			ptr_in = memcpy_serialize(ptr_in, state.culture_definitions.aristocrat);
			ptr_in = memcpy_serialize(ptr_in, state.culture_definitions.primary_factory_worker);
			ptr_in = memcpy_serialize(ptr_in, state.culture_definitions.secondary_factory_worker);
			ptr_in = memcpy_serialize(ptr_in, state.culture_definitions.officer_leadership_points);
			ptr_in = memcpy_serialize(ptr_in, state.culture_definitions.bureaucrat_tax_efficiency);
			ptr_in = memcpy_serialize(ptr_in, state.culture_definitions.conservative);
			ptr_in = memcpy_serialize(ptr_in, state.culture_definitions.jingoism);
			ptr_in = memcpy_serialize(ptr_in, state.culture_definitions.promotion_chance);
			ptr_in = memcpy_serialize(ptr_in, state.culture_definitions.demotion_chance);
			ptr_in = memcpy_serialize(ptr_in, state.culture_definitions.migration_chance);
			ptr_in = memcpy_serialize(ptr_in, state.culture_definitions.colonialmigration_chance);
			ptr_in = memcpy_serialize(ptr_in, state.culture_definitions.emigration_chance);
			ptr_in = memcpy_serialize(ptr_in, state.culture_definitions.assimilation_chance);
			ptr_in = memcpy_serialize(ptr_in, state.culture_definitions.conversion_chance);
		}
		{ // military definitions
			ptr_in = memcpy_serialize(ptr_in, state.military_definitions.first_background_trait);
			ptr_in = memcpy_serialize(ptr_in, state.military_definitions.no_background);
			ptr_in = memcpy_serialize(ptr_in, state.military_definitions.no_personality);
			ptr_in = serialize(ptr_in, state.military_definitions.unit_base_definitions);
			ptr_in = memcpy_serialize(ptr_in, state.military_definitions.base_army_unit);
			ptr_in = memcpy_serialize(ptr_in, state.military_definitions.base_naval_unit);
			ptr_in = memcpy_serialize(ptr_in, state.military_definitions.standard_civil_war);
			ptr_in = memcpy_serialize(ptr_in, state.military_definitions.standard_great_war);
			ptr_in = memcpy_serialize(ptr_in, state.military_definitions.standard_status_quo);
			ptr_in = memcpy_serialize(ptr_in, state.military_definitions.liberate);
			ptr_in = memcpy_serialize(ptr_in, state.military_definitions.uninstall_communist_gov);
			ptr_in = memcpy_serialize(ptr_in, state.military_definitions.crisis_colony);
			ptr_in = memcpy_serialize(ptr_in, state.military_definitions.crisis_liberate);
			ptr_in = memcpy_serialize(ptr_in, state.military_definitions.irregular);
			ptr_in = memcpy_serialize(ptr_in, state.military_definitions.infantry);
		}
		{ // national definitions
			ptr_in = serialize(ptr_in, state.national_definitions.flag_variable_names);
			ptr_in = serialize(ptr_in, state.national_definitions.global_flag_variable_names);
			ptr_in = serialize(ptr_in, state.national_definitions.variable_names);
			ptr_in = serialize(ptr_in, state.national_definitions.triggered_modifiers);
			ptr_in = memcpy_serialize(ptr_in, state.national_definitions.rebel_id);
			ptr_in = memcpy_serialize(ptr_in, state.national_definitions.static_game_rules);
			ptr_in = memcpy_serialize(ptr_in, state.national_definitions.static_modifiers);
			ptr_in = memcpy_serialize(ptr_in, state.national_definitions.num_allocated_national_variables);
			ptr_in = memcpy_serialize(ptr_in, state.national_definitions.num_allocated_national_flags);
			ptr_in = memcpy_serialize(ptr_in, state.national_definitions.num_allocated_global_flags);
			ptr_in = memcpy_serialize(ptr_in, state.national_definitions.flashpoint_focus);
			ptr_in = serialize(ptr_in, state.national_definitions.on_yearly_pulse);
			ptr_in = serialize(ptr_in, state.national_definitions.on_quarterly_pulse);
			ptr_in = serialize(ptr_in, state.national_definitions.on_battle_won);
			ptr_in = serialize(ptr_in, state.national_definitions.on_battle_lost);
			ptr_in = serialize(ptr_in, state.national_definitions.on_surrender);
			ptr_in = serialize(ptr_in, state.national_definitions.on_new_great_nation);
			ptr_in = serialize(ptr_in, state.national_definitions.on_lost_great_nation);
			ptr_in = serialize(ptr_in, state.national_definitions.on_election_tick);
			ptr_in = serialize(ptr_in, state.national_definitions.on_colony_to_state);
			ptr_in = serialize(ptr_in, state.national_definitions.on_state_conquest);
			ptr_in = serialize(ptr_in, state.national_definitions.on_colony_to_state_free_slaves);
			ptr_in = serialize(ptr_in, state.national_definitions.on_debtor_default);
			ptr_in = serialize(ptr_in, state.national_definitions.on_debtor_default_small);
			ptr_in = serialize(ptr_in, state.national_definitions.on_debtor_default_second);
			ptr_in = serialize(ptr_in, state.national_definitions.on_civilize);
			ptr_in = serialize(ptr_in, state.national_definitions.on_my_factories_nationalized);
			ptr_in = serialize(ptr_in, state.national_definitions.on_crisis_declare_interest);
			ptr_in = serialize(ptr_in, state.national_definitions.on_election_started);
			ptr_in = serialize(ptr_in, state.national_definitions.on_election_finished);
		}
		{ // provincial definitions
			ptr_in = serialize(ptr_in, state.province_definitions.flag_variable_names);
			ptr_in = memcpy_serialize(ptr_in, state.province_definitions.num_allocated_provincial_flags);
			ptr_in = serialize(ptr_in, state.province_definitions.canals);
			ptr_in = serialize(ptr_in, state.province_definitions.canal_provinces);
			ptr_in = serialize(ptr_in, state.province_definitions.map_of_gfx_terrain_object_names);
			ptr_in = memcpy_serialize(ptr_in, state.province_definitions.first_sea_province);
			ptr_in = memcpy_serialize(ptr_in, state.province_definitions.europe);
			ptr_in = memcpy_serialize(ptr_in, state.province_definitions.north_america);
			ptr_in = memcpy_serialize(ptr_in, state.province_definitions.south_america);
		}
		ptr_in = memcpy_serialize(ptr_in, state.start_date);
		ptr_in = memcpy_serialize(ptr_in, state.end_date);
		ptr_in = serialize(ptr_in, state.flag_type_names);
		ptr_in = serialize(ptr_in, state.commodity_group_names);
		ptr_in = serialize(ptr_in, state.trigger_data);
		ptr_in = serialize(ptr_in, state.trigger_data_indices);
		ptr_in = serialize(ptr_in, state.effect_data);
		ptr_in = serialize(ptr_in, state.effect_data_indices);
		ptr_in = serialize(ptr_in, state.value_modifier_segments);
		ptr_in = serialize(ptr_in, state.value_modifiers);
		ptr_in = serialize(ptr_in, state.key_data);
		ptr_in = serialize(ptr_in, state.untrans_key_to_text_sequence);

		{ // ui definitions
			ptr_in = serialize(ptr_in, state.ui_defs.gfx);
			ptr_in = serialize(ptr_in, state.ui_defs.textures);
			ptr_in = serialize(ptr_in, state.ui_defs.gui);
			ptr_in = serialize(ptr_in, state.ui_defs.emfx);
			ptr_in = serialize(ptr_in, state.font_collection.font_names);
			ptr_in = serialize(ptr_in, state.ui_defs.extensions);
			ptr_in = serialize(ptr_in, state.ui_defs.terrain_gfx);
		}

		dcon::load_record result = state.world.make_serialize_record_store_scenario();
		std::byte* start = reinterpret_cast<std::byte*>(ptr_in);
		state.world.serialize(start, result);

		return reinterpret_cast<uint8_t*>(start);
	}
	scenario_size sizeof_scenario_section(sys::state& state) {
		size_t sz = 0;

		// hand-written contribution
		{ // map
			sz += sizeof(state.map_state.map_data.size_x);
			sz += sizeof(state.map_state.map_data.size_y);
			sz += serialize_size(state.map_state.map_data.river_vertices);
			sz += serialize_size(state.map_state.map_data.river_starts);
			sz += serialize_size(state.map_state.map_data.river_counts);
			sz += serialize_size(state.map_state.map_data.coastal_vertices);
			sz += serialize_size(state.map_state.map_data.coastal_starts);
			sz += serialize_size(state.map_state.map_data.coastal_counts);
			sz += serialize_size(state.map_state.map_data.border_vertices);
			sz += serialize_size(state.map_state.map_data.borders);
			sz += serialize_size(state.map_state.map_data.terrain_id_map);
			sz += serialize_size(state.map_state.map_data.province_id_map);
			sz += serialize_size(state.map_state.map_data.diagonal_borders);
		}
	{ sz += sizeof(parsing::defines); }
	{ sz += sizeof(economy::global_economy_state); }
		{ // culture definitions
			sz += serialize_size(state.culture_definitions.party_issues);
			sz += serialize_size(state.culture_definitions.political_issues);
			sz += serialize_size(state.culture_definitions.social_issues);
			sz += serialize_size(state.culture_definitions.military_issues);
			sz += serialize_size(state.culture_definitions.economic_issues);
			sz += serialize_size(state.culture_definitions.tech_folders);
			sz += serialize_size(state.culture_definitions.crimes);
			sz += sizeof(state.culture_definitions.artisans);
			sz += sizeof(state.culture_definitions.capitalists);
			sz += sizeof(state.culture_definitions.farmers);
			sz += sizeof(state.culture_definitions.laborers);
			sz += sizeof(state.culture_definitions.clergy);
			sz += sizeof(state.culture_definitions.soldiers);
			sz += sizeof(state.culture_definitions.officers);
			sz += sizeof(state.culture_definitions.slaves);
			sz += sizeof(state.culture_definitions.bureaucrat);
			sz += sizeof(state.culture_definitions.aristocrat);
			sz += sizeof(state.culture_definitions.primary_factory_worker);
			sz += sizeof(state.culture_definitions.secondary_factory_worker);
			sz += sizeof(state.culture_definitions.officer_leadership_points);
			sz += sizeof(state.culture_definitions.bureaucrat_tax_efficiency);
			sz += sizeof(state.culture_definitions.conservative);
			sz += sizeof(state.culture_definitions.jingoism);
			sz += sizeof(state.culture_definitions.promotion_chance);
			sz += sizeof(state.culture_definitions.demotion_chance);
			sz += sizeof(state.culture_definitions.migration_chance);
			sz += sizeof(state.culture_definitions.colonialmigration_chance);
			sz += sizeof(state.culture_definitions.emigration_chance);
			sz += sizeof(state.culture_definitions.assimilation_chance);
			sz += sizeof(state.culture_definitions.conversion_chance);
		}
		{ // military definitions
			sz += sizeof(state.military_definitions.first_background_trait);
			sz += sizeof(state.military_definitions.no_background);
			sz += sizeof(state.military_definitions.no_personality);
			sz += serialize_size(state.military_definitions.unit_base_definitions);
			sz += sizeof(state.military_definitions.base_army_unit);
			sz += sizeof(state.military_definitions.base_naval_unit);
			sz += sizeof(state.military_definitions.standard_civil_war);
			sz += sizeof(state.military_definitions.standard_great_war);
			sz += sizeof(state.military_definitions.standard_status_quo);
			sz += sizeof(state.military_definitions.liberate);
			sz += sizeof(state.military_definitions.uninstall_communist_gov);
			sz += sizeof(state.military_definitions.crisis_colony);
			sz += sizeof(state.military_definitions.crisis_liberate);
			sz += sizeof(state.military_definitions.irregular);
			sz += sizeof(state.military_definitions.infantry);
		}
		{ // national definitions
			sz += serialize_size(state.national_definitions.flag_variable_names);
			sz += serialize_size(state.national_definitions.global_flag_variable_names);
			sz += serialize_size(state.national_definitions.variable_names);
			sz += serialize_size(state.national_definitions.triggered_modifiers);
			sz += sizeof(state.national_definitions.rebel_id);
			sz += sizeof(state.national_definitions.static_game_rules);
			sz += sizeof(state.national_definitions.static_modifiers);
			sz += sizeof(state.national_definitions.num_allocated_national_variables);
			sz += sizeof(state.national_definitions.num_allocated_national_flags);
			sz += sizeof(state.national_definitions.num_allocated_global_flags);
			sz += sizeof(state.national_definitions.flashpoint_focus);
			sz += serialize_size(state.national_definitions.on_yearly_pulse);
			sz += serialize_size(state.national_definitions.on_quarterly_pulse);
			sz += serialize_size(state.national_definitions.on_battle_won);
			sz += serialize_size(state.national_definitions.on_battle_lost);
			sz += serialize_size(state.national_definitions.on_surrender);
			sz += serialize_size(state.national_definitions.on_new_great_nation);
			sz += serialize_size(state.national_definitions.on_lost_great_nation);
			sz += serialize_size(state.national_definitions.on_election_tick);
			sz += serialize_size(state.national_definitions.on_colony_to_state);
			sz += serialize_size(state.national_definitions.on_state_conquest);
			sz += serialize_size(state.national_definitions.on_colony_to_state_free_slaves);
			sz += serialize_size(state.national_definitions.on_debtor_default);
			sz += serialize_size(state.national_definitions.on_debtor_default_small);
			sz += serialize_size(state.national_definitions.on_debtor_default_second);
			sz += serialize_size(state.national_definitions.on_civilize);
			sz += serialize_size(state.national_definitions.on_my_factories_nationalized);
			sz += serialize_size(state.national_definitions.on_crisis_declare_interest);
			sz += serialize_size(state.national_definitions.on_election_started);
			sz += serialize_size(state.national_definitions.on_election_finished);
		}
		{ // provincial definitions
			sz += serialize_size(state.province_definitions.flag_variable_names);
			sz += sizeof(state.province_definitions.num_allocated_provincial_flags);
			sz += serialize_size(state.province_definitions.canals);
			sz += serialize_size(state.province_definitions.canal_provinces);
			sz += serialize_size(state.province_definitions.map_of_gfx_terrain_object_names);
			sz += sizeof(state.province_definitions.first_sea_province);
			sz += sizeof(state.province_definitions.europe);
			sz += sizeof(state.province_definitions.north_america);
			sz += sizeof(state.province_definitions.south_america);
		}
		sz += sizeof(state.start_date);
		sz += sizeof(state.end_date);
		sz += serialize_size(state.flag_type_names);
		sz += serialize_size(state.commodity_group_names);
		sz += serialize_size(state.trigger_data);
		sz += serialize_size(state.trigger_data_indices);
		sz += serialize_size(state.effect_data);
		sz += serialize_size(state.effect_data_indices);
		sz += serialize_size(state.value_modifier_segments);
		sz += serialize_size(state.value_modifiers);
		sz += serialize_size(state.key_data);
		sz += serialize_size(state.untrans_key_to_text_sequence);

		{ // ui definitions
			sz += serialize_size(state.ui_defs.gfx);
			sz += serialize_size(state.ui_defs.textures);
			sz += serialize_size(state.ui_defs.gui);
			sz += serialize_size(state.ui_defs.emfx);
			sz += serialize_size(state.font_collection.font_names);
			sz += serialize_size(state.ui_defs.extensions);
			sz += serialize_size(state.ui_defs.terrain_gfx);
		}

		// data container contribution
		dcon::load_record loaded = state.world.make_serialize_record_store_scenario();
		// dcon::load_record loaded;
		auto szb = state.world.serialize_size(loaded);

		return scenario_size{ sz + szb, sz };
	}

	uint8_t const* read_save_section(uint8_t const* ptr_in, uint8_t const* section_end, sys::state& state) {
		// hand-written contribution
		ptr_in = deserialize(ptr_in, state.unit_names);
		ptr_in = deserialize(ptr_in, state.unit_names_indices);
		ptr_in = memcpy_deserialize(ptr_in, state.local_player_nation);
		ptr_in = memcpy_deserialize(ptr_in, state.current_date);
		ptr_in = memcpy_deserialize(ptr_in, state.game_seed);
		ptr_in = memcpy_deserialize(ptr_in, state.crisis_state);
		ptr_in = deserialize(ptr_in, state.crisis_participants);
		ptr_in = memcpy_deserialize(ptr_in, state.current_crisis);
		ptr_in = memcpy_deserialize(ptr_in, state.crisis_temperature);
		ptr_in = memcpy_deserialize(ptr_in, state.primary_crisis_attacker);
		ptr_in = memcpy_deserialize(ptr_in, state.primary_crisis_defender);
		ptr_in = memcpy_deserialize(ptr_in, state.current_crisis_mode);
		ptr_in = memcpy_deserialize(ptr_in, state.crisis_last_checked_gp);
		ptr_in = memcpy_deserialize(ptr_in, state.crisis_war);
		ptr_in = memcpy_deserialize(ptr_in, state.last_crisis_end_date);
		ptr_in = memcpy_deserialize(ptr_in, state.crisis_liberation_tag);
		ptr_in = memcpy_deserialize(ptr_in, state.crisis_colony);
		ptr_in = memcpy_deserialize(ptr_in, state.difficulty);
		ptr_in = memcpy_deserialize(ptr_in, state.inflation);
		ptr_in = deserialize(ptr_in, state.great_nations);
		ptr_in = deserialize(ptr_in, state.pending_n_event);
		ptr_in = deserialize(ptr_in, state.pending_f_n_event);
		ptr_in = deserialize(ptr_in, state.pending_p_event);
		ptr_in = deserialize(ptr_in, state.pending_f_p_event);
		ptr_in = memcpy_deserialize(ptr_in, state.pending_messages);
		ptr_in = memcpy_deserialize(ptr_in, state.player_data_cache);
		ptr_in = deserialize(ptr_in, state.future_n_event);
		ptr_in = deserialize(ptr_in, state.future_p_event);

		{ // news definitions
			ptr_in = memcpy_deserialize(ptr_in, state.news_definitions);
		}
		{ // national definitions
			ptr_in = deserialize(ptr_in, state.national_definitions.global_flag_variables);
		}

		{ // military definitions
			ptr_in = memcpy_deserialize(ptr_in, state.military_definitions.great_wars_enabled);
			ptr_in = memcpy_deserialize(ptr_in, state.military_definitions.world_wars_enabled);
		}

		// data container contribution

		dcon::load_record loaded;
		std::byte const* start = reinterpret_cast<std::byte const*>(ptr_in);
		state.world.deserialize(start, reinterpret_cast<std::byte const*>(section_end), loaded);

		return section_end;
	}

	uint8_t* write_save_section(uint8_t* ptr_in, sys::state& state) {
		// hand-written contribution
		ptr_in = serialize(ptr_in, state.unit_names);
		ptr_in = serialize(ptr_in, state.unit_names_indices);
		ptr_in = memcpy_serialize(ptr_in, state.local_player_nation);
		ptr_in = memcpy_serialize(ptr_in, state.current_date);
		ptr_in = memcpy_serialize(ptr_in, state.game_seed);
		ptr_in = memcpy_serialize(ptr_in, state.crisis_state);
		ptr_in = serialize(ptr_in, state.crisis_participants);
		ptr_in = memcpy_serialize(ptr_in, state.current_crisis);
		ptr_in = memcpy_serialize(ptr_in, state.crisis_temperature);
		ptr_in = memcpy_serialize(ptr_in, state.primary_crisis_attacker);
		ptr_in = memcpy_serialize(ptr_in, state.primary_crisis_defender);
		ptr_in = memcpy_serialize(ptr_in, state.current_crisis_mode);
		ptr_in = memcpy_serialize(ptr_in, state.crisis_last_checked_gp);
		ptr_in = memcpy_serialize(ptr_in, state.crisis_war);
		ptr_in = memcpy_serialize(ptr_in, state.last_crisis_end_date);
		ptr_in = memcpy_serialize(ptr_in, state.crisis_liberation_tag);
		ptr_in = memcpy_serialize(ptr_in, state.crisis_colony);
		ptr_in = memcpy_serialize(ptr_in, state.difficulty);
		ptr_in = memcpy_serialize(ptr_in, state.inflation);
		ptr_in = serialize(ptr_in, state.great_nations);
		ptr_in = serialize(ptr_in, state.pending_n_event);
		ptr_in = serialize(ptr_in, state.pending_f_n_event);
		ptr_in = serialize(ptr_in, state.pending_p_event);
		ptr_in = serialize(ptr_in, state.pending_f_p_event);
		ptr_in = memcpy_serialize(ptr_in, state.pending_messages);
		ptr_in = memcpy_serialize(ptr_in, state.player_data_cache);
		ptr_in = serialize(ptr_in, state.future_n_event);
		ptr_in = serialize(ptr_in, state.future_p_event);
		{ // news definitions
			ptr_in = memcpy_serialize(ptr_in, state.news_definitions);
		}
		{ // national definitions
			ptr_in = serialize(ptr_in, state.national_definitions.global_flag_variables);
		}
		{ // military definitions
			ptr_in = memcpy_serialize(ptr_in, state.military_definitions.great_wars_enabled);
			ptr_in = memcpy_serialize(ptr_in, state.military_definitions.world_wars_enabled);
		}

		// data container contribution
		dcon::load_record loaded = state.world.make_serialize_record_store_save();
		std::byte* start = reinterpret_cast<std::byte*>(ptr_in);
		state.world.serialize(start, loaded);

		return reinterpret_cast<uint8_t*>(start);
	}
	size_t sizeof_save_section(sys::state& state) {
		size_t sz = 0;
		// hand-written contribution
		sz += serialize_size(state.unit_names);
		sz += serialize_size(state.unit_names_indices);
		sz += sizeof(state.local_player_nation);
		sz += sizeof(state.current_date);
		sz += sizeof(state.game_seed);
		sz += sizeof(state.crisis_state);
		sz += serialize_size(state.crisis_participants);
		sz += sizeof(state.current_crisis);
		sz += sizeof(state.crisis_temperature);
		sz += sizeof(state.primary_crisis_attacker);
		sz += sizeof(state.primary_crisis_defender);
		sz += sizeof(state.current_crisis_mode);
		sz += sizeof(state.crisis_last_checked_gp);
		sz += sizeof(state.crisis_war);
		sz += sizeof(state.last_crisis_end_date);
		sz += sizeof(state.crisis_liberation_tag);
		sz += sizeof(state.crisis_colony);
		sz += sizeof(state.difficulty);
		sz += sizeof(state.inflation);
		sz += serialize_size(state.great_nations);
		sz += serialize_size(state.pending_n_event);
		sz += serialize_size(state.pending_f_n_event);
		sz += serialize_size(state.pending_p_event);
		sz += serialize_size(state.pending_f_p_event);
		sz += sizeof(state.pending_messages);
		sz += sizeof(state.player_data_cache);
		sz += serialize_size(state.future_n_event);
		sz += serialize_size(state.future_p_event);
		{ // news definitions
			sz += sizeof(state.news_definitions);
		}
		{ // national definitions
			sz += serialize_size(state.national_definitions.global_flag_variables);
		}
		{ // military definitions
			sz += sizeof(state.military_definitions.great_wars_enabled);
			sz += sizeof(state.military_definitions.world_wars_enabled);
		}
		// data container contribution
		dcon::load_record loaded = state.world.make_serialize_record_store_save();
		sz += size_t(state.world.serialize_size(loaded));
		return sz;
	}

	void write_scenario_file(sys::state& state, native_string_view name, uint32_t count) {
		scenario_header header;
		header.count = count;
		header.timestamp = uint64_t(std::time(nullptr));

		auto scenario_space = sizeof_scenario_section(state);
		size_t save_space = sizeof_save_section(state);

		state.scenario_counter = count;
		state.scenario_time_stamp = header.timestamp;


		// this is an upper bound, since compacting the data may require less space
		size_t total_size = sizeof_scenario_header(header)
			+ sizeof_mod_path(simple_fs::extract_state(state.common_fs))
			+ ZSTD_compressBound(size_t(scenario_space.total_size))
			+ ZSTD_compressBound(save_space)
			+ sizeof(uint32_t) * 4;

		uint8_t* temp_buffer = new uint8_t[total_size];
		uint8_t* buffer_position = temp_buffer;

		buffer_position = write_scenario_header(buffer_position, header);
		buffer_position = write_mod_path(buffer_position, simple_fs::extract_state(state.common_fs));

		uint8_t* temp_scenario_buffer = new uint8_t[size_t(scenario_space.total_size)];
		auto last_written = write_scenario_section(temp_scenario_buffer, state);
		auto last_written_count = last_written - temp_scenario_buffer;
		assert(size_t(last_written_count) == scenario_space.total_size);
		// calculate checksum
		checksum_key* checksum = &reinterpret_cast<scenario_header*>(temp_buffer + sizeof(uint32_t))->checksum;
		blake2b(checksum, sizeof(*checksum), temp_scenario_buffer + size_t(scenario_space.checksum_offset), size_t(scenario_space.total_size - scenario_space.checksum_offset), nullptr, 0);
		state.scenario_checksum = *checksum;

		buffer_position = write_compressed_section(buffer_position, temp_scenario_buffer, uint32_t(scenario_space.total_size));
		delete[] temp_scenario_buffer;

		uint8_t* temp_save_buffer = new uint8_t[save_space];
		auto last_save_written = write_save_section(temp_save_buffer, state);
		auto last_save_written_count = last_save_written - temp_save_buffer;
		assert(size_t(last_save_written_count) == save_space);
		buffer_position = write_compressed_section(buffer_position, temp_save_buffer, uint32_t(save_space));
		delete[] temp_save_buffer;

		auto total_size_used = buffer_position - temp_buffer;

		simple_fs::write_file(simple_fs::get_or_create_scenario_directory(), name, reinterpret_cast<char*>(temp_buffer),
			uint32_t(total_size_used));

		delete[] temp_buffer;
	}
	bool try_read_scenario_file(sys::state& state, native_string_view name) {
		auto dir = simple_fs::get_or_create_scenario_directory();
		auto save_file = open_file(dir, name);
		if(save_file) {
			scenario_header header;
			header.version = 0;

			auto contents = simple_fs::view_contents(*save_file);
			uint8_t const* buffer_pos = reinterpret_cast<uint8_t const*>(contents.data);
			auto file_end = buffer_pos + contents.file_size;

			if(contents.file_size > sizeof_scenario_header(header)) {
				buffer_pos = read_scenario_header(buffer_pos, header);
			}

			if(header.version != sys::scenario_file_version) {
				return false;
			}

			state.scenario_counter = header.count;
			state.scenario_time_stamp = header.timestamp;
			state.scenario_checksum = header.checksum;
			state.loaded_save_file = NATIVE("");
			state.loaded_scenario_file = name;

			buffer_pos = load_mod_path(buffer_pos, state);

			buffer_pos = with_decompressed_section(buffer_pos,
				[&](uint8_t const* ptr_in, uint32_t length) { read_scenario_section(ptr_in, ptr_in + length, state); });

			return true;
		} else {
			return false;
		}
	}

	bool try_read_scenario_and_save_file(sys::state& state, native_string_view name) {
		auto dir = simple_fs::get_or_create_scenario_directory();
		auto save_file = open_file(dir, name);
		if(save_file) {
			scenario_header header;
			header.version = 0;

			auto contents = simple_fs::view_contents(*save_file);
			uint8_t const* buffer_pos = reinterpret_cast<uint8_t const*>(contents.data);
			auto file_end = buffer_pos + contents.file_size;

			if(contents.file_size > sizeof_scenario_header(header)) {
				buffer_pos = read_scenario_header(buffer_pos, header);
			}

			if(header.version != sys::scenario_file_version) {
				return false;
			}

			state.scenario_counter = header.count;
			state.scenario_time_stamp = header.timestamp;
			state.scenario_checksum = header.checksum;

			state.loaded_save_file = NATIVE("");
			state.loaded_scenario_file = name;

			buffer_pos = load_mod_path(buffer_pos, state);

			buffer_pos = with_decompressed_section(buffer_pos,
				[&](uint8_t const* ptr_in, uint32_t length) { read_scenario_section(ptr_in, ptr_in + length, state); });
			buffer_pos = with_decompressed_section(buffer_pos,
				[&](uint8_t const* ptr_in, uint32_t length) { read_save_section(ptr_in, ptr_in + length, state); });
			return true;
		}
		return false;
	}

	bool try_read_scenario_as_save_file(sys::state& state, native_string_view name) {
		auto dir = simple_fs::get_or_create_scenario_directory();
		auto save_file = open_file(dir, name);
		if(save_file) {
			scenario_header header;
			header.version = 0;

			auto contents = simple_fs::view_contents(*save_file);
			uint8_t const* buffer_pos = reinterpret_cast<uint8_t const*>(contents.data);
			auto file_end = buffer_pos + contents.file_size;

			if(contents.file_size > sizeof_scenario_header(header)) {
				buffer_pos = read_scenario_header(buffer_pos, header);
			}

			if(header.version != sys::scenario_file_version) {
				return false;
			}

			if(!state.scenario_checksum.is_equal(header.checksum))
			return false;

			state.loaded_save_file = NATIVE("");

			buffer_pos = load_mod_path(buffer_pos, state);

			buffer_pos = with_decompressed_section(buffer_pos,
			[&](uint8_t const* ptr_in, uint32_t length) {
				// DO NOTHING -- this skips over reading the scenario section
			});
			buffer_pos = with_decompressed_section(buffer_pos,
			[&](uint8_t const* ptr_in, uint32_t length) {
				read_save_section(ptr_in, ptr_in + length, state);
			});
			return true;
		}
		return false;
	}

	std::string make_time_string(uint64_t value) {
		std::string result;
		for(int32_t i = 64 / 4; i --> 0; ) {
			result += char('a' + (0x0F & (value >> (i * 4))));
		}
		return result;
	}

	void write_save_file(sys::state& state, save_type type, std::string const& name) {
		save_header header;
		header.count = state.scenario_counter;
		//header.timestamp = state.scenario_time_stamp;
		auto time_stamp = std::time(nullptr);
		header.timestamp = int64_t(time_stamp);
		header.checksum = state.scenario_checksum;
		header.tag = state.world.nation_get_identity_from_identity_holder(state.local_player_nation);
		header.cgov = state.world.nation_get_government_type(state.local_player_nation);
		header.d = state.current_date;

		std::memcpy(header.save_name, name.c_str(), std::min(name.length(), size_t(31)));
		if(name.length() < 31) {
			header.save_name[name.length()] = 0;
		} else {
			header.save_name[31] = 0;
		}

		size_t save_space = sizeof_save_section(state);

		// this is an upper bound, since compacting the data may require less space
		size_t total_size = sizeof_save_header(header) + ZSTD_compressBound(save_space) + sizeof(uint32_t) * 2;

		uint8_t* temp_buffer = new uint8_t[total_size];
		uint8_t* buffer_position = temp_buffer;

		buffer_position = write_save_header(buffer_position, header);

		uint8_t* temp_save_buffer = new uint8_t[save_space];
		write_save_section(temp_save_buffer, state);
		buffer_position = write_compressed_section(buffer_position, temp_save_buffer, uint32_t(save_space));
		delete[] temp_save_buffer;

		auto total_size_used = buffer_position - temp_buffer;

		auto sdir = simple_fs::get_or_create_save_game_directory();

		if(type == sys::save_type::autosave) {
			simple_fs::write_file(sdir, native_string(NATIVE("autosave_")) + text::utf8_to_native(std::to_string(state.autosave_counter)) + native_string(NATIVE(".bin")), reinterpret_cast<char*>(temp_buffer), uint32_t(total_size_used));
			state.autosave_counter = (state.autosave_counter + 1) % sys::max_autosaves;
		} else if(type == sys::save_type::bookmark) {
			auto ymd_date = state.current_date.to_ymd(state.start_date);
			auto base_str = "bookmark_" + make_time_string(uint64_t(std::time(nullptr))) + "-" + std::to_string(ymd_date.year) + "-" + std::to_string(ymd_date.month) + "-" + std::to_string(ymd_date.day) + ".bin";
			simple_fs::write_file(sdir, text::utf8_to_native(base_str), reinterpret_cast<char*>(temp_buffer), uint32_t(total_size_used));
		} else {
			auto ymd_date = state.current_date.to_ymd(state.start_date);
			auto base_str = make_time_string(uint64_t(std::time(nullptr))) + "-" + nations::int_to_tag(state.world.national_identity_get_identifying_int(header.tag)) + "-" + std::to_string(ymd_date.year) + "-" + std::to_string(ymd_date.month) + "-" + std::to_string(ymd_date.day) + ".bin";
			simple_fs::write_file(sdir, text::utf8_to_native(base_str), reinterpret_cast<char*>(temp_buffer), uint32_t(total_size_used));
		}
		delete[] temp_buffer;

		state.save_list_updated.store(true, std::memory_order::release); // update for ui
	}
	bool try_read_save_file(sys::state& state, native_string_view name) {
		auto dir = simple_fs::get_or_create_save_game_directory();
		auto save_file = open_file(dir, name);
		if(save_file) {
			save_header header;
			header.version = 0;

			auto contents = simple_fs::view_contents(*save_file);
			uint8_t const* buffer_pos = reinterpret_cast<uint8_t const*>(contents.data);
			auto file_end = buffer_pos + contents.file_size;

			if(contents.file_size > sizeof_save_header(header)) {
				buffer_pos = read_save_header(buffer_pos, header);
			}

			if(header.version != sys::save_file_version) {
				return false;
			}

			//if(state.scenario_counter != header.count)
			//	return false;
			//if(state.scenario_time_stamp != header.timestamp)
			//	return false;
			if(!state.scenario_checksum.is_equal(header.checksum))
			return false;

			state.loaded_save_file = name;

			buffer_pos = with_decompressed_section(buffer_pos,
				[&](uint8_t const* ptr_in, uint32_t length) { read_save_section(ptr_in, ptr_in + length, state); });

			return true;
		} else {
			return false;
		}
	}

} // namespace sys
