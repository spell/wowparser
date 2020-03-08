//
// Created by Jesse on 08/03/2020.
//

#pragma once

#include <string_view>
#include <optional>
#include <variant>

#include "parameters.hpp"

enum class subevent_prefix_type : int {
	unknown,
	swing,
	range,
	spell,
	spell_periodic,
	spell_building,
	environmental,
};

enum class subevent_suffix_type : int {
	unknown,
	damage,
	missed,
	heal,
};

subevent_prefix_type event_type_to_prefix(std::string_view s) {
	if (s.starts_with("SWING")) {
		return subevent_prefix_type::swing;
	} else if (s.starts_with("SPELL_PERIODIC")) {
		return subevent_prefix_type::spell_periodic;
	} else if (s.starts_with("SPELL_BUILDING")) {
		return subevent_prefix_type::spell_building;
	} else if (s.starts_with("SPELL")) {
		return subevent_prefix_type::spell;
	} else if (s.starts_with("RANGE")) {
		return subevent_prefix_type::range;
	} else if (s.starts_with("ENVIRONMENTAL")) {
		return subevent_prefix_type::environmental;
	}

	return subevent_prefix_type::unknown;
}

subevent_suffix_type event_type_to_suffix(std::string_view s) {
	if (s.ends_with("DAMAGE")) {
		return subevent_suffix_type::damage;
	} else if (s.ends_with("MISSED")) {
		return subevent_suffix_type::missed;
	} else if (s.ends_with("HEAL")) {
		return subevent_suffix_type::heal;
	}

	return subevent_suffix_type::unknown;
}

using subevent_prefix = std::variant<swing_parameters, spell_parameters, environmental_parameters>;
using subevent_suffix = std::variant<damage_parameters, missed_parameters, heal_parameters>;

struct combat_event {
	std::string event_type;
	subevent_prefix_type prefix_type;
	subevent_suffix_type suffix_type;
	base_parameters base;
	advanced_parameters advanced;
	std::optional<subevent_prefix> prefix;
	std::optional<subevent_suffix> suffix;
};
