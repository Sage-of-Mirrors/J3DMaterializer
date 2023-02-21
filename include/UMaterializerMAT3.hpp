#pragma once

#include "util/UComponentSet.hpp"

#include <GX/GXEnum.hpp>
#include <glm/glm.hpp>

#include <vector>
#include <memory>

namespace bStream { class CFileStream; }
class J3DMaterial;

class UMaterializerMAT3 {
    UComponentSet<EGXCullMode> mCullModeSet;
    UComponentSet<uint8_t> mColorChannelCountSet;
    UComponentSet<uint8_t> mTexGenCountSet;
    UComponentSet<uint8_t> mTevStageCountSet;
    UComponentSet<struct J3DZMode> mZModeSet;
    UComponentSet<glm::vec4> mMaterialColorSet;
    UComponentSet<struct J3DColorChannel> mColorChannelSet;
    UComponentSet<glm::vec4> mAmbientColorSet;
    UComponentSet<struct J3DTexCoordInfo> mTexCoordSet;
    UComponentSet<struct J3DTexCoordInfo> mTexCoord2Set;
    UComponentSet<struct J3DTexMatrixInfo> mTexMatrixSet;
    UComponentSet<uint16_t> mTextureIndexSet;
    UComponentSet<glm::vec4> mKonstColorSet;
    UComponentSet<struct J3DTevOrderInfo> mTevOrderSet;
    UComponentSet<glm::vec4> mTevColorSet;
    UComponentSet<struct J3DTevStageInfo> mTevStageSet;
    UComponentSet<struct J3DSwapModeInfo> mSwapModeSet;
    UComponentSet<struct J3DSwapModeTableInfo> mSwapModeTableSet;
    UComponentSet<struct J3DFog> mFogSet;
    UComponentSet<struct J3DAlphaCompare> mAlphaCompareSet;
    UComponentSet<struct J3DBlendMode> mBlendModeSet;
    UComponentSet<bool> mZCompLocSet;
    UComponentSet<bool> mDitherSet;
    UComponentSet<struct J3DNBTScaleInfo> mNBTScaleSet;

    void WriteOffset(bStream::CFileStream* outputStream, size_t sectionStart, size_t offset);
    void WriteMaterial(bStream::CFileStream* outputStream, std::shared_ptr<J3DMaterial> material);
    void WriteIndirectData(bStream::CFileStream* outputStream, std::shared_ptr<J3DMaterial> material);

public:
    UMaterializerMAT3();

    void WriteData(bStream::CFileStream* outputStream, const std::vector<std::shared_ptr<J3DMaterial>> materials);
};
