#pragma once

#include <string>

class UMaterializerUIPanel {
protected:
    std::string mName;
    bool bIsOpen{ false };

    virtual void RenderContents(float deltaTime) = 0;

public:
    UMaterializerUIPanel(const std::string name);

    virtual void Render(float deltaTime);

    std::string GetName() const { return mName; }
    bool IsOpen() const { return bIsOpen; }
};
