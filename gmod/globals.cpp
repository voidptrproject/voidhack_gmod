#include "globals.hpp"

namespace internal {
	static inline ::internal::variables_storage_t settings;
	static inline ::internal::variables_storage_t globals;
}

::internal::variables_storage_t& settings::internal::get_settings_storage() {
	return ::internal::settings;
}

::internal::variables_storage_t& globals::internal::get_globals_storage() {
	return ::internal::globals;
}
