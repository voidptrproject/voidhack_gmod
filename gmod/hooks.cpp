#include "hooks.hpp"

#include <MinHook.h>
#include "memory.hpp"

#include <vector>

template <typename func> struct hook_t {
	hook_t() {}
	hook_t(func new_function) : hook_fn(new_function) {  }

	func original;
	func hook_fn;

	void hook(uintptr_t address) { MH_CreateHook((LPVOID)address, (LPVOID)hook_fn, (LPVOID*)&original); MH_EnableHook((LPVOID)address); }
	void unhook() { MH_DisableHook((LPVOID)original); MH_RemoveHook((LPVOID)original); }
};

hook_t<void(__fastcall*)(float, bool)> cl_move_hook([](float a, bool b) {
	auto warning = memory::symbol_t<void(__cdecl*)(char const*, ...)>::get_symbol(memory::tier0_module, "Warning");
	warning.ptr("CL_Move hook called!\n");

	cl_move_hook.original(a, b);
});

void hooks::initialize_hooks() {
	MH_Initialize();

	cl_move_hook.hook(memory::address_t({"E8 ? ? ? ? FF 15 ? ? ? ? F2 0F 10 0D ? ? ? ? 85 FF"}, memory::engine_module).absolute(0x1, 0x5));
}

void hooks::shutdown_hooks() {
	MH_Uninitialize();
}
