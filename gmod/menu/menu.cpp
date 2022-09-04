#include "menu.hpp"

#include "content.hpp"

#include "../input.hpp"
#include "../hooks.hpp"

#include <Windows.h>

#include <d3dx9.h>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image.h>
#include <stb_image_resize.h>
#include <stb_image_write.h>

using namespace menu;

enum class EMenuState {
	Closed,
	Opened
};

struct MenuContext {
	EMenuState state = EMenuState::Closed;
	float fadeAnimation = 0.f;
};

inline auto& GetMenuContext() {
	static MenuContext context;
	return context;
}

bool menu::MenuOpen() {
	return GetMenuContext().state == EMenuState::Opened || GetMenuContext().fadeAnimation > 0.f;
}

bool StyleSetuped = false;
void* BackgroundImageData = nullptr;
ImVec2 ImageSizes;
float ImageAlpha = 0.8f;
bool AlreadyUpdateImage = false;

void LoadBackgroundImage() {
	if (AlreadyUpdateImage)
		return;
	AlreadyUpdateImage = true;
	std::thread([&]() {
		auto image_path = (env::get_data_path() / "bg.png");

		int w, h, cn;
		unsigned char* imageData = stbi_load(image_path.generic_string().c_str(), &w, &h, &cn, 4);
		if (!imageData) return;

		auto targetSize = ImVec2(ImGui::GetIO().DisplaySize.x / 3.f, ImGui::GetIO().DisplaySize.y / 3.f);
		auto newSize = ImVec2(w, h);

		while (targetSize.x < newSize.x || targetSize.y < newSize.y) {
			newSize.x -= 10;
			newSize.y -= 10;
		}

		stbir_resize_uint8(imageData, w, h, 0, imageData, newSize.x, newSize.y, 0, cn);
		w = newSize.x;
		h = newSize.y;

		int dataSize = 0;
		auto data = stbi_write_png_to_mem(imageData, 0, w, h, cn, &dataSize);

		PDIRECT3DTEXTURE9 texture;
		auto result = D3DXCreateTextureFromFileInMemory(render::get_device(), data, dataSize, &texture);
		if (result != S_OK) return;

		D3DSURFACE_DESC image_desc;
		texture->GetLevelDesc(0, &image_desc);
		BackgroundImageData = texture;
		ImageSizes = { (float)image_desc.Width, (float)image_desc.Height };

		stbi_image_free(imageData);
		free(data);

		AlreadyUpdateImage = false;
		}).detach();
}

void menu::render_menu() {
	using namespace ImGui;

	GetMenuContext().fadeAnimation = GetMenuContext().state == EMenuState::Opened ? 
		ImMin(GetMenuContext().fadeAnimation + GetIO().DeltaTime * 2.f, 1.f) : ImMax(GetMenuContext().fadeAnimation - GetIO().DeltaTime * 2.f, 0.f);

	if (menu::MenuOpen()) {
		if (!interfaces::surface->is_cursor_visible() && !ImGui::GetIO().MouseDrawCursor) {
			GetIO().MouseDrawCursor = true;
		}
		PushStyleVar(ImGuiStyleVar_Alpha, GetMenuContext().fadeAnimation);

		PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.06f, 0.06f, 0.06f, 0.4f));
		PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });
		PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 0 });
		PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, { 5, 5 });

		SetNextWindowSize(GetIO().DisplaySize);
		SetNextWindowPos({ 0, 0 });
		Begin("##TABS", (bool*)0, ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoDecoration
				| ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings);

		for (auto& i : Tabs()) {
			TabSelector(i.name.c_str(), &i.state);
		}

		if (BackgroundImageData) {
			PushStyleVar(ImGuiStyleVar_Alpha, ImMin(ImageAlpha, GetMenuContext().fadeAnimation));
			SetCursorPos(GetWindowSize() - ImageSizes);
			Image(BackgroundImageData, ImageSizes);
			PopStyleVar();
		}

		End();

		if (BackgroundImageData) {
			Begin("YOU ARE GAY");
			SliderFloat("Fuck me...", &ImageAlpha, 0.f, 1.f);
			if (Button("Update##BGUPDATE") && std::filesystem::exists((env::get_data_path() / "bg.png"))) {
				LoadBackgroundImage();
			}
			End();
		}

		PopStyleColor();
		PopStyleVar(4);

		for (auto& i : Tabs()) {
			if (i.state || i.animationState) {
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImMin(i.animationState, GetMenuContext().fadeAnimation));
				i.drawFunction();
				ImGui::PopStyleVar();
			}

			i.animationState = i.state ? ImMin(i.animationState + GetIO().DeltaTime * 2.f, 1.f) : ImMax(i.animationState - GetIO().DeltaTime * 2.f, 0.f);
		}

		if (!BackgroundImageData && std::filesystem::exists((env::get_data_path() / "bg.png"))) {
			LoadBackgroundImage();
		}
	} else if (GetIO().MouseDrawCursor) {
		GetIO().MouseDrawCursor = false;
	}
}

hooks::hook_t<void(__stdcall*)(i_surface*)> lock_cursor_hook([](i_surface* self) {
	if (menu::MenuOpen())
		return self->unlock_cursor();
	return lock_cursor_hook.original(self);
});

void menu::InitializeMenu() {
  	static int OpenMenuKey = VK_INSERT;
	
	lock_cursor_hook.hook(interfaces::surface.get_virtual_table()[66]);

	input::add_handler({"MenuOpen", &OpenMenuKey, [&](input::EKeyState state) {
		if (state == input::EKeyState::Released)
			GetMenuContext().state = GetMenuContext().state == EMenuState::Closed ? EMenuState::Opened : EMenuState::Closed;
		}}
	);
}

void menu::AddElementToCategory(EMenuCategory category, std::shared_ptr<MenuElement> element) {
	for (auto& i : Tabs()) {
		if (i.tabContent.tabID == category) {
			i.tabContent.elements.emplace_back(element);
			std::sort(i.tabContent.elements.begin(), i.tabContent.elements.end(), [](decltype(i.tabContent.elements)::value_type& v1,
				decltype(i.tabContent.elements)::value_type& v2) { return v1->GetId() < v2->GetId(); });
		}
	}
}

decltype(MenuTabContent::elements)& menu::GetElementsForCategory(EMenuCategory category) {
	for (auto& i : Tabs()) {
		if (i.tabContent.tabID == category) {
			return i.tabContent.elements;
		}
	}
}
