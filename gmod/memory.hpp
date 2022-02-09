#pragma once

#include <memory_signature.hpp>

#include <memory>
#include <string_view>
#include <string>

#include <Windows.h>

namespace memory {
	class module_t {
		HMODULE handle;
	public:
		module_t() {}
		module_t(HMODULE handle) : handle(handle) {}
		module_t(const module_t&) = default;
		module_t(std::string_view module_name) {
			handle = GetModuleHandle(module_name.data());
		}

		inline auto get_handle() const { return handle; }
		inline auto get_dos_header() const { return reinterpret_cast<PIMAGE_DOS_HEADER>(handle); }
		inline auto get_nt_headers() const { return reinterpret_cast<PIMAGE_NT_HEADERS>(reinterpret_cast<std::uint8_t*>(handle) + get_dos_header()->e_lfanew); }

		inline std::uint8_t* begin() {
			return reinterpret_cast<std::uint8_t*>(get_handle());
		}

		inline std::uint8_t* end() {
			return reinterpret_cast<std::uint8_t*>(get_handle() + get_nt_headers()->OptionalHeader.SizeOfImage);
		}
	};
	template <typename _t> struct symbol_t {
		module_t module;
		_t ptr;

		inline static symbol_t<_t> get_symbol(module_t module, std::string_view name) {
			symbol_t<_t> symbol;
			symbol.module = module;
			symbol.ptr = (_t)GetProcAddress(module.get_handle(), name.data());
			return symbol;
		}

		inline _t operator->() { return ptr; }
	};
	
	static inline constexpr auto relative_to_absolute(uintptr_t address, int offset, int instruction_size) noexcept 
	{ return address + instruction_size + (*(int*)(address + offset)); }

	// Modules
	static inline module_t tier0_module("tier0.dll");
	static inline module_t menusystem_module("menusystem.dll");
	static inline module_t engine_module("engine.dll");
	static inline module_t client_module("client.dll");
	static inline module_t material_system_module("materialsystem.dll");
	static inline module_t matsurface_module("vguimatsurface.dll");
	static inline module_t lua_shared_module("lua_shared.dll");
	static inline module_t vgui_module("vgui2.dll");
	static inline module_t vstdlib_module("vstdlib.dll");

	// Global vars
	static inline HINSTANCE dllinstance;
}