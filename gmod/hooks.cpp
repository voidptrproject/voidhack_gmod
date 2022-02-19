#include "hooks.hpp"

#include <imgui_impl_win32.h>

#include <kiero.h>
#include <MinHook.h>

#include "memory.hpp"
#include "render.hpp"
#include "globals.hpp"
#include "input.hpp"

#include <d3d9.h>
#include <Windows.h>
#include <intrin.h>

#include <thread>
#include <functional>
#include <algorithm>
#include <assert.h>

#include "interfaces.hpp"
#include "features/features.hpp"

#include "game/entities/c_base_player.hpp"

static auto warning = memory::symbol_t<void(__cdecl*)(char const*, ...)>::get_symbol(memory::tier0_module, "Warning").ptr;

template <typename func> struct hook_t {
	hook_t() {}
	hook_t(func new_function) : hook_fn(new_function) {}
	~hook_t() {}

	func original;
	func hook_fn;

	void hook(uintptr_t address) { MH_CreateHook((LPVOID)address, (LPVOID)hook_fn, (LPVOID*)&original); MH_EnableHook((LPVOID)address); }
	void unhook() { MH_DisableHook((LPVOID)original); MH_RemoveHook((LPVOID)original); }
};

static inline hook_t<void(__fastcall*)(float, bool)> cl_move_hook([](float a, bool b) {
	return cl_move_hook.original(a, b);
});

static inline long end_scane_hooked_fn(IDirect3DDevice9* device);
static inline hook_t<long(__stdcall*)(IDirect3DDevice9*)> end_scene_hook(end_scane_hooked_fn);
inline long end_scane_hooked_fn(IDirect3DDevice9* device) {
	render::internal::render_hook(device, (uintptr_t)_ReturnAddress());
	return end_scene_hook.original(device);
}

static inline hook_t<bool(__fastcall*)(i_client_mode*, float, c_user_cmd*)> create_move_hook ([](i_client_mode* self, float frame_time, c_user_cmd* cmd) -> bool {
	if (!cmd || cmd->command_number == 0 || !interfaces::engine->is_in_game())
		return create_move_hook.original(self, frame_time, cmd);
	bool _return = false;
	std::for_each(features::get_features_interface().features.begin(), features::get_features_interface().features.end(), [&](features::feature* f) {
		if (f->create_move_callback)
			if (!_return && f->create_move_callback.value()(frame_time, cmd))
				_return = true;
	});

	return create_move_hook.original(self, frame_time, cmd);
});

static inline hook_t<void(__stdcall*)(i_surface*)> lock_cursor_hook([](i_surface* self) {
	if (globals::get<bool>("menu_opened"))
		return self->unlock_cursor();
	return lock_cursor_hook.original(self);
});

static inline WNDPROC original_wndproc = nullptr; extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
static LRESULT STDMETHODCALLTYPE hooked_wndproc(HWND window, UINT message_type, WPARAM w_param, LPARAM l_param) {
	if (message_type == WM_KEYDOWN || message_type == WM_SYSKEYDOWN)
		input::internal::notify_handlers(w_param, input::e_key_state::pressed);
	else if (message_type == WM_KEYUP || message_type == WM_SYSKEYUP)
		input::internal::notify_handlers(w_param, input::e_key_state::released);

	if (ImGui_ImplWin32_WndProcHandler(window, message_type, w_param, l_param) && globals::get<bool>("menu_opened")) {
		return TRUE;
	}
	return CallWindowProc(original_wndproc, window, message_type, w_param, l_param);
}

static inline hook_t<void(__fastcall*)(chl_client*, int)> frame_stage_notify_hook([](chl_client* client, int frame_stage) {
	std::for_each(features::get_features_interface().features.begin(), features::get_features_interface().features.end(), [&](features::feature* f) {
		if (f->frame_stage_callback) f->frame_stage_callback.value()(frame_stage);
	});
	return frame_stage_notify_hook.original(client, frame_stage);});

void hooks::initialize_hooks() {
	assert(MH_Initialize() == MH_OK);
	assert(kiero::init(kiero::RenderType::D3D9) == kiero::Status::Success);

	original_wndproc = (WNDPROC)SetWindowLongPtr(FindWindowW(0, L"Garry's Mod (x64)"), GWLP_WNDPROC, (LONG_PTR)hooked_wndproc);
	cl_move_hook.hook(memory::address_t({ "E8 ? ? ? ? FF 15 ? ? ? ? F2 0F 10 0D ? ? ? ? 85 FF" }, memory::engine_module).absolute(0x1, 0x5));
	create_move_hook.hook(memory::address_t({ "40 53 48 83 EC 30 0F 29 74 24 ? 49 8B D8" }, memory::client_module));
	frame_stage_notify_hook.hook(memory::address_t({ "48 83 EC 28 89 15 ? ? ? ?" }, memory::client_module));
	//40 55 53 56 41 54 41 55 41 56 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 45 8B 70 40
	end_scene_hook.hook(kiero::getMethodsTable()[42]);
	lock_cursor_hook.hook(interfaces::surface.get_virtual_table()[66]);
}

void hooks::shutdown_hooks() {
	MH_RemoveHook(MH_ALL_HOOKS);
	MH_Uninitialize();
	SetWindowLongPtr(FindWindowW(0, L"Garry's Mod (x64)"), GWLP_WNDPROC, (LONG_PTR)original_wndproc);
}
