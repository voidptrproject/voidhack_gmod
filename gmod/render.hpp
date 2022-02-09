#pragma once

#include "memory.hpp"

namespace render {
	namespace internal {
		void render_hook(const memory::address_t& return_address);
	}

}