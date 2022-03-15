#pragma once

#include "widgets.hpp"
#include "../globals.hpp"

#include <imgui_stdlib.h>

#include <fstream>

void AimBotTab() {
    using namespace ImGui;

    Begin("AimBot##SKEETCRACK", 0, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize);

    TabHeader("AimBot");

    End();
}

void EspTab() {
    using namespace ImGui;

    Begin("ESP##SKEETCRACK", 0, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize);

    TabHeader("ESP");

    ToggleButton("ESP##ESPENABLED", settings::GetVariablePointer<bool>("Esp"));
    if (BeginPopupContextItem(NULL, ImGuiPopupFlags_MouseButtonRight)) {
        BeginGroup();
        CheckboxFlags("Box##ESPBOX", settings::GetVariablePointer<int>("EspVisualSettings"), settings::EVisualSettings_Box);
        CheckboxFlags("Health bar##ESPBOX", settings::GetVariablePointer<int>("EspVisualSettings"), settings::EVisualSettings_HealthBar);
        CheckboxFlags("Name##ESPBOX", settings::GetVariablePointer<int>("EspVisualSettings"), settings::EVisualSettings_Name);
        CheckboxFlags("Team name##ESPBOX", settings::GetVariablePointer<int>("EspVisualSettings"), settings::EVisualSettings_TeamName);
        CheckboxFlags("User group##ESPBOX", settings::GetVariablePointer<int>("EspVisualSettings"), settings::EVisualSettings_UserGroup);
        EndGroup();
        EndPopup();
    }

    End();
}

void MiscTab() {
    using namespace ImGui;

    Begin("Misc##SKEETCRACK", 0, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize);

    TabHeader("Misc");

    ToggleButton("BunnyHop##BHOPENABLED", settings::GetVariablePointer<bool>("BunnyHop"));

    Separator();
    ToggleButton("Information HUD##INFOHUD", settings::GetVariablePointer<bool>("InformationHUD"));
    ToggleButton("Observers HUD##INFOHUD", settings::GetVariablePointer<bool>("ObserversHUD"));

    End();
}

void SaveSettingsToFile(const std::string& fileName) {
    std::ofstream stream(env::get_data_path() / (fileName + ".vpcfg"));
    settings::SaveSettingsToStream(stream);
}
void LoadSettingsFromFile(const std::string& fileName) {
    std::ifstream stream(env::get_data_path() / (fileName + ".vpcfg"));
    settings::LoadSettingsFromStream(stream);
}
bool SaveExists(const std::string& fileName) {
    return std::filesystem::exists(env::get_data_path() / (fileName + ".vpcfg"));
}
std::string remove_extension(const std::string& filename) {
    size_t lastdot = filename.find_last_of(".");
    if (lastdot == std::string::npos) return filename;
    return filename.substr(0, lastdot);
}

void SettingsTab() {
    using namespace ImGui;

    Begin("Settings##SKEETCRACK", 0, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize);
    static std::string saveName;
    static float configSavedAnimation;

    TabHeader("Settings");

    configSavedAnimation = ImMax(configSavedAnimation - GetIO().DeltaTime * 2.f, 0.f);

    if (BeginListBox("##SettingsFileList", { GetIO().DisplaySize.x / 6.f, 150 })) {
        for (const auto& entry : std::filesystem::directory_iterator(env::get_data_path())) {
            if (entry.is_regular_file()) {
                if (entry.path().extension() == ".vpcfg") {
                    auto filename = remove_extension(entry.path().filename().string());
                    if (Selectable(std::string(filename).append("##SELECTABLEFILE").c_str(), filename == saveName))
                        saveName = filename;
                    if (filename == saveName)
                        SetItemDefaultFocus();
                    if (BeginPopupContextItem(NULL, ImGuiPopupFlags_MouseButtonRight)) {
                        if (Button("Save##SAVESETTINGS", { GetIO().DisplaySize.x / 8.f - GetStyle().ItemSpacing.x, 0.f })) {
                            SaveSettingsToFile(filename);
                            notifymanager::AddNotify(std::format("\"{}\" was saved", filename));
                        }
                        if (Button("Load##LOADSAVE", { GetIO().DisplaySize.x / 8.f - GetStyle().ItemSpacing.x, 0.f })) {
                            LoadSettingsFromFile(filename);
                            notifymanager::AddNotify(std::format("\"{}\" was loaded", filename));
                        }
                        if (Button("Remove##REMOVESAVE", { GetIO().DisplaySize.x / 8.f - GetStyle().ItemSpacing.x, 0.f })) {
                            std::filesystem::remove(env::get_data_path() / (filename + ".vpcfg"));
                            notifymanager::AddNotify(std::format("\"{}\" was removed", filename));
                        }
                        EndPopup();
                    }
                }
            }
        }

        EndListBox();
    }

    PushItemWidth(GetIO().DisplaySize.x / 6.f);
    InputText("##SAVESAVE", &saveName);
    PopItemWidth();

    BeginGroup();
    if (Button("Save##SAVESETTINGS", { GetIO().DisplaySize.x / 8.f - GetStyle().ItemSpacing.x, 0.f})) {
        SaveSettingsToFile(saveName);
        notifymanager::AddNotify(std::format("\"{}\" was saved", saveName));
    }
    SameLine();

    if (!SaveExists(saveName)) {
        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
    }
    if (Button("Load##LOADSAVE", { GetIO().DisplaySize.x / 6.f - GetItemRectSize().x - GetStyle().ItemSpacing.x, 0})) {
        LoadSettingsFromFile(saveName);
        notifymanager::AddNotify(std::format("\"{}\" was loaded", saveName));
    }
    if (!SaveExists(saveName)) {
        ImGui::PopItemFlag();
        ImGui::PopStyleVar();
    }
    EndGroup();

    if (Button("Open directory##OPENCONFIGDIRECTORY", { GetIO().DisplaySize.x / 6.f, 0.f}))
        ShellExecuteW(NULL, NULL, env::get_data_path().c_str(), NULL, NULL, SW_SHOWNORMAL);

    End();
}

static inline std::vector<TabData_t> Tabs = {
	{"AimBot", AimBotTab},
	{"Esp", EspTab},
	{"Misc", MiscTab},
	{"Settings", SettingsTab}
};