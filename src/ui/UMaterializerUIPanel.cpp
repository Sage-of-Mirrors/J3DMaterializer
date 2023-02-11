#include "ui/UMaterializerStagePanel.hpp"

#include <imgui.h>

UMaterializerUIPanel::UMaterializerUIPanel(const std::string name) : mName(name) {

}

void UMaterializerUIPanel::Render(float deltaTime) {
    ImGui::Begin(mName.data());

    RenderContents(deltaTime);

    ImGui::End();
}
