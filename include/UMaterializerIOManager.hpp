#pragma once

#include <memory>
#include <vector>

namespace bStream { class CFileStream; }
class J3DMaterial;

class UMaterializerIOManager {
    std::vector<char> mUnmodifiedData;

public:
    UMaterializerIOManager(bStream::CFileStream* inputStream);

    void WriteData(bStream::CFileStream* outputStream, const std::vector<std::shared_ptr<J3DMaterial>> materials);
};
