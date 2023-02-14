#pragma once

#include "UMaterializerUIPanel.hpp"

#include <memory>
#include <vector>

struct J3DTexMatrixInfo;

class UMaterializerTexMatrixPanel : public UMaterializerUIPanel {
    std::weak_ptr<J3DTexMatrixInfo> mTexMatrix;

protected:
    virtual void RenderContents(float deltaTime) override;

public:
    UMaterializerTexMatrixPanel(const std::string name, std::shared_ptr<J3DTexMatrixInfo> texMAtrix);
};