#include "ui/UMaterializerStagePanel.hpp"

#include "UUIUtil.hpp"
#include <J3D/J3DMaterial.hpp>
#include <imgui.h>

UMaterializerStagePanel::UMaterializerStagePanel(const std::string name, std::shared_ptr<J3DTevBlock> block, uint32_t index)
    : UMaterializerUIPanel(name), mBlock(block), mIndex(index) {

}

void UMaterializerStagePanel::RenderContents(float deltaTime) {
    if (mBlock.expired()) {
        ImGui::Text("Current TEV stage is invalid!");

        bShouldClose = true;
        return;
    }

    std::shared_ptr<J3DTevBlock> block = mBlock.lock();

    UIUtil::RenderComboEnum<EGXKonstColorSel>("Konst Color Select", block->mKonstColorSelection[mIndex]);
    UIUtil::RenderComboEnum<EGXKonstAlphaSel>("Konst Alpha Select", block->mKonstAlphaSelection[mIndex]);

    ImGui::Spacing();
    ImGui::Spacing();

    if (ImGui::CollapsingHeader("TEV Order")) {
        UIUtil::RenderComboEnum<EGXTexCoordSlot>("Tex Coord Gen", block->mTevOrders[mIndex].TexCoordId);
        int texMap = block->mTevOrders[mIndex].TexMap;
        if (ImGui::InputInt("Tex Map", &texMap)) {
            if (texMap < block->mTextureIndices.size()) {
                texMap = 0;
            }

            block->mTevOrders[mIndex].TexMap = texMap;
        }

        ImGui::Spacing();

        UIUtil::RenderComboEnum<EGXColorChannelId>("Color Channel", block->mTevOrders[mIndex].ChannelId);
    }

    ImGui::Spacing();
    ImGui::Spacing();

    std::shared_ptr<J3DTevStageInfo> stage = block->mTevStages[mIndex];

    // Color combiner UI
    if (ImGui::CollapsingHeader("Color Combiner")) {
        ImGui::PushID("color");

        if (ImGui::CollapsingHeader("Inputs")) {
            UIUtil::RenderComboEnum<EGXCombineColorInput>("A", stage->ColorInput[0]);
            UIUtil::RenderComboEnum<EGXCombineColorInput>("B", stage->ColorInput[1]);
            UIUtil::RenderComboEnum<EGXCombineColorInput>("C", stage->ColorInput[2]);
            UIUtil::RenderComboEnum<EGXCombineColorInput>("D", stage->ColorInput[3]);
        }

        ImGui::Spacing();
        ImGui::Spacing();

        UIUtil::RenderComboEnum<EGXTevOp>("Operation", stage->ColorOperation);
        UIUtil::RenderComboEnum<EGXTevBias>("Bias", stage->ColorBias);
        UIUtil::RenderComboEnum<EGXTevScale>("Scale", stage->ColorScale);

        ImGui::Spacing();

        ImGui::Checkbox("Clamp", &stage->ColorClamp);

        ImGui::Spacing();

        UIUtil::RenderComboEnum<EGXTevRegister>("Output Register", stage->ColorOutputRegister);
        
        ImGui::PopID();
    }

    ImGui::Spacing();
    ImGui::Spacing();

    // Alpha combiner UI
    if (ImGui::CollapsingHeader("Alpha Combiner")) {
        ImGui::Text("Inputs");

        ImGui::PushID("alpha");

        if (ImGui::CollapsingHeader("Inputs")) {
            UIUtil::RenderComboEnum<EGXCombineAlphaInput>("A", stage->AlphaInput[0]);
            UIUtil::RenderComboEnum<EGXCombineAlphaInput>("B", stage->AlphaInput[1]);
            UIUtil::RenderComboEnum<EGXCombineAlphaInput>("C", stage->AlphaInput[2]);
            UIUtil::RenderComboEnum<EGXCombineAlphaInput>("D", stage->AlphaInput[3]);
        }

        ImGui::Spacing();
        ImGui::Spacing();

        UIUtil::RenderComboEnum<EGXTevOp>("Operation", stage->AlphaOperation);
        UIUtil::RenderComboEnum<EGXTevBias>("Bias", stage->AlphaBias);
        UIUtil::RenderComboEnum<EGXTevScale>("Scale", stage->AlphaScale);

        ImGui::Spacing();

        ImGui::Checkbox("Clamp", &stage->AlphaClamp);

        ImGui::Spacing();

        UIUtil::RenderComboEnum<EGXTevRegister>("Output Register", stage->AlphaOutputRegister);

        ImGui::PopID();
    }
}
