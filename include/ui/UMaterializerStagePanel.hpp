#pragma once

#include "UMaterializerUIPanel.hpp"

#include <memory>
#include <vector>

struct J3DTevBlock;

class UMaterializerStagePanel : public UMaterializerUIPanel {
    std::weak_ptr<J3DTevBlock> mBlock;
    uint32_t mIndex;

protected:
    virtual void RenderContents(float deltaTime) override;

public:
    UMaterializerStagePanel(const std::string name, std::shared_ptr<J3DTevBlock> block, uint32_t mIndex);
};
