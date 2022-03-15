#pragma once

#include "memory.hpp"

#include <d3d9.h>
#include <functional>
#include <string_view>
#include <mutex>

#include <imgui.h>
#include <imgui_internal.h>

namespace render {
	struct render_handler;
	struct render_data_t;
	namespace internal {
		void render_hook(IDirect3DDevice9* device, const memory::address_t& return_address);
		void notify_render_handlers(render_data_t& context);

		struct render_context_t {
			ImDrawList* main_draw_list;
		};

		render_context_t& get_render_context();
	}

	struct render_data_t {
		ImDrawList* draw_list;
	};

	using render_function_t = std::function<void(render_data_t& context)>;
	void add_render_handler(render_function_t handler);
	
	IDirect3DDevice9* get_device();

	inline ImVec2 calculate_text_size(std::string_view text) { return ImGui::CalcTextSize(text.data()); }
}

namespace notifymanager {
	struct Notify_t {
		Notify_t(std::string_view name, std::string_view data) : name(name), data(data) {}

		std::string name, data;
		float animationState = 0.f;
		bool wasVisible = false;
	};

	class NotifyManager {
		std::vector<Notify_t> notifies;
		std::mutex notifyMutex;

		ImVec2 getNotifySize() { return { ImGui::GetIO().DisplaySize.x / 6.f, 20.f }; }
	public:
		NotifyManager() {}

		auto& GetMutex() { return notifyMutex; }
		void AddNotify(const Notify_t& notify) { notifies.push_back(notify); }

		void UpdateAndRender() {
			std::lock_guard g(notifyMutex);
			if (notifies.empty())
				return;

			notifies.erase(std::remove_if(notifies.begin(), notifies.end(), [](Notify_t& n) {
				return n.wasVisible && n.animationState == 0.f;
				}), notifies.end());

			ImVec2 prevNotifyPosition = { ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y - 10.f };
			for (auto notify = notifies.begin(); notify != notifies.end(); notify++) {
				if (!notify->wasVisible && notify->animationState == 2.f) {
					notify->wasVisible = true;
				}

				ImGui::SetNextWindowSize(getNotifySize());
				ImGui::SetNextWindowPos({ ImGui::GetIO().DisplaySize.x - getNotifySize().x * ImMin(notify->animationState, 1.f),
					prevNotifyPosition.y - getNotifySize().y * 1.5f });
				ImGui::Begin(std::string(notify->name).append("##NOTIFY").append(std::to_string((uintptr_t)notify._Ptr)).c_str(), 0, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs);

				ImGui::TextWrapped(notify->data.c_str());
				prevNotifyPosition = ImGui::GetWindowPos();

				ImGui::End();

				notify->animationState = notify->wasVisible ? ImMax(notify->animationState - ImGui::GetIO().DeltaTime * 2.f, 0.f)
					: ImMin(notify->animationState + ImGui::GetIO().DeltaTime * 2.f, 2.f);
			}
		}
	};

	NotifyManager& GetNotifyManager();
	void AddNotify(std::string_view data);

}