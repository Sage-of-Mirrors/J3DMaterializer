#pragma once

#include "UMaterializerUIPanel.hpp"

#include <memory>
#include <vector>

struct J3DTexCoordInfo;

class UMaterializerTexGenPanel : public UMaterializerUIPanel {
    std::weak_ptr<J3DTexCoordInfo> mTexGen;

protected:
    virtual void RenderContents(float deltaTime) override;

public:
    UMaterializerTexGenPanel(const std::string name, std::shared_ptr<J3DTexCoordInfo> texGen);
};