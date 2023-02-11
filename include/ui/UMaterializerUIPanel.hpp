#pragma once

#include <string>

class UMaterializerUIPanel {
protected:
    std::string mName;

    virtual void RenderContents(float deltaTime) = 0;

public:
    UMaterializerUIPanel(const std::string name);

    virtual void Render(float deltaTime);
};
