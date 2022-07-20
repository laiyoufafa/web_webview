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

#ifndef PASTEBOARD_CLIENT_ADAPTER_H
#define PASTEBOARD_CLIENT_ADAPTER_H

#include <memory>
#include <vector>

namespace OHOS::NWeb {
class PasteDataRecordAdapter;
class PasteDataAdapter;
using PasteRecordList = std::vector<std::shared_ptr<PasteDataRecordAdapter>>;
class PasteBoardClientAdapter {
public:
    PasteBoardClientAdapter() = default;

    virtual ~PasteBoardClientAdapter() = default;

    virtual bool GetPasteData(PasteRecordList& data) = 0;

    virtual void SetPasteData(const PasteRecordList& data) = 0;

    virtual bool HasPasteData() = 0;

    virtual void Clear() = 0;
};

class PasteDataRecordAdapter {
public:
    PasteDataRecordAdapter() = default;

    virtual ~PasteDataRecordAdapter() = default;

    static std::shared_ptr<PasteDataRecordAdapter> NewRecord(
        const std::string& mimeType,
        std::shared_ptr<std::string> htmlText,
        std::shared_ptr<std::string> plainText);

    virtual std::string GetMimeType()= 0;

    virtual std::shared_ptr<std::string> GetHtmlText() = 0;

    virtual std::shared_ptr<std::string> GetPlainText() = 0;
};

class PasteDataAdapter {
public:
    PasteDataAdapter() = default;

    virtual ~PasteDataAdapter() = default;

    virtual void AddHtmlRecord(const std::string &html) = 0;

    virtual void AddTextRecord(const std::string &text) = 0;

    virtual std::vector<std::string> GetMimeTypes() = 0;
    
    virtual std::shared_ptr<std::string> GetPrimaryHtml() = 0;
    
    virtual std::shared_ptr<std::string> GetPrimaryText() = 0;
    
    virtual std::shared_ptr<std::string> GetPrimaryMimeType() = 0;

    virtual std::shared_ptr<PasteDataRecordAdapter> GetRecordAt(std::size_t index) = 0;
    
    virtual std::size_t GetRecordCount() = 0;

    virtual PasteRecordList AllRecords() const = 0;
};
}
#endif