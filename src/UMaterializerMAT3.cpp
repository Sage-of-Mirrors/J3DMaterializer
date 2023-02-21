#include "UMaterializerMAT3.hpp"
#include "J3D/J3DMaterial.hpp"
#include "J3D/J3DNameTable.hpp"
#include "J3D/J3DUtil.hpp"

#include <bstream.h>

#include <algorithm>

UMaterializerMAT3::UMaterializerMAT3() {
    mCullModeSet.Add(EGXCullMode::Back);
    mCullModeSet.Add(EGXCullMode::Front);
    mCullModeSet.Add(EGXCullMode::None);

    mZCompLocSet.Add(true);
    mZCompLocSet.Add(false);
    mDitherSet.Add(true);
    mDitherSet.Add(false);
}

void UMaterializerMAT3::WriteMaterial(bStream::CFileStream* outputStream, std::shared_ptr<J3DMaterial> material) {
    outputStream->writeUInt8(static_cast<uint8_t>(material->PEMode));
    outputStream->writeUInt8(static_cast<uint8_t>(mCullModeSet.Add(material->LightBlock.mCullMode)));
    outputStream->writeUInt8(static_cast<uint8_t>(mColorChannelCountSet.Add(material->LightBlock.mColorChannels.size())));
    outputStream->writeUInt8(static_cast<uint8_t>(mTexGenCountSet.Add(material->TexGenBlock.mTexCoordInfo.size())));
    outputStream->writeUInt8(static_cast<uint8_t>(mTevStageCountSet.Add(material->TevBlock->mTevStages.size())));
    outputStream->writeUInt8(static_cast<uint8_t>(mZCompLocSet.Add(material->PEBlock.mZCompLoc)));
    outputStream->writeUInt8(static_cast<uint8_t>(mZModeSet.Add(material->PEBlock.mZMode)));
    outputStream->writeUInt8(static_cast<uint8_t>(mDitherSet.Add(material->PEBlock.mDither)));

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

void UMaterializerMAT3::WriteIndirectData(bStream::CFileStream* outputStream, std::shared_ptr<J3DMaterial> material) {
    std::shared_ptr<J3DIndirectBlock> ind = material->IndirectBlock;
    
    outputStream->writeUInt8(ind->mEnabled);
    outputStream->writeUInt8(ind->mNumStages);
    outputStream->writeUInt16(UINT16_MAX);

    J3DIndirectTexOrderInfo dummyTexOrder;
    J3DIndirectTexMatrixInfo dummyTexMatrix;
    J3DIndirectTexScaleInfo dummyTexScale;
    J3DIndirectTevStageInfo dummyTevStage;

    for (int i = 0; i < 4; i++) {
        if (i >= ind->mIndirectTexOrders.size()) {
            dummyTexOrder.Serialize(outputStream);
        }
        else {
            ind->mIndirectTexOrders[i]->Serialize(outputStream);
        }
    }

    for (int i = 0; i < 3; i++) {
        if (i >= ind->mIndirectTexMatrices.size()) {
            dummyTexMatrix.Serialize(outputStream);
        }
        else {
            ind->mIndirectTexMatrices[i]->Serialize(outputStream);
        }
    }

    for (int i = 0; i < 4; i++) {
        if (i >= ind->mIndirectTexCoordScales.size()) {
            dummyTexScale.Serialize(outputStream);
        }
        else {
            ind->mIndirectTexCoordScales[i]->Serialize(outputStream);
        }
    }

    for (int i = 0; i < 16; i++) {
        if (i >= ind->mIndirectTevStages.size()) {
            dummyTevStage.Serialize(outputStream);
        }
        else {
            ind->mIndirectTevStages[i]->Serialize(outputStream);
        }
    }
}

void UMaterializerMAT3::WriteOffset(bStream::CFileStream* outputStream, size_t sectionStart, size_t offset) {
    size_t streamSize = outputStream->tell();
    outputStream->seek(sectionStart + offset);
    outputStream->writeUInt32(streamSize - sectionStart);
    outputStream->seek(streamSize);
}

void UMaterializerMAT3::WriteData(bStream::CFileStream* outputStream, const std::vector<std::shared_ptr<J3DMaterial>> materials) {
    size_t sectionStart = outputStream->tell();
    size_t streamSize = sectionStart;

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

    J3DNameTable nametable;

    for (std::shared_ptr<J3DMaterial> mat : materials) {
        WriteMaterial(outputStream, mat);
        nametable.AddName(mat->Name);
    }

    WriteOffset(outputStream, sectionStart, 0x10);

    // TODO: Write instance table
    for (int i = 0; i < materials.size(); i++) {
        outputStream->writeUInt16(i);
    }

    WriteOffset(outputStream, sectionStart, 0x14);

    nametable.Serialize(outputStream);

    WriteOffset(outputStream, sectionStart, 0x18);

    for (std::shared_ptr<J3DMaterial> mat : materials) {
        WriteIndirectData(outputStream, mat);
    }

    WriteOffset(outputStream, sectionStart, 0x1C);

    for (EGXCullMode c : mCullModeSet) {
        outputStream->writeUInt32((uint32_t)c);
    }

    WriteOffset(outputStream, sectionStart, 0x20);

    for (glm::vec4 c : mMaterialColorSet) {
        outputStream->writeUInt8(static_cast<uint8_t>(c.r * 255));
        outputStream->writeUInt8(static_cast<uint8_t>(c.g * 255));
        outputStream->writeUInt8(static_cast<uint8_t>(c.b * 255));
        outputStream->writeUInt8(static_cast<uint8_t>(c.a * 255));
    }

    WriteOffset(outputStream, sectionStart, 0x24);

    for (uint8_t c : mColorChannelCountSet) {
        outputStream->writeUInt8(c);
    }

    J3DUtility::PadStreamWithString(outputStream, 4);
    WriteOffset(outputStream, sectionStart, 0x28);

    for (J3DColorChannel c : mColorChannelSet) {
        c.Serialize(outputStream);
    }

    WriteOffset(outputStream, sectionStart, 0x2C);

    for (glm::vec4 c : mAmbientColorSet) {
        outputStream->writeUInt8(static_cast<uint8_t>(c.r * 255));
        outputStream->writeUInt8(static_cast<uint8_t>(c.g * 255));
        outputStream->writeUInt8(static_cast<uint8_t>(c.b * 255));
        outputStream->writeUInt8(static_cast<uint8_t>(c.a * 255));
    }

    WriteOffset(outputStream, sectionStart, 0x30); // Light data offset; lights in materials not supported
    WriteOffset(outputStream, sectionStart, 0x34);

    for (uint8_t c : mTexGenCountSet) {
        outputStream->writeUInt8(c);
    }

    J3DUtility::PadStreamWithString(outputStream, 4);
    WriteOffset(outputStream, sectionStart, 0x38);

    for (J3DTexCoordInfo c : mTexCoordSet) {
        c.Serialize(outputStream);
    }

    WriteOffset(outputStream, sectionStart, 0x40);

    for (J3DTexMatrixInfo c : mTexMatrixSet) {
        c.Serialize(outputStream);
    }

    WriteOffset(outputStream, sectionStart, 0x48);

    for (uint16_t c : mTextureIndexSet) {
        outputStream->writeUInt16(c);
    }

    J3DUtility::PadStreamWithString(outputStream, 4);
    WriteOffset(outputStream, sectionStart, 0x4C);

    for (J3DTevOrderInfo c : mTevOrderSet) {
        c.Serialize(outputStream);
    }

    WriteOffset(outputStream, sectionStart, 0x50);

    for (glm::vec4 c : mTevColorSet) {
        outputStream->writeUInt16(static_cast<uint16_t>(c.r));
        outputStream->writeUInt16(static_cast<uint16_t>(c.g));
        outputStream->writeUInt16(static_cast<uint16_t>(c.b));
        outputStream->writeUInt16(static_cast<uint16_t>(c.a));
    }

    WriteOffset(outputStream, sectionStart, 0x54);

    for (glm::vec4 c : mKonstColorSet) {
        outputStream->writeUInt8(static_cast<uint8_t>(c.r * 255));
        outputStream->writeUInt8(static_cast<uint8_t>(c.g * 255));
        outputStream->writeUInt8(static_cast<uint8_t>(c.b * 255));
        outputStream->writeUInt8(static_cast<uint8_t>(c.a * 255));
    }

    WriteOffset(outputStream, sectionStart, 0x58);

    for (uint8_t c : mTevStageCountSet) {
        outputStream->writeUInt8(c);
    }

    J3DUtility::PadStreamWithString(outputStream, 4);
    WriteOffset(outputStream, sectionStart, 0x5C);

    for (J3DTevStageInfo c : mTevStageSet) {
        c.Serialize(outputStream);
    }

    WriteOffset(outputStream, sectionStart, 0x60);

    for (J3DSwapModeInfo c : mSwapModeSet) {
        c.Serialize(outputStream);
    }

    WriteOffset(outputStream, sectionStart, 0x64);

    for (J3DSwapModeTableInfo c : mSwapModeTableSet) {
        c.Serialize(outputStream);
    }

    WriteOffset(outputStream, sectionStart, 0x68);

    for (J3DFog c : mFogSet) {
        c.Serialize(outputStream);
    }

    WriteOffset(outputStream, sectionStart, 0x6C);

    for (J3DAlphaCompare c : mAlphaCompareSet) {
        c.Serialize(outputStream);
    }

    WriteOffset(outputStream, sectionStart, 0x70);

    for (J3DBlendMode c : mBlendModeSet) {
        c.Serialize(outputStream);
    }

    WriteOffset(outputStream, sectionStart, 0x74);

    for (J3DZMode c : mZModeSet) {
        c.Serialize(outputStream);
    }

    WriteOffset(outputStream, sectionStart, 0x78);

    for (bool c : mZCompLocSet) {
        outputStream->writeUInt8(c);
    }

    J3DUtility::PadStreamWithString(outputStream, 4);
    WriteOffset(outputStream, sectionStart, 0x7C);

    for (bool c : mDitherSet) {
        outputStream->writeUInt8(c);
    }

    J3DUtility::PadStreamWithString(outputStream, 4);
    WriteOffset(outputStream, sectionStart, 0x80);

    for (J3DNBTScaleInfo c : mNBTScaleSet) {
        c.Serialize(outputStream);
    }

    J3DUtility::PadStreamWithString(outputStream, 16);

    // Update MAT3 size
    outputStream->seek(sectionStart + 4);
    outputStream->writeUInt32(outputStream->getSize() - sectionStart);

    outputStream->seek(outputStream->getSize(), false);
}
