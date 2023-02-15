#pragma once

#include "UMaterializerUIPanel.hpp"

#include <memory>
#include <vector>

struct J3DColorChannel;

class UMaterializerColorChannelPanel : public UMaterializerUIPanel {
    std::weak_ptr<J3DColorChannel> mChannel;

protected:
    virtual void RenderContents(float deltaTime) override;

public:
    UMaterializerColorChannelPanel(const std::string name, std::shared_ptr<J3DColorChannel> channel);
};