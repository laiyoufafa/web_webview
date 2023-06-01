/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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

#include "web_clipboard_controller.h"

#include <regex>

namespace {
const std::string IMG_TAG_PATTERN = "<img.*?data-ohos=.*?>";
const std::string IMG_TAG_SRC_PATTERN = "src='(.*?)'";
const std::string IMG_TAG_SRC_HEAD = "src=\"";
const std::string IMG_LOCAL_URI = "file:///";
const std::string IMG_LOCAL_PATH = "://";
constexpr int FOUR_BYTES = 4;
constexpr int EIGHT_BIT = 8;

struct Cmp {
    bool operator()(const int& lhs, const int& rhs) const
    {
        return lhs > rhs;
    }
};
} // namespace

namespace OHOS {
namespace NWeb {

// static
WebClipboardController& WebClipboardController::GetInstance()
{
    static WebClipboardController instance;
    return instance;
}

std::shared_ptr<MiscServices::PasteData> WebClipboardController::SplitHtml(std::shared_ptr<std::string> html) noexcept
{
    std::vector<std::pair<std::string, int>> matchVec = SplitHtmlWithImgLabel(html);
    std::map<std::string, std::vector<uint8_t>> imgSrcMap = SplitHtmlWithImgSrcLabel(matchVec);
    std::shared_ptr<MiscServices::PasteData> pasteData = BuildPasteData(html, imgSrcMap);
    return pasteData;
}

std::shared_ptr<std::string> WebClipboardController::RebuildHtml(
    std::shared_ptr<MiscServices::PasteData> pasteData) noexcept
{
    std::vector<std::shared_ptr<MiscServices::PasteDataRecord>> pasteDataRecords = pasteData->AllRecords();
    std::shared_ptr<std::string> htmlData;
    std::map<int, std::pair<std::string, std::string>, Cmp> replaceUris;

    for (auto& item : pasteDataRecords) {
        std::shared_ptr<std::string> html = item->GetHtmlText();
        if (html) {
            htmlData = html;
        }
        std::shared_ptr<OHOS::Uri> uri = item->GetUri();
        std::shared_ptr<MiscServices::MineCustomData> customData = item->GetCustomData();
        if (!uri || !customData) {
            continue;
        }
        std::map<std::string, std::vector<uint8_t>> customItemData = customData->GetItemData();
        for (auto& itemData : customItemData) {
            for (auto i = 0; i < itemData.second.size(); i += FOUR_BYTES) {
                int offset = static_cast<int>(itemData.second[i]) | static_cast<int>(itemData.second[i + 1] << 8) |
                             static_cast<int>(itemData.second[i + 2] << 16) |
                             static_cast<int>(itemData.second[i + 3] << 24);
                replaceUris[offset] = std::make_pair(uri->ToString(), itemData.first);
            }
        }
    }

    RemoveAllRecord(pasteData);
    for (auto& replaceUri : replaceUris) {
        htmlData->replace(replaceUri.first, replaceUri.second.second.size(), replaceUri.second.first);
    }
    pasteData->AddHtmlRecord(*htmlData);
    return htmlData;
}

std::vector<std::pair<std::string, int>> WebClipboardController::SplitHtmlWithImgLabel(
    const std::shared_ptr<std::string> html) noexcept
{
    std::smatch results;
    std::string pattern(IMG_TAG_PATTERN);
    std::regex r(pattern);
    std::string::const_iterator iterStart = html->begin();
    std::string::const_iterator iterEnd = html->end();
    std::vector<std::pair<std::string, int>> matchVec;

    while (std::regex_search(iterStart, iterEnd, results, r)) {
        std::string tmp = results[0];
        iterStart = results[0].second;
        int offset = static_cast<int>(iterStart - html->begin()) - tmp.size();

        matchVec.emplace_back(std::make_pair(tmp, offset));
    }

    return matchVec;
}

std::map<std::string, std::vector<uint8_t>> WebClipboardController::SplitHtmlWithImgSrcLabel(
    const std::vector<std::pair<std::string, int>>& matchVec) noexcept
{
    std::map<std::string, std::vector<uint8_t>> res;
    std::smatch match;
    std::regex re(IMG_TAG_SRC_PATTERN);
    for (auto& node : matchVec) {
        std::string::const_iterator iterStart = node.first.begin();
        std::string::const_iterator iterEnd = node.first.end();

        while (std::regex_search(iterStart, iterEnd, match, re)) {
            std::string tmp = match[0];
            iterStart = match[0].second;
            int offset = static_cast<int>(iterStart - node.first.begin()) - tmp.size();
            tmp = tmp.substr(IMG_TAG_SRC_HEAD.size());
            tmp.pop_back();
            if (!IsLocalURI(tmp)) {
                continue;
            }
            offset += IMG_TAG_SRC_HEAD.size() + node.second;
            for (int i = 0; i < FOUR_BYTES; i++) {
                res[tmp].emplace_back((offset >> (EIGHT_BIT * i)) & 0xff);
            }
        }
    }
    return res;
}

std::shared_ptr<MiscServices::PasteData> WebClipboardController::BuildPasteData(
    std::shared_ptr<std::string> html, const std::map<std::string, std::vector<uint8_t>>& imgSrcMap) noexcept
{
    std::shared_ptr<MiscServices::PasteData> pasteData = std::make_shared<MiscServices::PasteData>();
    pasteData->AddHtmlRecord(*html);
    for (auto& item : imgSrcMap) {
        MiscServices::PasteDataRecord::Builder builder(MiscServices::MIMETYPE_TEXT_URI);
        auto uri = std::make_shared<OHOS::Uri>(item.first);
        builder.SetUri(uri);
        auto customData = std::make_shared<MiscServices::MineCustomData>();

        customData->AddItemData(item.first, item.second);
        builder.SetCustomData(customData);
        auto record = builder.Build();
        pasteData->AddRecord(record);
    }
    return pasteData;
}

void WebClipboardController::RemoveAllRecord(std::shared_ptr<MiscServices::PasteData> pasteData) noexcept
{
    std::size_t recordCount = pasteData->GetRecordCount();
    for (auto i = 0; i < recordCount; i++) {
        pasteData->RemoveRecordAt(i);
    }
}

bool WebClipboardController::IsLocalURI(std::string& uri) noexcept
{
    return uri.substr(0, IMG_LOCAL_URI.size()) == IMG_LOCAL_URI || uri.find(IMG_LOCAL_PATH) == std::string::npos;
}
} // namespace NWeb
} // namespace OHOS
