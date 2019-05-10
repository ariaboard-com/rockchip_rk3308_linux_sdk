/*
 * Normalizer.h
 *
 * Copyright 2018 Rockchip.com, Inc. or its affiliates.
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

#ifndef LOCAL_PLAYER_SDK_MEDIAPLAYER_INCLUDE_MEDIAPLAYER_NORMALIZER_H_
#define LOCAL_PLAYER_SDK_MEDIAPLAYER_INCLUDE_MEDIAPLAYER_NORMALIZER_H_

#include <memory>

namespace localPlayerSDK {
namespace mediaPlayer {

/**
 * This class performs a linear transform between two ranges [sourceMin, sourceMax] -> [normalizedMin, normalizedMax].
 * This is used by the MediaPlayer to convert between different volume ranges. MediaPlayer internally uses GStreamer,
 * whose volume element takes in a @c gdouble.
 */
class Normalizer {
public:
    /**
     * Creates a unique_ptr to a @c Normalizer class.
     *
     * @param sourceMin The minimum value in the source space.
     * @param sourceMax The maximum value in the source space.
     * @param normalizedMin The minimum value in the normalized space.
     * @param normalizedMax The maximum value in the normalized space.
     * @return A pointer to an instance of the Normalizer class if the ranges are valid. Otherwise, nullptr is returned.
     */
    static std::unique_ptr<Normalizer> create(
        const double& sourceMin,
        const double& sourceMax,
        const double& normalizedMin,
        const double& normalizedMax);

    /**
     * Performs feature scaling from [srcMin, srcMax] to [normalizedMin, normalizedMax].
     *
     * @param unnormalizedInput The input value to normalize.
     * @param[out] normalizedOutput The result after normalization.
     * @return Whether the operation was successful.
     */
    bool normalize(const double& unnormalizedInput, double* normalizedOutput);

private:
    /**
     * Constructor.
     *
     * @param sourceMin The minimum value in the source space.
     * @param sourceMax The maximum value in the source space.
     * @param normalizedMin The minimum value in the normalized space.
     * @param normalizedMax The maximum value in the normalized space.
     */
    Normalizer(
        const double& sourceMin,
        const double& sourceMax,
        const double& normalizedMin,
        const double& normalizedMax);

    /// The scale factor for the linear transform.
    double m_scaleFactor;
    /// The minimum value in the source space.
    double m_sourceMin;
    /// The maximum value in the source space.
    double m_sourceMax;
    /// The minimum value in the normalized space.
    double m_normalizedMin;
    /// The maximum value in the normalized space.
    double m_normalizedMax;
};

}  // namespace mediaPlayer
}  // namespace localPlayerSDK

#endif  // LOCAL_PLAYER_SDK_MEDIAPLAYER_INCLUDE_MEDIAPLAYER_NORMALIZER_H_
