#include "ui/UMaterializerStagePanel.hpp"

#include <imgui.h>

UMaterializerUIPanel::UMaterializerUIPanel(const std::string name) : mName(name), bIsOpen(true) {

}

void UMaterializerUIPanel::Render(float deltaTime) {
    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_AlwaysAutoResize;
    ImGui::SetNextWindowSizeConstraints(ImVec2(400, 0), ImVec2(900, 900));

    ImGui::Begin(mName.data(), &bIsOpen, windowFlags);
    RenderContents(deltaTime);
    ImGui::End();
}
