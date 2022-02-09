#include "hooks.hpp"

#include <MinHook.h>
#include <kiero.h>

#include "memory.hpp"
#include "interfaces.hpp"
#include "render.hpp"

#include <d3d9.h>
#include <Windows.h>
#include <intrin.h>
#include <thread>

static auto warning = memory::symbol_t<void(__cdecl*)(char const*, ...)>::get_symbol(memory::tier0_module, "Warning");

template <typename func> struct hook_t {
	hook_t() {}
	hook_t(func new_function) : hook_fn(new_function) {  }
	~hook_t() {}

	func original;
	func hook_fn;

	void hook(uintptr_t address) { MH_CreateHook((LPVOID)address, (LPVOID)hook_fn, (LPVOID*)&original); MH_EnableHook((LPVOID)address); }
	void unhook() { MH_DisableHook((LPVOID)original); MH_RemoveHook((LPVOID)original); }
};

static hook_t<void(__fastcall*)(float, bool)> cl_move_hook([](float a, bool b) {
	return cl_move_hook.original(a, b);
});

static hook_t<long(__stdcall*)(IDirect3DDevice9*)> end_scene_hook([](IDirect3DDevice9* device) -> long {
	render::internal::render_hook((uintptr_t)_ReturnAddress());
	return end_scene_hook.original(device);
});

static inline WNDPROC original_wndproc = nullptr;
static LRESULT STDMETHODCALLTYPE hooked_wndproc(HWND window, UINT message_type, WPARAM w_param, LPARAM l_param) {
	return CallWindowProc(original_wndproc, window, message_type, w_param, l_param);
}

void hooks::initialize_hooks() {
	MH_Initialize();
	kiero::init(kiero::RenderType::D3D9);

	original_wndproc = (WNDPROC)SetWindowLongPtr(FindWindowW(0, L"Garry's Mod (x64)"), GWLP_WNDPROC, (LONG_PTR)hooked_wndproc);
	cl_move_hook.hook(memory::address_t({"E8 ? ? ? ? FF 15 ? ? ? ? F2 0F 10 0D ? ? ? ? 85 FF"}, memory::engine_module).absolute(0x1, 0x5));
	end_scene_hook.hook(kiero::getMethodsTable()[42]);
}

void hooks::shutdown_hooks() {
	MH_RemoveHook(MH_ALL_HOOKS);
	MH_Uninitialize();
	SetWindowLongPtr(FindWindowW(0, L"Garry's Mod (x64)"), GWLP_WNDPROC, (LONG_PTR)original_wndproc);
}
