#pragma once

#include <functional>
#include <vector>
#include <optional>

#include "../interfaces.hpp"
#include "../render.hpp"

namespace features {
	struct feature;
	struct features_interface_t {
		std::vector<feature*> features;

		void add_feature(feature* f) { features.push_back(f); }
	};
	features_interface_t& get_features_interface();

	using create_move_callback_t = std::optional<std::function<bool(float, c_user_cmd*)>>;
	using frame_stage_notify_callback_t = std::optional<std::function<void(int stage)>>;
	using feature_initialize_callback_t = std::optional<std::function<void()>>;

	constexpr auto pass_callback = std::nullopt;
	struct feature {
		feature_initialize_callback_t initialize_callback;
		create_move_callback_t create_move_callback;
		frame_stage_notify_callback_t frame_stage_callback;

		feature(feature_initialize_callback_t initialize = pass_callback, create_move_callback_t create_move = pass_callback, frame_stage_notify_callback_t fs = pass_callback) 
			: frame_stage_callback(fs), create_move_callback(create_move), initialize_callback(initialize) {
			if (initialize.has_value()) initialize.value()();
			get_features_interface().add_feature(this);
		}
	};
}