#include "input.hpp"

input::internal::key_handlers_t& input::internal::get_handlers() {
	static input::internal::key_handlers_t instance;
	return instance;
}
