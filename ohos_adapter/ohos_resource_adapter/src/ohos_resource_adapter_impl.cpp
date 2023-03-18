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
#include <unistd.h>

#include "bundle_mgr_proxy.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "nweb_log.h"
#include "system_ability_definition.h"

using namespace OHOS::AbilityBase;

namespace {
const std::string NWEB_HAP_PATH = "/system/app/com.ohos.nweb/NWeb.hap";
const std::string NWEB_HAP_PATH_1 = "/system/app/NWeb/NWeb.hap";
const std::string NWEB_BUNDLE_NAME = "com.ohos.nweb";
const std::string NWEB_PACKAGE = "entry";
constexpr uint32_t TM_YEAR_BITS = 9;
constexpr uint32_t TM_MON_BITS = 5;
constexpr uint32_t TM_MIN_BITS = 5;
constexpr uint32_t TM_HOUR_BITS = 11;
constexpr uint32_t START_YEAR = 1900;
} // namespace

namespace OHOS::NWeb {
namespace {
sptr<OHOS::AppExecFwk::BundleMgrProxy> GetBundleMgrProxy()
{
    auto systemAbilityMgr = OHOS::SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!systemAbilityMgr) {
        WVLOG_E("fail to get system ability mgr.");
        return nullptr;
    }
    auto remoteObject = systemAbilityMgr->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    if (!remoteObject) {
        WVLOG_E("fail to get bundle manager proxy.");
        return nullptr;
    }
    WVLOG_D("get bundle manager proxy success.");
    return iface_cast<OHOS::AppExecFwk::BundleMgrProxy>(remoteObject);
}

std::string GetNWebHapPath()
{
    auto iBundleMgr = GetBundleMgrProxy();
    if (iBundleMgr) {
        OHOS::AppExecFwk::AbilityInfo abilityInfo;
        OHOS::AppExecFwk::HapModuleInfo hapModuleInfo;
        abilityInfo.bundleName = NWEB_BUNDLE_NAME;
        abilityInfo.package = NWEB_PACKAGE;
        if (iBundleMgr->GetHapModuleInfo(abilityInfo, hapModuleInfo)) {
            WVLOG_D("get hap module info success. %{public}s", hapModuleInfo.hapPath.c_str());
            return hapModuleInfo.hapPath;
        }
    }
    if (access(NWEB_HAP_PATH.c_str(), F_OK) == 0) {
        WVLOG_D("eixt NWEB_HAP_PATH");
        return NWEB_HAP_PATH;
    }
    if (access(NWEB_HAP_PATH_1.c_str(), F_OK) == 0) {
        WVLOG_D("eixt NWEB_HAP_PATH_1");
        return NWEB_HAP_PATH_1;
    }
    WVLOG_E("get nweb hap path failed.");
    return "";
}
} // namespace

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
    std::string nwebHapPath = GetNWebHapPath();
    if (!nwebHapPath.empty()) {
        sysExtractor_ = ExtractorUtil::GetExtractor(nwebHapPath, newCreate);
        if (!sysExtractor_) {
            WVLOG_E("RuntimeExtractor create failed for %{public}s", nwebHapPath.c_str());
        }
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

std::string OhosResourceAdapterImpl::GetModuleName(const char *configStr, size_t len)
{
    if (configStr == nullptr) {
        return std::string();
    }
    std::string config(configStr, len);
    static const char *key = "\"moduleName\"";
    auto idx = config.find(key);
    if (idx == std::string::npos) {
        return std::string();
    }
    auto start = config.find("\"", idx + strlen(key));
    if (start == std::string::npos) {
        return std::string();
    }
    auto end = config.find("\"", start + 1);
    if (end == std::string::npos) {
        return std::string();
    }

    std::string retStr = std::string(configStr + start + 1, end - start - 1);
    return retStr;
}

std::string OhosResourceAdapterImpl::ParseModuleName(const std::shared_ptr<Extractor> &manager)
{
    if (manager == nullptr) {
        return std::string();
    }
    std::unique_ptr<uint8_t[]> configBuf;
    size_t len;
    bool ret = manager->ExtractToBufByName("config.json", configBuf, len);
    if (!ret) {
        WVLOG_E("failed to get config data from ability");
        return std::string();
    }
    // parse config.json
    std::string mName = GetModuleName(reinterpret_cast<char *>(configBuf.get()), len);
    if (mName.size() == 0) {
        WVLOG_E("parse moduleName from config.json error");
        return std::string();
    }
    return mName;
}

bool OhosResourceAdapterImpl::GetRawFileData(const std::shared_ptr<Extractor>& manager,
    const std::string& rawFile, size_t& len, std::unique_ptr<uint8_t[]>& dest)
{
    if (!manager) {
        return false;
    }
    if (manager->IsStageModel()) {
        return manager->ExtractToBufByName(rawFile, dest, len);
    }
    std::string moduleName = OhosResourceAdapterImpl::ParseModuleName(manager);
    std::string rawFilePath("assets/");
    rawFilePath.append(moduleName);
    rawFilePath.append("/");
    rawFilePath.append(rawFile);
    WVLOG_E("fa filepath:%{public}s", rawFilePath.c_str());
    return manager->ExtractToBufByName(rawFilePath, dest, len);
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
