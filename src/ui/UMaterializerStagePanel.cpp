#include "ui/UMaterializerStagePanel.hpp"

#include "UUIUtil.hpp"
#include <J3D/J3DMaterial.hpp>
#include <imgui.h>

UMaterializerStagePanel::UMaterializerStagePanel(const std::string name, std::shared_ptr<J3DTevBlock> block, uint32_t index)
    : UMaterializerUIPanel(name), mBlock(block), mIndex(index) {

}

void UMaterializerStagePanel::RenderContents(float deltaTime) {
    // Make sure block is valid (I assume it always will be, but best to be sure)
    if (mBlock.expired()) {
        ImGui::Text("Current TEV block is invalid!");

        bIsOpen = false;
        return;
    }

    std::shared_ptr<J3DTevBlock> block = mBlock.lock();

    // Make sure our assigned TEV stage is valid
    if (mIndex >= block->mTevStages.size() || block->mTevStages[mIndex].get() == nullptr) {
        ImGui::Text("Current TEV stage is invalid!");

        bIsOpen = false;
        return;
    }

    UIUtil::RenderComboEnum<EGXKonstColorSel>("Konst Color Select", block->mKonstColorSelection[mIndex]);
    UIUtil::RenderComboEnum<EGXKonstAlphaSel>("Konst Alpha Select", block->mKonstAlphaSelection[mIndex]);

    ImGui::Spacing();

    std::shared_ptr<J3DTevOrderInfo> tevOrder = block->mTevOrders[mIndex];

    UIUtil::RenderComboEnum<EGXTexCoordSlot>("Tex Coord Gen", tevOrder->TexCoordId);
    int texMap = tevOrder->TexMap;
    if (ImGui::InputInt("Tex Map", &texMap)) {
        if (texMap < 0) {
            texMap = 0;
        }
        else if (texMap >= block->mTextureIndices.size()) {
            texMap = block->mTextureIndices.size() - 1;
        }

        tevOrder->TexMap = texMap;
    }

    ImGui::Spacing();

    UIUtil::RenderComboEnum<EGXColorChannelId>("Color Channel", tevOrder->ChannelId);

    ImGui::Spacing();

    if (ImGui::CollapsingHeader("Color Swizzling")) {
        ImGui::Indent();

        UIUtil::RenderComboEnum<EGXSwapMode>("Tex Swizzle R", tevOrder->mTexSwapMode.R);
        UIUtil::RenderComboEnum<EGXSwapMode>("Tex Swizzle G", tevOrder->mTexSwapMode.G);
        UIUtil::RenderComboEnum<EGXSwapMode>("Tex Swizzle B", tevOrder->mTexSwapMode.B);
        UIUtil::RenderComboEnum<EGXSwapMode>("Tex Swizzle A", tevOrder->mTexSwapMode.A);

        ImGui::Spacing();

        UIUtil::RenderComboEnum<EGXSwapMode>("Ras Swizzle R", tevOrder->mRasSwapMode.R);
        UIUtil::RenderComboEnum<EGXSwapMode>("Ras Swizzle G", tevOrder->mRasSwapMode.G);
        UIUtil::RenderComboEnum<EGXSwapMode>("Ras Swizzle B", tevOrder->mRasSwapMode.B);
        UIUtil::RenderComboEnum<EGXSwapMode>("Ras Swizzle A", tevOrder->mRasSwapMode.A);

        ImGui::Unindent();
    }

    ImGui::Spacing();

    std::shared_ptr<J3DTevStageInfo> stage = block->mTevStages[mIndex];

    // Color combiner UI
    if (ImGui::CollapsingHeader("Color Combiner")) {
        ImGui::PushID("color");

        ImGui::Indent();

        if (ImGui::CollapsingHeader("Inputs")) {
            ImGui::Indent();

            UIUtil::RenderComboEnum<EGXCombineColorInput>("A", stage->ColorInput[0]);
            UIUtil::RenderComboEnum<EGXCombineColorInput>("B", stage->ColorInput[1]);
            UIUtil::RenderComboEnum<EGXCombineColorInput>("C", stage->ColorInput[2]);
            UIUtil::RenderComboEnum<EGXCombineColorInput>("D", stage->ColorInput[3]);

            ImGui::Unindent();
        }

        ImGui::Spacing();

        UIUtil::RenderComboEnum<EGXTevOp>("Operation", stage->ColorOperation);
        UIUtil::RenderComboEnum<EGXTevBias>("Bias", stage->ColorBias);
        UIUtil::RenderComboEnum<EGXTevScale>("Scale", stage->ColorScale);

        ImGui::Spacing();

        ImGui::Checkbox("Clamp", &stage->ColorClamp);

        ImGui::Spacing();

        UIUtil::RenderComboEnum<EGXTevRegister>("Output Register", stage->ColorOutputRegister);
        
        ImGui::Unindent();

        ImGui::PopID();
    }

    ImGui::Spacing();

    // Alpha combiner UI
    if (ImGui::CollapsingHeader("Alpha Combiner")) {
        ImGui::PushID("alpha");

        ImGui::Indent();

        if (ImGui::CollapsingHeader("Inputs")) {
            ImGui::Indent();

            UIUtil::RenderComboEnum<EGXCombineAlphaInput>("A", stage->AlphaInput[0]);
            UIUtil::RenderComboEnum<EGXCombineAlphaInput>("B", stage->AlphaInput[1]);
            UIUtil::RenderComboEnum<EGXCombineAlphaInput>("C", stage->AlphaInput[2]);
            UIUtil::RenderComboEnum<EGXCombineAlphaInput>("D", stage->AlphaInput[3]);

            ImGui::Unindent();
        }

        ImGui::Spacing();

        UIUtil::RenderComboEnum<EGXTevOp>("Operation", stage->AlphaOperation);
        UIUtil::RenderComboEnum<EGXTevBias>("Bias", stage->AlphaBias);
        UIUtil::RenderComboEnum<EGXTevScale>("Scale", stage->AlphaScale);

        ImGui::Spacing();

        ImGui::Checkbox("Clamp", &stage->AlphaClamp);

        ImGui::Spacing();

        UIUtil::RenderComboEnum<EGXTevRegister>("Output Register", stage->AlphaOutputRegister);

        ImGui::Unindent();

        ImGui::PopID();
    }
}
