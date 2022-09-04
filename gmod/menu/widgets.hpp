#pragma once

#define IMGUI_DEFINE_MATH_OPERATORS

#include <imgui.h>
#include <imgui_internal.h>

#include <iostream>
#include <algorithm>
#include <map>
#include <mutex>
#include <string>
#include <variant>
#include <functional>
#include <string>
#include <string_view>
#include "../globals.hpp"

#include "keys_names.hpp"

namespace menu {

    inline void TabSelector(std::string_view name, bool* isActive) {
        using namespace ImGui;
        static std::map<std::string, float> animations;

        auto symbolSize = CalcTextSize("A");
        auto textSize = CalcTextSize(name.data());

        Dummy({ textSize.x + GetStyle().ItemInnerSpacing.x * 2.f + 20.f * animations[name.data()], textSize.y + GetStyle().ItemInnerSpacing.y * 2.f });

        auto backgroundColor = *isActive ? ImGui::GetColorU32(ImGuiCol_Header) : ImGui::GetColorU32(ImGuiCol_HeaderActive);
        RenderFrame(GetItemRectMin(), GetItemRectMax(), backgroundColor);
        RenderText({ GetItemRectMax().x - GetStyle().ItemInnerSpacing.x - textSize.x, GetItemRectMin().y + textSize.y / 2.f - GetStyle().ItemInnerSpacing.y }, name.data());

        if (IsItemClicked())
            *isActive = !*isActive;

        if (*isActive)
            animations[name.data()] = ImMin(animations[name.data()] + GetIO().DeltaTime, 1.f);
        else
            animations[name.data()] = ImMax(animations[name.data()] - GetIO().DeltaTime, 0.f);
    }

    inline void ToggleButton(std::string_view name, bool* var, const ImVec2& size_arg = {}) {
        using namespace ImGui;
        static std::map<std::string, float> animations;

        auto labelSize = CalcTextSize(name.data(), 0, true);
        ImVec2 size = CalcItemSize(size_arg, /*(labelSize.x + GetStyle().FramePadding.x * 2.0f) * 4.f*/ GetIO().DisplaySize.x / 6.f, labelSize.y + GetStyle().FramePadding.y * 2.0f);

        if (InvisibleButton(name.data(), size))
            *var = !*var;

        auto animationState = animations[name.data()];
        auto rectMaxAdd = ImMax(GetItemRectSize().x * animationState, 0.f);

        if (animationState > 0.f) {
            GetWindowDrawList()->PushClipRect(GetItemRectMin(), GetItemRectMax());
            GetWindowDrawList()->AddRectFilled({ GetItemRectMin().x + 1.f, GetItemRectMin().y + 1.f },
                { GetItemRectMin().x + rectMaxAdd - 1.f, GetItemRectMax().y - 1.f }, GetColorU32(ImGuiCol_ButtonActive), 5.f);
            GetWindowDrawList()->PopClipRect();
        }

        RenderText({ GetItemRectMax().x - GetItemRectSize().x / 2.f - labelSize.x / 2.f, GetItemRectMin().y + GetItemRectSize().y / 2.f - labelSize.y / 2.f }, name.data());

        PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.f);
        RenderFrameBorder(GetItemRectMin(), GetItemRectMax(), 10.f);
        PopStyleVar();

        animations[name.data()] = *var ? ImMin(animations[name.data()] + GetIO().DeltaTime, 1.f) : ImMax(animations[name.data()] - GetIO().DeltaTime, 0.f);
    }

    inline bool Hotkey(std::string_view name, int* key, const ImVec2& size_arg = {}) {
        using namespace ImGui;
        static std::map<std::string, bool> map;

        auto labelSize = CalcTextSize(name.data(), 0, true);
        ImVec2 size = CalcItemSize(size_arg, GetIO().DisplaySize.x / 6.f, labelSize.y + GetStyle().FramePadding.y * 2.0f);
        InvisibleButton(name.data(), size);

        std::string viewText;
        bool& isInChange = map[name.data()];
        auto newKey = *key;
        bool valueWasChanged = false;

        if (GetIO().MouseClicked[0] && !IsItemHovered()) {
            isInChange = false;
            ClearActiveID();
        }

        if (IsItemClicked() && !isInChange) {
            isInChange = true;
            GetIO().MouseDown[0] = false;
        }

        constexpr int mouseButtons[] = { VK_LBUTTON, VK_RBUTTON, VK_MBUTTON, VK_XBUTTON1, VK_XBUTTON2 };
        if (isInChange) {
            for (auto i = 0; i < 5; ++i) 
                if (GetIO().MouseDown[i])
                    newKey = mouseButtons[i], valueWasChanged = true, ClearActiveID();

            if (!valueWasChanged) 
                for (auto i = VK_BACK; i <= VK_RMENU; ++i)
                    if (GetIO().KeysDown[i])
                        newKey = i, valueWasChanged = true, ClearActiveID();
        }

        if (IsKeyPressedMap(ImGuiKey_Escape)) {
            isInChange = false;
            ClearActiveID();
        } else if (valueWasChanged) {
            *key = newKey;
            isInChange = false;

            SetItemDefaultFocus();
        }

        viewText = isInChange ? "..." : KeyNames[newKey];
        auto viewTextSize = CalcTextSize(viewText.c_str());

        RenderFrame(GetItemRectMin(), GetItemRectMax(), GetColorU32(ImGuiCol_Button), true, GetStyle().FrameRounding);

        const ImRect clipRect = {GetItemRectMin(), GetItemRectMax()};
        RenderTextClipped(GetItemRectMin() + GetStyle().FramePadding, GetItemRectMax() - GetStyle().FramePadding, viewText.c_str(), 0, 0, GetStyle().ButtonTextAlign, &clipRect);

        return valueWasChanged;
    }

    enum EMenuCategory {
        EMenuCategory_AimBot = 0,
        EMenuCategory_Esp,
        EMenuCategory_Misc,
        EMenuCategory_Settings,
        EMenuCategory_Last = EMenuCategory_Settings
    };

    class ElementWithPopup {
        std::function<void()> popupFunction;
    public:
        void SetPopupFunction(decltype(popupFunction) fn) { popupFunction = fn; }
        auto& GetPopupFunction() { return popupFunction; }
    };

    class MenuElement {
        std::vector<MenuElement*> children;
        std::string id;
        MenuElement* parent;
        std::function<void()> popupFunction;

    public:
        MenuElement(std::string_view id, MenuElement* parent = 0) : id(id), parent(parent) {}
        virtual void Render() = 0;

        auto& GetChildren() { return children; }
        auto& GetId() { return id; }
        auto* GetParent() { return parent; }
    };

    class ToggleButtonElement : public MenuElement, public ElementWithPopup {
        bool* target;
    public:
        ToggleButtonElement(std::string_view id, bool* target, MenuElement* parent = 0) : MenuElement(id, parent), target(target) {}

        void Render() override {
            ToggleButton(GetId(), target);

            if (GetPopupFunction()) {
                if (ImGui::BeginPopupContextItem(NULL, ImGuiPopupFlags_MouseButtonRight)) {
                    GetPopupFunction()();
                    ImGui::EndPopup();
                }
            }
        }
    };

    struct MenuTabContent {
        EMenuCategory tabID;
        std::vector<std::shared_ptr<MenuElement>> elements;
    };

    struct TabData_t {
        TabData_t(std::string name, std::function<void()> drawFunction, EMenuCategory category)
            : name(name), drawFunction(drawFunction) {
            tabContent.tabID = category;
        }

        MenuTabContent tabContent;
        std::string name;
        bool state = false;
        std::function<void()> drawFunction;
        float animationState = 0.f;
    };

    inline void TabHeader(std::string_view name) {
        auto headerTextSize = ImGui::CalcTextSize(name.data());
        ImGui::SetCursorPosX(ImGui::GetWindowSize().x * 0.5f - headerTextSize.x * 0.5f);
        ImGui::Text(name.data());
        ImGui::Separator();
    }
}