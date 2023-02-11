#include "ui/UMaterializerStagePanel.hpp"

#include "UUIUtil.hpp"
#include <J3D/J3DMaterialData.hpp>
#include <imgui.h>

UMaterializerStagePanel::UMaterializerStagePanel(const std::string name) : UMaterializerUIPanel(name), mCurrentStage(nullptr) {

}

void UMaterializerStagePanel::RenderContents(float deltaTime) {
    if (mCurrentStage.get() == nullptr) {
        ImGui::Text("Current TEV stage is invalid!");
        return;
    }

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
    ImGui::BeginChild("stageSelect", ImVec2(0, 260), true, window_flags);

    for (int i = 0; i < mTevStages.size(); i++) {
        char a[16];
        std::snprintf(a, 16, "Stage %i", i + 1);

        if (ImGui::Selectable(a, false)) {
            mCurrentStage = mTevStages[i];
        }
    }

    ImGui::EndChild();
    ImGui::PopStyleVar();

    // Color combiner UI
    if (ImGui::CollapsingHeader("Color Combiner")) {
        ImGui::Indent();
        ImGui::Text("Inputs");
        ImGui::Indent();

        ImGui::PushID("color");
        UIUtil::RenderComboEnum<EGXCombineColorInput>("A", mCurrentStage->ColorInput[0]);
        UIUtil::RenderComboEnum<EGXCombineColorInput>("B", mCurrentStage->ColorInput[1]);
        UIUtil::RenderComboEnum<EGXCombineColorInput>("C", mCurrentStage->ColorInput[2]);
        UIUtil::RenderComboEnum<EGXCombineColorInput>("D", mCurrentStage->ColorInput[3]);

        ImGui::Unindent();
        ImGui::Separator();

        UIUtil::RenderComboEnum<EGXTevOp>("Operation", mCurrentStage->ColorOperation);
        UIUtil::RenderComboEnum<EGXTevBias>("Bias", mCurrentStage->ColorBias);
        UIUtil::RenderComboEnum<EGXTevScale>("Scale", mCurrentStage->ColorScale);
        ImGui::Checkbox("Clamp", &mCurrentStage->ColorClamp);
        UIUtil::RenderComboEnum<EGXTevRegister>("Output Register", mCurrentStage->ColorOutputRegister);
        ImGui::PopID();

        ImGui::Unindent();
    }

    ImGui::Separator();

    // Alpha combiner UI
    if (ImGui::CollapsingHeader("Alpha Combiner")) {
        ImGui::Indent();
        ImGui::Text("Inputs");
        ImGui::Indent();

        ImGui::PushID("alpha");
        UIUtil::RenderComboEnum<EGXCombineAlphaInput>("A", mCurrentStage->AlphaInput[0]);
        UIUtil::RenderComboEnum<EGXCombineAlphaInput>("B", mCurrentStage->AlphaInput[1]);
        UIUtil::RenderComboEnum<EGXCombineAlphaInput>("C", mCurrentStage->AlphaInput[2]);
        UIUtil::RenderComboEnum<EGXCombineAlphaInput>("D", mCurrentStage->AlphaInput[3]);

        ImGui::Unindent();
        ImGui::Separator();

        UIUtil::RenderComboEnum<EGXTevOp>("Operation", mCurrentStage->AlphaOperation);
        UIUtil::RenderComboEnum<EGXTevBias>("Bias", mCurrentStage->AlphaBias);
        UIUtil::RenderComboEnum<EGXTevScale>("Scale", mCurrentStage->AlphaScale);
        ImGui::Checkbox("Clamp", &mCurrentStage->AlphaClamp);
        UIUtil::RenderComboEnum<EGXTevRegister>("Output Register", mCurrentStage->AlphaOutputRegister);
        ImGui::PopID();

        ImGui::Unindent();
    }
}
