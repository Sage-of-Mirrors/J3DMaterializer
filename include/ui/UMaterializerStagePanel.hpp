#pragma once

#include "UMaterializerUIPanel.hpp"

#include <memory>
#include <vector>

struct J3DTevStageInfo;

class UMaterializerStagePanel : public UMaterializerUIPanel {
    std::vector<std::shared_ptr<J3DTevStageInfo>> mTevStages;
    std::shared_ptr<J3DTevStageInfo> mCurrentStage;

protected:
    virtual void RenderContents(float deltaTime) override;

public:
    UMaterializerStagePanel(const std::string name);

    void SetStageList(std::vector<std::shared_ptr<J3DTevStageInfo>> stages) {
        mTevStages = stages;
        mCurrentStage = mTevStages[0];
    }
};
