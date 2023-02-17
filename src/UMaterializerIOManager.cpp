#include "UMaterializerIOManager.hpp"
#include "UMaterializerMAT3.hpp"

#include "J3D/J3DBlock.hpp"
#include "J3D/J3DMaterial.hpp"

#include <bstream.h>

UMaterializerIOManager::UMaterializerIOManager(bStream::CFileStream* inputStream) {
    if (inputStream == nullptr) {
        return;
    }

    // Grab section count for failsafe reasons
    inputStream->seek(0);
    uint32_t numSections = inputStream->peekUInt32(0x0C);

    // Copy model header into the buffer
    for (int i = 0; i < 0x20; i++) {
        mUnmodifiedData.push_back(inputStream->readUInt8());
    }

    for (int i = 0; i < numSections; i++) {
        // Check if we've found MAT3. TODO: MAT1? MAT2?
        if ((EJ3DBlockType)inputStream->peekUInt32(inputStream->tell()) == EJ3DBlockType::MAT3) {
            break;
        }

        // Copy section into buffer
        uint32_t sectionSize = inputStream->peekUInt32(inputStream->tell() + 4);
        for (int j = 0; j < sectionSize; j++) {
            mUnmodifiedData.push_back(inputStream->readUInt8());
        }
    }
}

void UMaterializerIOManager::WriteData(bStream::CFileStream* outputStream, const std::vector<std::shared_ptr<J3DMaterial>> materials) {
    if (outputStream == nullptr) {
        return;
    }

    outputStream->writeBytes(mUnmodifiedData.data(), mUnmodifiedData.size());

    UMaterializerMAT3 mat3;
    mat3.WriteData(outputStream, materials);
}
