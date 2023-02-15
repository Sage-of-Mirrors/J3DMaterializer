#include "ui/UMaterializerColorChannelPanel.hpp"

#include "UUIUtil.hpp"
#include <J3D/J3DMaterial.hpp>
#include <imgui.h>

UMaterializerColorChannelPanel::UMaterializerColorChannelPanel(const std::string name, std::shared_ptr<J3DColorChannel> channel)
    : UMaterializerUIPanel(name), mChannel(channel) {

}

void UMaterializerColorChannelPanel::RenderContents(float deltaTime) {
    if (mChannel.expired()) {
        ImGui::Text("Current color channel is invalid!");

        bIsOpen = false;
        return;
    }

    std::shared_ptr<J3DColorChannel> channel = mChannel.lock();

    UIUtil::RenderComboEnum<EGXColorSource>("Material Color Source", channel->MaterialSource);
    UIUtil::RenderComboEnum<EGXColorSource>("Ambient Color Source", channel->AmbientSource);

    ImGui::Spacing();

    if (ImGui::CollapsingHeader("Lighting")) {
        ImGui::Indent();

        ImGui::Checkbox("Lighting Enabled", &channel->LightingEnabled);

        ImGui::Spacing();

        if (channel->LightingEnabled) {
            UIUtil::RenderComboEnum<EGXDiffuseFunction>("Light Diffuse Function", channel->DiffuseFunction);
            UIUtil::RenderComboEnum<EGXAttenuationFunction>("Light Ambient Function", channel->AttenuationFunction);

            ImGui::Spacing();

            for (int i = 0; i < 8; i++) {
                char boxName[8];
                snprintf(boxName, 8, "Light %i", i);

                bool lightEnabled = channel->LightMask & (1 << i);
                ImGui::Checkbox(boxName, &lightEnabled);
            }
        }

        ImGui::Unindent();
    }
}
