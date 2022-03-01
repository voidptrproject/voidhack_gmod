#include "features.hpp"

features::features_interface_t& features::get_features_interface() {
	static features::features_interface_t _interface;
	return _interface;
}
