/*
 * RKPEndpointAssignerInterface.h
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

#ifndef LOCAL_PLAYER_SDK_RKPCOMMON_SDKINTERFACES_INCLUDE_RKPCOMMON_SDKINTERFACES_RKPENDPOINTASSIGNERINTERFACE_H_
#define LOCAL_PLAYER_SDK_RKPCOMMON_SDKINTERFACES_INCLUDE_RKPCOMMON_SDKINTERFACES_RKPENDPOINTASSIGNERINTERFACE_H_

#include <string>

namespace localPlayerSDK {
namespace rkpCommon {
namespace sdkInterfaces {

/**
 * An interface for setting RKP endpoint.
 */
class RKPEndpointAssignerInterface {
public:
    /// Destructor.
    virtual ~RKPEndpointAssignerInterface() = default;

    /**
     * Set RKP Endpoint as the given parameter
     *
     * @param rkpEndpoint RKP endpoint to set.
     */
    virtual void setRKPEndpoint(const std::string& rkpEndpoint) = 0;
};

}  // namespace sdkInterfaces
}  // namespace rkpCommon
}  // namespace localPlayerSDK

#endif  // LOCAL_PLAYER_SDK_RKPCOMMON_SDKINTERFACES_INCLUDE_RKPCOMMON_SDKINTERFACES_RKPENDPOINTASSIGNERINTERFACE_H_
