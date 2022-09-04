#include "globals.hpp"

#include <Windows.h>

std::vector<settings::base_variable*>& settings::get_vars() {
	static std::vector<settings::base_variable*> v;
	return v;
}

std::string settings::save_settings_to_string() {
	nlohmann::json json;
	std::for_each(get_vars().begin(), get_vars().end(),
		[&](settings::base_variable* listener) { if (listener) listener->save(json); });
	return json.dump();
}

void settings::save_settings_to_stream(std::ostream& stream) {
	stream.write(save_settings_to_string().c_str(), save_settings_to_string().size());
}

void settings::load_settings_from_string(std::string_view data) {
	auto json = nlohmann::json::parse(data);
	std::for_each(get_vars().begin(), get_vars().end(),
		[&](settings::base_variable* listener) { if (listener) listener->load(json); });
}

void settings::load_settings_from_stream(std::istream& stream) {
	std::string data;
	std::string line;
	while (std::getline(stream, line))
		data.append(line);
	load_settings_from_string(data);
}
