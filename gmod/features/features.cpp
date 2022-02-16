#include "features.hpp"

static inline features::features_interface_t _interface;

features::features_interface_t& features::get_features_interface() {
	return _interface;
}
