#include "UMaterializerMAT3.hpp"
#include "J3D/J3DMaterial.hpp"

#include <bstream.h>

#include <algorithm>

UMaterializerMAT3::UMaterializerMAT3() {

}

void UMaterializerMAT3::CollateMaterialData(std::shared_ptr<J3DMaterial> material) {

}

void UMaterializerMAT3::CollatePixelEngineBlock(std::shared_ptr<J3DMaterial> material) {
}

void UMaterializerMAT3::WriteMaterial(bStream::CFileStream* outputStream, std::shared_ptr<J3DMaterial> material) {
    outputStream->writeUInt8(static_cast<uint8_t>(material->PEMode));
    outputStream->writeUInt8(static_cast<uint8_t>(mCullModeSet.Add(material->LightBlock.mCullMode)));
    outputStream->writeUInt8(static_cast<uint8_t>(mColorChannelCountSet.Add(material->LightBlock.mColorChannels.size())));
    outputStream->writeUInt8(static_cast<uint8_t>(mTexGenCountSet.Add(material->TexGenBlock.mTexCoordInfo.size())));
    outputStream->writeUInt8(static_cast<uint8_t>(mTevStageCountSet.Add(material->TevBlock->mTevStages.size())));
    outputStream->writeUInt8(static_cast<uint8_t>(material->PEBlock.mZCompLoc));
    outputStream->writeUInt8(static_cast<uint8_t>(mZModeSet.Add(material->PEBlock.mZMode)));
    outputStream->writeUInt8(static_cast<uint8_t>(material->PEBlock.mDither));

    // Material color indices
    outputStream->writeUInt16(static_cast<uint16_t>(mMaterialColorSet.Add(material->LightBlock.mMatteColor[0])));
    outputStream->writeUInt16(static_cast<uint16_t>(mMaterialColorSet.Add(material->LightBlock.mMatteColor[1])));

    // Color channel indices
    for (int i = 0; i < 4; i++) {
        uint16_t index = i >= material->LightBlock.mColorChannels.size() ? i : mColorChannelSet.Add(*material->LightBlock.mColorChannels[i]);
        outputStream->writeUInt16(index);
    }

    // Ambient color indices
    outputStream->writeUInt16(static_cast<uint16_t>(mAmbientColorSet.Add(material->LightBlock.mAmbientColor[0])));
    outputStream->writeUInt16(static_cast<uint16_t>(mAmbientColorSet.Add(material->LightBlock.mAmbientColor[1])));

    // Embedded light indices; not supported by Materializer
    for (int i = 0; i < 8; i++) {
        outputStream->writeUInt16(UINT16_MAX);
    }

    // TexCoord indices
    for (int i = 0; i < 8; i++) {
        uint16_t index = i >= material->TexGenBlock.mTexCoordInfo.size() ? UINT16_MAX : mTexCoordSet.Add(*material->TexGenBlock.mTexCoordInfo[i]);
        outputStream->writeUInt16(index);
    }

    // TexCoord2 indices
    for (int i = 0; i < 8; i++) {
        uint16_t index = i >= material->TexGenBlock.mTexCoord2Info.size() ? UINT16_MAX : mTexCoord2Set.Add(*material->TexGenBlock.mTexCoord2Info[i]);
        outputStream->writeUInt16(index);
    }

    // Tex matrix indices
    for (int i = 0; i < 10; i++) {
        uint16_t index = i >= material->TexGenBlock.mTexMatrix.size() ? UINT16_MAX : mTexMatrixSet.Add(*material->TexGenBlock.mTexMatrix[i]);
        outputStream->writeUInt16(index);
    }

    // Post tex matrix indices; no supported by Materializer
    for (int i = 0; i < 20; i++) {
        outputStream->writeUInt16(UINT16_MAX);
    }

    // Texture indices
    for (int i = 0; i < 8; i++) {
        uint16_t index = i >= material->TevBlock->mTextureIndices.size() ? UINT16_MAX : mTextureIndexSet.Add(material->TevBlock->mTextureIndices[i]);
        outputStream->writeUInt16(index);
    }

    // Konst color indices
    for (int i = 0; i < 4; i++) {
        outputStream->writeUInt16(static_cast<uint16_t>(mKonstColorSet.Add(material->TevBlock->mTevKonstColors[i])));
    }

    // Konst color selects
    for (int i = 0; i < 16; i++) {
        outputStream->writeUInt8(static_cast<uint8_t>(material->TevBlock->mKonstColorSelection[i]));
    }
    // Konst alpha selects
    for (int i = 0; i < 16; i++) {
        outputStream->writeUInt8(static_cast<uint8_t>(material->TevBlock->mKonstAlphaSelection[i]));
    }

    // TEV order indices
    for (int i = 0; i < 16; i++) {
        uint16_t index = i >= material->TevBlock->mTevOrders.size() ? UINT16_MAX : mTevOrderSet.Add(*material->TevBlock->mTevOrders[i]);
        outputStream->writeUInt16(index);
    }

    // TEV color indices
    for (int i = 0; i < 4; i++) {
        outputStream->writeUInt16(static_cast<uint16_t>(mTevColorSet.Add(material->TevBlock->mTevColors[i])));
    }

    // TEV stage indices
    for (int i = 0; i < 16; i++) {
        uint16_t index = i >= material->TevBlock->mTevStages.size() ? UINT16_MAX : mTevStageSet.Add(*material->TevBlock->mTevStages[i]);
        outputStream->writeUInt16(index);
    }

    // TEV swap mode table indices; we need to store these before writing to the stream
    std::vector<uint16_t> swapModeIndices;
    std::vector<uint16_t> swapModeTableIndices;
    for (int i = 0; i < material->TevBlock->mTevOrders.size(); i++) {
        uint16_t rasIndex = mSwapModeTableSet.Add(material->TevBlock->mTevOrders[i]->mRasSwapMode);
        uint16_t texIndex = mSwapModeTableSet.Add(material->TevBlock->mTevOrders[i]->mTexSwapMode);

        if (std::find(swapModeTableIndices.begin(), swapModeTableIndices.end(), rasIndex) == swapModeTableIndices.end()) {
            swapModeTableIndices.push_back(rasIndex);
        }

        if (std::find(swapModeTableIndices.begin(), swapModeTableIndices.end(), texIndex) == swapModeTableIndices.end()) {
            swapModeTableIndices.push_back(texIndex);
        }

        swapModeIndices.push_back(mSwapModeSet.Add(J3DSwapModeInfo(rasIndex, texIndex)));
    }

    // Fill in unused slots for swap mode indices
    for (int i = swapModeIndices.size(); i < 16; i++) {
        swapModeIndices.push_back(UINT16_MAX);
    }

    // Fill in unused slots for swap mode table indices
    for (int i = swapModeTableIndices.size(); i < 16; i++) {
        if (i <= 3) {
            swapModeTableIndices.push_back(0);
        }
        else {
            swapModeTableIndices.push_back(UINT16_MAX);
        }
    }

    // TEV swap mode indices
    for (uint16_t index : swapModeIndices) {
        outputStream->writeUInt16(index);
    }

    // TEV swap mode table indices
    for (uint16_t index : swapModeTableIndices) {
        outputStream->writeUInt16(index);
    }

    // Fog index
    outputStream->writeUInt16(static_cast<uint16_t>(mFogSet.Add(material->PEBlock.mFog)));

    // Alpha compare index
    outputStream->writeUInt16(static_cast<uint16_t>(mAlphaCompareSet.Add(material->PEBlock.mAlphaCompare)));

    // Blend mode index
    outputStream->writeUInt16(static_cast<uint16_t>(mBlendModeSet.Add(material->PEBlock.mBlendMode)));

    // NBT scale index
    outputStream->writeUInt16(static_cast<uint16_t>(mNBTScaleSet.Add(material->TexGenBlock.mNBTScale)));
}

void UMaterializerMAT3::WriteData(bStream::CFileStream* outputStream, const std::vector<std::shared_ptr<J3DMaterial>> materials) {
    size_t sectionStart = outputStream->tell();

    // MAT3 header
    outputStream->writeUInt32(0x4D415433); // 'MAT3'
    outputStream->writeUInt32(0); // Section size placeholder
    outputStream->writeUInt16(materials.size()); // Material count
    outputStream->writeInt16(-1);

    // Component offsets
    outputStream->writeUInt32(0x84); // Offset to material init data; always 0x84

    for (int i = 0; i < 0x1D; i++) {
        outputStream->writeUInt32(0); // Placeholder for offset at i
    }

    for (std::shared_ptr<J3DMaterial> mat : materials) {
        WriteMaterial(outputStream, mat);
    }

    // TODO: output components

    // Update MAT3 size
    outputStream->seek(sectionStart + 4);
    outputStream->writeUInt32(outputStream->getSize() - sectionStart);

    outputStream->seek(outputStream->getSize(), false);
}
