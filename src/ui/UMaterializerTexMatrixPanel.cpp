#include "ui/UMaterializerTexMatrixPanel.hpp"

#include "UUIUtil.hpp"
#include <J3D/J3DMaterial.hpp>
#include <imgui.h>

UMaterializerTexMatrixPanel::UMaterializerTexMatrixPanel(const std::string name, std::shared_ptr<J3DTexMatrixInfo> texMtx)
    : UMaterializerUIPanel(name), mTexMatrix(texMtx) {

}

void UMaterializerTexMatrixPanel::RenderContents(float deltaTime) {
    if (mTexMatrix.expired()) {
        ImGui::Text("Current tex gen is invalid!");

        bIsOpen = false;
        return;
    }

    std::shared_ptr<J3DTexMatrixInfo> texMtx = mTexMatrix.lock();

    UIUtil::RenderComboEnum<EGXTexMatrixType>("Type", texMtx->Type);
    UIUtil::RenderComboEnum<EJ3DTexMatrixProjection>("Projection", texMtx->Projection);

    ImGui::Spacing();

    ImGui::InputFloat3("Origin", &texMtx->Origin.x);
    
    ImGui::Spacing();

    ImGui::InputFloat2("Scale", &texMtx->Transform.Scale.x);
    ImGui::InputFloat("Rotation", &texMtx->Transform.Rotation);
    ImGui::InputFloat2("Translation", &texMtx->Transform.Translation.x);
}
