#pragma once

#include <functional>
#include <vector>
#include <optional>

#include "../interfaces.hpp"
#include "../render.hpp"
#include "../hooks.hpp"
#include "../globals.hpp"
#include "../input.hpp"
#include "../menu/menu.hpp"

namespace features {
	struct feature;
	struct features_interface_t {
		std::vector<feature*> features;

		void add_feature(feature* f) { features.push_back(f); }
	};
	features_interface_t& get_features_interface();

	struct feature {
		feature(std::function<void()> initialize_function) {
			initialize_function();
			get_features_interface().add_feature(this);
		}
	};
}