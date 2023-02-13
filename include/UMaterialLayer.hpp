#pragma once

#include <vector>
#include <memory>

class J3DMaterial;

class UMaterialLayer {
    std::vector<std::shared_ptr<J3DMaterial>> mMaterials;

public:
    UMaterialLayer(std::vector<std::shared_ptr<J3DMaterial>> materials) { mMaterials = materials; }

    std::vector<std::shared_ptr<J3DMaterial>> GetMaterials() const { return mMaterials; }
};
