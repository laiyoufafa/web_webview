/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef OHOS_RESOURCE_ADAPTER_IMPL_H
#define OHOS_RESOURCE_ADAPTER_IMPL_H

#include "extractor.h"
#include "file_mapper.h"
#include "ohos_resource_adapter.h"

namespace OHOS::NWeb {
class OhosFileMapperImpl : public OhosFileMapper {
public:
    OhosFileMapperImpl(std::unique_ptr<OHOS::AbilityBase::FileMapper> fileMap,
        const std::shared_ptr<OHOS::AbilityBase::Extractor>& extractor);

    ~OhosFileMapperImpl() override = default;

    int32_t GetFd() const override;

    int32_t GetOffset() const override;

    std::string GetFileName() const override;

    bool IsCompressed() const override;

    void* GetDataPtr() const override;

    size_t GetDataLen() const override;

    bool UnzipData(std::unique_ptr<uint8_t[]>& dest, size_t& len) override;

private:
    std::shared_ptr<OHOS::AbilityBase::Extractor> extractor_ = nullptr;

    std::unique_ptr<OHOS::AbilityBase::FileMapper> fileMap_ = nullptr;
};

class OhosResourceAdapterImpl : public OhosResourceAdapter {
public:
    explicit OhosResourceAdapterImpl(const std::string& hapPath);

    ~OhosResourceAdapterImpl() override = default;

    bool GetRawFileData(const std::string& rawFile, size_t& len,
        std::unique_ptr<uint8_t[]>& dest, bool isSys = false) override;

    bool GetRawFileMapper(const std::string& rawFile, std::unique_ptr<OhosFileMapper>& dest,
        bool isSys = false) override;

    bool IsRawFileExist(const std::string& rawFile, bool isSys = false) override;

    bool GetRawFileLastModTime(const std::string& rawFile,
        uint16_t& date, uint16_t& time, bool isSys = false) override;

    bool GetRawFileLastModTime(const std::string& rawFile, time_t& time, bool isSys = false) override;

private:
    void Init(const std::string& hapPath);
    
    static bool GetRawFileData(const std::shared_ptr<OHOS::AbilityBase::Extractor>& manager,
        const std::string& rawFile, size_t& len, std::unique_ptr<uint8_t[]>& dest);

    static bool HasEntry(const std::shared_ptr<OHOS::AbilityBase::Extractor>& manager,
        const std::string& rawFile);

    static bool GetFileInfo(const std::shared_ptr<OHOS::AbilityBase::Extractor>& manager,
        const std::string& rawFile, OHOS::AbilityBase::FileInfo& info);

    static bool GetRawFileMapper(const std::shared_ptr<OHOS::AbilityBase::Extractor>& manager,
        const std::string& rawFile, std::unique_ptr<OhosFileMapper>& dest);

    std::shared_ptr<OHOS::AbilityBase::Extractor> sysExtractor_;
    std::shared_ptr<OHOS::AbilityBase::Extractor> extractor_;
};
}  // namespace OHOS::NWeb

#endif  // OHOS_RESOURCE_ADAPTER_IMPL_H