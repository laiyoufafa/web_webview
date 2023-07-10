/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include <cstring>
#include <cstdint>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <securec.h>
#include <thread>

#include "accesstoken_kit.h"
#include "nativetoken_kit.h"
#include "nweb_adapter_helper.h"
#include "token_setproc.h"

#define private public
#include "screen_capture_adapter_impl.h"
#undef private

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen;

namespace OHOS {
namespace NWeb {

namespace {
constexpr int32_t AUDIO_SAMPLE_RATE = 16000;
constexpr int32_t AUDIO_CHANNELS = 2;
constexpr int32_t SCREEN_WIDTH = 1080;
constexpr int32_t SCREEN_HEIGHT = 720;
constexpr int32_t SLEEP_FOR_MILLI_SECONDS_CNT = 500;
std::shared_ptr<ScreenCaptureAdapter> g_screenCapture = nullptr;

class ScreenCaptureCallbackAdapterTest : public ScreenCaptureCallbackAdapter {
public:
    ScreenCaptureCallbackAdapterTest() = default;
    ~ScreenCaptureCallbackAdapterTest() override = default;

    void OnError(int32_t errorCode) override
    {
        (void)errorCode;
    }

    void OnAudioBufferAvailable(bool isReady, AudioCaptureSourceTypeAdapter type) override
    {
        (void)isReady;
        (void)type;
    }

    void OnVideoBufferAvailable(bool isReady) override
    {
        if (!isReady || !g_screenCapture) {
            return;
        }
        std::unique_ptr<SurfaceBufferAdapter> buffer = g_screenCapture->AcquireVideoBuffer();
        if (buffer) {
            g_screenCapture->ReleaseVideoBuffer();
        }
    }
};
}

class ScreenCaptureAdapterImplTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void ScreenCaptureAdapterImplTest::SetUpTestCase(void)
{
    // set native token
    uint64_t tokenId;
    const char *perms[2];
    perms[0] = "ohos.permission.CAPTURE_SCREEN";
    perms[1] = "ohos.permission.MICROPHONE";
    NativeTokenInfoParams infoInstance = {
        .dcapsNum = 0,
        .permsNum = 1,
        .aclsNum = 0,
        .dcaps = NULL,
        .perms = perms,
        .acls = NULL,
        .processName = "web_screen_capture_tdd",
        .aplStr = "system_basic",
    };
    tokenId = GetAccessTokenId(&infoInstance);
    SetSelfTokenID(tokenId);
    OHOS::Security::AccessToken::AccessTokenKit::ReloadNativeTokenInfo();
    
    g_screenCapture = std::make_shared<ScreenCaptureAdapterImpl>();
    EXPECT_NE(g_screenCapture, nullptr);
    ScreenCaptureConfigAdapter config;
    config.captureMode = CaptureModeAdapter::CAPTURE_HOME_SCREEN;
    config.dataType = DataTypeAdapter::ORIGINAL_STREAM_DATA_TYPE;
    config.audioInfo.micCapInfo.audioSampleRate = AUDIO_SAMPLE_RATE;
    config.audioInfo.micCapInfo.audioChannels = AUDIO_CHANNELS;
    config.audioInfo.innerCapInfo.audioSampleRate = AUDIO_SAMPLE_RATE;
    config.audioInfo.innerCapInfo.audioChannels = AUDIO_CHANNELS;
    config.videoInfo.videoCapInfo.videoFrameWidth = SCREEN_WIDTH;
    config.videoInfo.videoCapInfo.videoFrameHeight = SCREEN_HEIGHT;
    int32_t result = g_screenCapture->Init(config);
    EXPECT_EQ(result, 0);
}

void ScreenCaptureAdapterImplTest::TearDownTestCase(void)
{}

void ScreenCaptureAdapterImplTest::SetUp(void)
{}

void ScreenCaptureAdapterImplTest::TearDown(void)
{}

/**
 * @tc.name: ScreenCaptureAdapterImplTest_Init_001
 * @tc.desc: Init.
 * @tc.type: FUNC
 * @tc.require: AR000I7I57
 */
HWTEST_F(ScreenCaptureAdapterImplTest, ScreenCaptureAdapterImplTest_Init_001, TestSize.Level1)
{
    auto adapterImpl = std::make_shared<ScreenCaptureAdapterImpl>();
    EXPECT_NE(adapterImpl, nullptr);
    ScreenCaptureConfigAdapter config;
    config.captureMode = CaptureModeAdapter::CAPTURE_INVAILD;
    config.dataType = DataTypeAdapter::ORIGINAL_STREAM_DATA_TYPE;
    config.audioInfo.micCapInfo.audioSampleRate = AUDIO_SAMPLE_RATE;
    config.audioInfo.micCapInfo.audioChannels = AUDIO_CHANNELS;
    config.audioInfo.innerCapInfo.audioSampleRate = AUDIO_SAMPLE_RATE;
    config.audioInfo.innerCapInfo.audioChannels = AUDIO_CHANNELS;
    config.videoInfo.videoCapInfo.videoFrameWidth = SCREEN_WIDTH;
    config.videoInfo.videoCapInfo.videoFrameHeight = SCREEN_HEIGHT;
    int32_t result = adapterImpl->Init(config);
    EXPECT_EQ(result, -1);
    config.captureMode = CaptureModeAdapter::CAPTURE_HOME_SCREEN;
    result = adapterImpl->Init(config);
    EXPECT_EQ(result, 0);
    result = adapterImpl->Init(config);
    EXPECT_EQ(result, 0);
}

/**
 * @tc.name: ScreenCaptureAdapterImplTest_SetMicrophoneEnable_002
 * @tc.desc: SetMicrophoneEnable.
 * @tc.type: FUNC
 * @tc.require: AR000I7I57
 */
HWTEST_F(ScreenCaptureAdapterImplTest, ScreenCaptureAdapterImplTest_SetMicrophoneEnable_002, TestSize.Level1)
{
    auto adapterImpl = std::make_shared<ScreenCaptureAdapterImpl>();
    EXPECT_NE(adapterImpl, nullptr);
    int32_t result = adapterImpl->SetMicrophoneEnable(false);
    EXPECT_EQ(result, -1);
    result = g_screenCapture->SetMicrophoneEnable(false);
    EXPECT_EQ(result, 0);
    result = g_screenCapture->SetMicrophoneEnable(true);
    EXPECT_EQ(result, 0);
}

/**
 * @tc.name: ScreenCaptureAdapterImplTest_AcquireVideoBuffer_003
 * @tc.desc: AcquireVideoBuffer.
 * @tc.type: FUNC
 * @tc.require: AR000I7I57
 */
HWTEST_F(ScreenCaptureAdapterImplTest, ScreenCaptureAdapterImplTest_AcquireVideoBuffer_003, TestSize.Level1)
{
    auto adapterImpl = std::make_shared<ScreenCaptureAdapterImpl>();
    EXPECT_NE(adapterImpl, nullptr);
    std::unique_ptr<SurfaceBufferAdapter> buffer = adapterImpl->AcquireVideoBuffer();
    EXPECT_EQ(buffer, nullptr);
    int32_t result = adapterImpl->ReleaseVideoBuffer();
    EXPECT_EQ(result, -1);
    g_screenCapture->AcquireVideoBuffer();
    g_screenCapture->ReleaseVideoBuffer();
}

/**
 * @tc.name: ScreenCaptureAdapterImplTest_Capture_004
 * @tc.desc: Capture.
 * @tc.type: FUNC
 * @tc.require: AR000I7I57
 */
HWTEST_F(ScreenCaptureAdapterImplTest, ScreenCaptureAdapterImplTest_Capture_004, TestSize.Level1)
{
    auto adapterImpl = std::make_shared<ScreenCaptureAdapterImpl>();
    EXPECT_NE(adapterImpl, nullptr);
    int32_t result = adapterImpl->StartCapture();
    EXPECT_EQ(result, -1);
    result = adapterImpl->StopCapture();
    EXPECT_EQ(result, -1);

    auto callbackAdapter = std::make_shared<ScreenCaptureCallbackAdapterTest>();
    EXPECT_NE(callbackAdapter, nullptr);
    result = g_screenCapture->SetCaptureCallback(callbackAdapter);
    EXPECT_EQ(result, 0);
    result = g_screenCapture->StartCapture();
    EXPECT_EQ(result, 0);
    std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_FOR_MILLI_SECONDS_CNT));
    result = g_screenCapture->StopCapture();
    EXPECT_EQ(result, 0);
}
}
} // namespace NWeb