/*
 * AttachmentReaderSource.h
 *
 * Copyright 2018 Rockchip.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *     http://www.rock-chips.com/
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

#ifndef LOCAL_PLAYER_SDK_MEDIAPLAYER_INCLUDE_MEDIAPLAYER_ATTACHMENTREADERSOURCE_H_
#define LOCAL_PLAYER_SDK_MEDIAPLAYER_INCLUDE_MEDIAPLAYER_ATTACHMENTREADERSOURCE_H_

#include <memory>

#include <gst/gst.h>
#include <gst/app/gstappsrc.h>

#include <LPCommon/Utils/MediaPlayer/MediaPlayerInterface.h>

#include "MediaPlayer/BaseStreamSource.h"

namespace localPlayerSDK {
namespace mediaPlayer {

class AttachmentReaderSource : public BaseStreamSource {
public:
    /**
     * Creates an instance of the @c AttachmentReaderSource and installs the source within the GStreamer pipeline.
     *
     * @param pipeline The @c PipelineInterface through which the source of the @c AudioPipeline may be set.
     * @param attachmentReader The @c AttachmentReader from which to create the pipeline source from.
     *
     * @return An instance of the @c AttachmentReaderSource if successful else a @c nullptr.
     */
    static std::unique_ptr<AttachmentReaderSource> create(
        PipelineInterface* pipeline,
        std::shared_ptr<rkpCommon::rkp::attachment::AttachmentReader> attachmentReader);

    ~AttachmentReaderSource();

    /// @name Overridden SourceInterface methods.
    /// @{
    bool isPlaybackRemote() const override;
    /// @}

private:
    /**
     * Constructor.
     *
     * @param pipeline The @c PipelineInterface through which the source of the @c AudioPipeline may be set.
     * @param attachmentReader The @c AttachmentReader from which to create the pipeline source from.
     */
    AttachmentReaderSource(
        PipelineInterface* pipeline,
        std::shared_ptr<rkpCommon::rkp::attachment::AttachmentReader> attachmentReader);

    /// @name Overridden BaseStreamSource methods.
    /// @{
    bool isOpen() override;
    void close() override;
    gboolean handleReadData() override;
    gboolean handleSeekData(guint64 offset) override;
    /// @}

    /// @name RequiresShutdown Functions
    /// @{
    void doShutdown() override{};
    /// @}

private:
    /// The @c AttachmentReader to read audioData from.
    std::shared_ptr<rkpCommon::rkp::attachment::AttachmentReader> m_reader;
};

}  // namespace mediaPlayer
}  // namespace localPlayerSDK

#endif  // LOCAL_PLAYER_SDK_MEDIAPLAYER_INCLUDE_MEDIAPLAYER_ATTACHMENTREADERSOURCE_H_
