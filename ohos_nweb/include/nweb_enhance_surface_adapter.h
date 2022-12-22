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

#ifndef NWEB_ENHANCE_SURFACE_ADAPTER_H
#define NWEB_ENHANCE_SURFACE_ADAPTER_H

#include <refbase.h>
#include <stdint.h>
#include <surface.h>

#include "nweb.h"
#include "surface_buffer.h"

namespace OHOS::NWeb {
class NWebEnhanceSurfaceAdapter {
public:
    static NWebEnhanceSurfaceAdapter &Instance();
    NWebCreateInfo GetCreateInfo(void *enhanceSurfaceInfo,
                                 const NWebInitArgs &initArgs,
                                 uint32_t width,
                                 uint32_t height);
private:
    void GetSize(NWebCreateInfo &createInfo, uint32_t width, uint32_t height) const;
    void GetRenderInterface(NWebCreateInfo &createInfo);
};
} // namespace OHOS::NWeb

#endif // NWEB_ENHANCE_SURFACE_ADAPTER_H