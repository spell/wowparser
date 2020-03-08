//
// Created by Jesse on 07/03/2020.
//

#pragma once

#include <charconv>
#include <concepts>
#include <string>
#include <tuple>

/*!
 * absenced of parameters
 */
using no_parameters = std::tuple<>;

/*!
 * combat_log_version_parameters encapsulates a COMBAT_LOG_VERSION event
 */
using combat_log_version_parameters = std::tuple<std::string, int, std::string, int, std::string, std::string, std::string, int>;

/*!
 * base parameters defines the first 8 parameters that show up in any combat log event
 */
using base_parameters = std::tuple<std::string, std::string, uint32_t, uint32_t, std::string, std::string, uint32_t, uint32_t>;

/*!
 * advanced parameters are the 9th through 27th parameter that appear in any advanced combat log
 */
using advanced_parameters = std::tuple<std::string, std::string, int64_t, int64_t, int32_t, int32_t, int32_t, int32_t,
		uint8_t, int32_t, int32_t, int32_t, float, float, uint32_t, float, int32_t>;

/*!
 * These are the subevent prefix parameters
 */
using swing_parameters = std::tuple<>;
using spell_parameters = std::tuple<uint32_t, std::string, uint32_t>;
using spell_periodic_parameters = spell_parameters;
using spell_building_parameters = spell_parameters;
using range_parameters = spell_parameters;
using environmental_parameters = std::tuple<std::string>;

/*!
 * damage parameters appear on any event of type *_DAMAGE
 */
using damage_parameters = std::tuple<uint32_t, uint32_t, int32_t, uint32_t, int32_t, int32_t, int32_t, bool, bool, bool>;
using missed_parameters = std::tuple<std::string, bool, uint32_t, uint32_t, bool>;
using heal_parameters = std::tuple<uint32_t, uint32_t, uint32_t, uint32_t, bool>;

template<typename T>
T convert_parameter(std::string_view s) {
	return s;
}

template<>
bool convert_parameter(std::string_view s) {
	if (s == "nil") return false;
	else if (s == "1") return true;
	throw std::exception("convert_parameter<bool>: value must be 'nil' or '1'");
}

template<>
std::string convert_parameter(std::string_view s) {
	if (s[0] == '"' && s[s.length() - 1] == '"') {
		return std::string(s.begin() + 1, s.end() - 1);
	}
	return std::string(s);
}

template<>
float convert_parameter(std::string_view s) {
	if (s.length() == 0) {
		throw std::exception("convert_parameter<float>: value is empty");
	}

	float result = 0.0f;
	auto [p, ec] = std::from_chars(s.data(), s.data() + s.size(), result);
	if (ec != std::errc()) {
		std::cerr << "convert_parameter<float>: invalid floating point: " << s << std::endl;
	}

	return result;
}

template<typename T>
concept integral_not_bool = std::is_integral_v<T> && !std::is_same_v<T, bool>;

template<integral_not_bool T>
T convert_parameter(std::string_view s) {
	if (s.length() == 0) {
		throw std::exception("convert_parameter<std::integral>: value is empty");
	}

	T result = 0;

	if (s.length() >= 2 && s[1] == 'x') {
		auto[p, ec] = std::from_chars(s.data() + 2, s.data() + s.size(), result, 16);
		if (ec != std::errc()) {
			std::cerr << "convert_parameter<std::integral>: invalid base-16 integer: " << s << std::endl;
		}
	} else {
		auto[p, ec] = std::from_chars(s.data(), s.data() + s.size(), result, 10);
		if (ec != std::errc()) {
			std::cerr << "convert_parameter<std::integral>: invalid base-10 integer: " << s << std::endl;
		}
	}

	return result;
}

template<typename T, typename S, size_t I = 0>
void parse_parameters(const S &parameters, T &dst, size_t offset = 0) {
	if constexpr (I < std::tuple_size<T>::value) {
		if (I + offset < parameters.size()) {
			std::get<I>(dst) = convert_parameter<std::tuple_element<I, T>::type>(parameters[offset + I]);
			parse_parameters<T, S, I + 1>(parameters, dst, offset);
		}
	}
}
