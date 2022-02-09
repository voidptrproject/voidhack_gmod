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

		inline std::string get_name() const {
			char name[MAX_PATH]; GetModuleFileName(handle, name, MAX_PATH);
			return name;
		}
		inline auto get_handle() const { return handle; }
		inline auto get_dos_header() const { return reinterpret_cast<PIMAGE_DOS_HEADER>(handle); }
		inline auto get_nt_headers() const { return reinterpret_cast<PIMAGE_NT_HEADERS>(reinterpret_cast<std::uint8_t*>(handle) + get_dos_header()->e_lfanew); }

		inline std::uint8_t* begin() const {
			return reinterpret_cast<std::uint8_t*>(get_handle());
		}
		inline std::uint8_t* end() const {
			return reinterpret_cast<std::uint8_t*>(get_handle() + get_nt_headers()->OptionalHeader.SizeOfImage);
		}
	};
	template <typename _t> struct symbol_t {
		module_t module;
		_t ptr;

		inline static symbol_t<_t> get_symbol(const module_t& module, std::string_view name) {
			symbol_t<_t> symbol;
			symbol.module = module;
			symbol.ptr = (_t)GetProcAddress(module.get_handle(), name.data());
			return symbol;
		}

		inline static symbol_t<_t> get_symbol(const module_t& module, const jm::memory_signature& sig) {
			symbol_t<_t> symbol;
			symbol.module = module;
		}

		inline _t operator->() { return ptr; }
	};

	static inline constexpr auto relative_to_absolute(uintptr_t address, int offset, int instruction_size) noexcept {
		return address + instruction_size + (*(int*)(address + offset));
	}

	struct address_t {
		address_t(uintptr_t addr) : address(addr) {}
		address_t(const jm::memory_signature& sig, const module_t& mod) {
			address = (uintptr_t)sig.find(mod.begin(), mod.end());
		}
		inline auto absolute(int offset, int instruction_size) const {
			return relative_to_absolute(address, offset, instruction_size);
		}
		inline auto get_info() const {
			MEMORY_BASIC_INFORMATION mi; VirtualQuery((LPCVOID)address, &mi, sizeof(MEMORY_BASIC_INFORMATION));
			return mi;
		}
		inline auto get_module() const {
			return module_t((HMODULE)get_info().AllocationBase);
		}

		uintptr_t operator()() { return address; }
		uintptr_t address;
	};

	// Modules ------
	static inline module_t tier0_module("tier0.dll");
	static inline module_t menusystem_module("menusystem.dll");
	static inline module_t engine_module("engine.dll");
	static inline module_t client_module("client.dll");
	static inline module_t material_system_module("materialsystem.dll");
	static inline module_t matsurface_module("vguimatsurface.dll");
	static inline module_t lua_shared_module("lua_shared.dll");
	static inline module_t vgui_module("vgui2.dll");
	static inline module_t vstdlib_module("vstdlib.dll");
	// -----------

	// Global vars --------
	static inline HINSTANCE dllinstance;
	// --------------

	template<class t> struct interface_t {
		interface_t(t* p) : ptr(p) {}
		interface_t(const memory::module_t& mod, std::string_view name) { ptr = create_interface(mod, name); }

		t* operator->() { return ptr; }
		t* ptr;

		static t* create_interface(const memory::module_t& mod, std::string_view name) {
			return memory::symbol_t<t* (*)(const char*, int)>::get_symbol(mod, "CreateInterface").ptr(name.data(), 0);
		}
	};
}