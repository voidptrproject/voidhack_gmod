#include "input.hpp"

static inline input::internal::key_handlers_t instance;

input::internal::key_handlers_t& input::internal::get_handlers() {
	return instance;
}
