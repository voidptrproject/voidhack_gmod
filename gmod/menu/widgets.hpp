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

struct TabData_t {
    TabData_t(std::string name, std::function<void()> drawFunction)
        : name(name), drawFunction(drawFunction) {
    }

    std::string name;
    bool state = false;
    std::function<void()> drawFunction;
    float animationState = 1.f;
};