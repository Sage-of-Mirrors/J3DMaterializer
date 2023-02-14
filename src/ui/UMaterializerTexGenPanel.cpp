#include "ui/UMaterializerTexGenPanel.hpp"

#include "UUIUtil.hpp"
#include <J3D/J3DMaterial.hpp>
#include <imgui.h>

UMaterializerTexGenPanel::UMaterializerTexGenPanel(const std::string name, std::shared_ptr<J3DTexCoordInfo> texGen)
    : UMaterializerUIPanel(name), mTexGen(texGen) {

}

void UMaterializerTexGenPanel::RenderContents(float deltaTime) {
    if (mTexGen.expired()) {
        ImGui::Text("Current tex gen is invalid!");

        bIsOpen = false;
        return;
    }

    std::shared_ptr<J3DTexCoordInfo> texGen = mTexGen.lock();

    UIUtil::RenderComboEnum<EGXTexGenType>("Type", texGen->Type);
    UIUtil::RenderComboEnum<EGXTexGenSrc>("Source", texGen->Source);

    ImGui::Spacing();

    UIUtil::RenderComboEnum<EGXTexMatrix>("Matrix", texGen->TexMatrix);
}
