#pragma once

#include "memory.hpp"

#include <d3d9.h>

namespace render {
	namespace internal {
		void render_hook(IDirect3DDevice9* device, const memory::address_t& return_address);
	}

	IDirect3DDevice9* get_device();
}