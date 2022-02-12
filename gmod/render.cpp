#include "render.hpp"

#include <mutex>

#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx9.h>

#include "globals.hpp"
#include "input.hpp"

static inline IDirect3DDevice9* device;

inline static input::key_handler_t menu_key_handler("menu_open", VK_INSERT, [&](input::e_key_state state) {
	auto& var = settings::get<bool>("menu_opened");
	if (state == input::e_key_state::released)
		var = !var;
});

void render::internal::render_hook(IDirect3DDevice9* dev, const memory::address_t& return_address) {
	static std::once_flag once_flag; std::call_once(once_flag, [&]() {
		if (auto hwnd = FindWindow(0, "Garry's Mod (x64)"); hwnd) {
			ImGui::CreateContext();
			ImGui_ImplWin32_Init(hwnd);
			ImGui_ImplDX9_Init(dev);
			ImGui::GetIO().IniFilename = 0;
		}
	});

	dev = device;
	//if (!return_address.get_module().get_name().contains("gameoverlay")) return;

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	if (settings::get<bool>("menu_opened")) {
		ImGui::Begin("TESTWINDOW");

		ImGui::InputInt("TESTINPUT", &settings::get<int>("testint"));
		ImGui::Checkbox("Bhop", &settings::get<bool>("bhop"));

		ImGui::End();
	}

	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}

IDirect3DDevice9* render::get_device() {
	return device;
}
