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

#ifndef AUDIO_RENDERER_ADAPTER_IMPL_H
#define AUDIO_RENDERER_ADAPTER_IMPL_H

#include "audio_renderer_adapter.h"

#include "audio_renderer.h"

namespace OHOS::AVSession {
class AVSession;
};

namespace OHOS::NWeb {
using namespace OHOS::AudioStandard;

class AudioRendererCallbackImpl : public AudioRendererCallback {
public:
    AudioRendererCallbackImpl(std::shared_ptr<AudioRendererCallbackAdapter> cb);

    ~AudioRendererCallbackImpl() override = default;

    void OnInterrupt(const InterruptEvent &interruptEvent) override;

    void OnStateChange(const RendererState state, const StateChangeCmdType cmdType = CMD_FROM_CLIENT) override;

private:
    std::shared_ptr<AudioRendererCallbackAdapter> cb_ = nullptr;
};

class AudioRendererAdapterImpl : public AudioRendererAdapter {
public:
    AudioRendererAdapterImpl() = default;

    ~AudioRendererAdapterImpl();

    int32_t Create(const AudioAdapterRendererOptions &rendererOptions,
        std::string cachePath = std::string()) override;

    bool Start() override;

    bool Pause() override;

    bool Stop() override;

    bool Release() override;

    int32_t Write(uint8_t *buffer, size_t bufferSize) override;

    int32_t GetLatency(uint64_t &latency) const override;

    int32_t SetVolume(float volume) const override;

    float GetVolume() const override;

    int32_t SetAudioRendererCallback(const std::shared_ptr<AudioRendererCallbackAdapter> &callback) override;

    void SetInterruptMode(bool audioExclusive) override;

    bool IsRendererStateRunning() override;

    static AudioSamplingRate GetAudioSamplingRate(AudioAdapterSamplingRate samplingRate);

    static AudioEncodingType GetAudioEncodingType(AudioAdapterEncodingType encodingType);

    static AudioSampleFormat GetAudioSampleFormat(AudioAdapterSampleFormat sampleFormat);

    static AudioChannel GetAudioChannel(AudioAdapterChannel channel);

    static ContentType GetAudioContentType(AudioAdapterContentType contentType);

    static StreamUsage GetAudioStreamUsage(AudioAdapterStreamUsage streamUsage);

private:
    std::unique_ptr<AudioRenderer> audio_renderer_;
    std::shared_ptr<AudioRendererCallbackImpl> callback_;
    static std::shared_ptr<OHOS::AVSession::AVSession> avsession_;
};
}  // namespace OHOS::NWeb

#endif // AUDIO_RENDERER_ADAPTER_IMPL_H