#include "ui/UMaterializerStagePanel.hpp"

#include <imgui.h>

UMaterializerUIPanel::UMaterializerUIPanel(const std::string name) : mName(name) {

}

void UMaterializerUIPanel::Render(float deltaTime) {
    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_AlwaysAutoResize;
    ImGui::SetNextWindowSizeConstraints(ImVec2(400, 0), ImVec2(900, 900));

    ImGui::Begin(mName.data(), nullptr, windowFlags);
    RenderContents(deltaTime);
    ImGui::End();
}
