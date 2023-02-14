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

#include "ohos_resource_adapter_impl.h"

#include <ctime>
#include <securec.h>
#include <sstream>

#include "nweb_log.h"

using namespace OHOS::AbilityBase;

namespace {
const std::string NWEB_HAP_PATH = "/system/app/com.ohos.nweb/NWeb.hap";
constexpr uint32_t TM_YEAR_BITS = 9;
constexpr uint32_t TM_MON_BITS = 5;
constexpr uint32_t TM_MIN_BITS = 5;
constexpr uint32_t TM_HOUR_BITS = 11;
constexpr uint32_t START_YEAR = 1900;
} // namespace

namespace OHOS::NWeb {

OhosFileMapperImpl::OhosFileMapperImpl(std::unique_ptr<OHOS::AbilityBase::FileMapper> fileMap,
    const std::shared_ptr<Extractor>& extractor): extractor_(extractor), fileMap_(std::move(fileMap))
{
}

int32_t OhosFileMapperImpl::GetFd() const
{
    return -1;
}

int32_t OhosFileMapperImpl::GetOffset() const
{
    return fileMap_ ? fileMap_->GetOffset(): -1;
}

std::string OhosFileMapperImpl::GetFileName() const
{
    return fileMap_ ? fileMap_->GetFileName(): "";
}

bool OhosFileMapperImpl::IsCompressed() const
{
    return fileMap_ ? fileMap_->IsCompressed(): false;
}

void* OhosFileMapperImpl::GetDataPtr() const
{
    return fileMap_ ? fileMap_->GetDataPtr(): nullptr;
}

size_t OhosFileMapperImpl::GetDataLen() const
{
    return fileMap_ ? fileMap_->GetDataLen(): 0;
}

bool OhosFileMapperImpl::UnzipData(std::unique_ptr<uint8_t[]>& dest, size_t& len)
{
    if (extractor_ && IsCompressed()) {
        return extractor_->UnzipData(std::move(fileMap_), dest, len);
    }
    return false;
}

OhosResourceAdapterImpl::OhosResourceAdapterImpl(const std::string& hapPath)
{
    Init(hapPath);
}

void OhosResourceAdapterImpl::Init(const std::string& hapPath)
{
    bool newCreate = false;
    sysExtractor_ = ExtractorUtil::GetExtractor(NWEB_HAP_PATH, newCreate);
    if (!sysExtractor_) {
        WVLOG_E("RuntimeExtractor create failed for %{public}s", NWEB_HAP_PATH.c_str());
    }
    if (hapPath.empty()) {
        return;
    }
    std::string loadPath = ExtractorUtil::GetLoadFilePath(hapPath);
    extractor_ = ExtractorUtil::GetExtractor(loadPath, newCreate);
    if (!extractor_) {
        WVLOG_E("RuntimeExtractor create failed for %{public}s", hapPath.c_str());
    }
}

bool OhosResourceAdapterImpl::GetRawFileData(const std::string& rawFile, size_t& len,
    std::unique_ptr<uint8_t[]>& dest, bool isSys)
{
    return GetRawFileData(isSys? sysExtractor_: extractor_, rawFile, len, dest);
}

bool OhosResourceAdapterImpl::GetRawFileMapper(const std::string& rawFile,
    std::unique_ptr<OhosFileMapper>& dest, bool isSys)
{
    return GetRawFileMapper(isSys? sysExtractor_: extractor_, rawFile, dest);
}

bool OhosResourceAdapterImpl::IsRawFileExist(const std::string& rawFile, bool isSys)
{
    return HasEntry(isSys? sysExtractor_: extractor_, rawFile);
}

bool OhosResourceAdapterImpl::GetRawFileLastModTime(const std::string& rawFile,
    uint16_t& date, uint16_t& time, bool isSys)
{
    FileInfo info;
    if (GetFileInfo(isSys? sysExtractor_: extractor_, rawFile, info)) {
        date = info.lastModDate;
        time = info.lastModTime;
        return true;
    }
    return false;
}

bool OhosResourceAdapterImpl::GetRawFileLastModTime(const std::string& rawFile, time_t& time, bool isSys)
{
    FileInfo info;
    if (GetFileInfo(isSys? sysExtractor_: extractor_, rawFile, info)) {
        uint16_t modifiedDate = info.lastModDate;
        uint16_t modifiedTime = info.lastModTime;
        struct tm newTime;
        newTime.tm_year = ((modifiedDate >> TM_YEAR_BITS) & 0x7f) + START_YEAR;
        newTime.tm_mon = (modifiedDate >> TM_MON_BITS) & 0xf;
        newTime.tm_mday = modifiedDate & 0x1f;
        newTime.tm_hour = (modifiedTime >> TM_HOUR_BITS) & 0x1f;
        newTime.tm_min = (modifiedTime >> TM_MIN_BITS) & 0x2f;
        newTime.tm_sec = (modifiedTime << 1) & 0x1f;
        newTime.tm_isdst = 0;
        time = mktime(&newTime);
        return true;
    }
    return false;
}

// static
bool OhosResourceAdapterImpl::HasEntry(const std::shared_ptr<OHOS::AbilityBase::Extractor>& manager,
    const std::string& rawFile)
{
    if (!manager) {
        return false;
    }
    return manager->HasEntry(rawFile);
}

bool OhosResourceAdapterImpl::GetFileInfo(const std::shared_ptr<OHOS::AbilityBase::Extractor>& manager,
    const std::string& rawFile, OHOS::AbilityBase::FileInfo& info)
{
    if (!manager) {
        return false;
    }
    return manager->GetFileInfo(rawFile, info);
}

bool OhosResourceAdapterImpl::GetRawFileData(const std::shared_ptr<Extractor>& manager,
    const std::string& rawFile, size_t& len, std::unique_ptr<uint8_t[]>& dest)
{
    if (!manager) {
        return false;
    }
    return manager->ExtractToBufByName(rawFile, dest, len);
}

bool OhosResourceAdapterImpl::GetRawFileMapper(const std::shared_ptr<OHOS::AbilityBase::Extractor>& manager,
    const std::string& rawFile, std::unique_ptr<OhosFileMapper>& dest)
{
    if (!manager) {
        return false;
    }
    std::unique_ptr<OHOS::AbilityBase::FileMapper> fileMap = manager->GetData(rawFile);
    if (fileMap == nullptr) {
        return false;
    }
    bool isCompressed = fileMap->IsCompressed();
    dest = std::make_unique<OhosFileMapperImpl>(std::move(fileMap), isCompressed ? manager: nullptr);
    return true;
}
}  // namespace OHOS::NWeb