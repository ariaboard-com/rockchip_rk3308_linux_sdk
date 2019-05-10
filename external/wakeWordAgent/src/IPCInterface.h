/**
 * Copyright 2016 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Licensed under the Amazon Software License (the "License"). You may not use this file 
 * except in compliance with the License. A copy of the License is located at
 *
 *   http://aws.amazon.com/asl/
 *
 * or in the "license" file accompanying this file. This file is distributed on an "AS IS" 
 * BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, express or implied. See the License 
 * for the specific language governing permissions and limitations under the License.
 */

#ifndef ALEXA_VS_WAKE_WORD_IPC_INTERFACE_H_
#define ALEXA_VS_WAKE_WORD_IPC_INTERFACE_H_

#include <string>

namespace AlexaWakeWord {

// This class provides the interface for interacting with the Alexa Sample App
// over IPC.  In this project, this is required to communicate to the Sample App
// that the WakeWordAgent has detected the keyword ('Alexa').  The Sample App
// responds in turn to the Agent when the user has completed their interaction
// with Alexa.
class IPCInterface {

public:
  enum Command : uint32_t {
    DISCONNECT                = 1,  // OUTGOING : Ask the AVS client to disconnect from us
    WAKE_WORD_DETECTED        = 2,  // OUTGOING : sent to AVS client when a wake word is detected
    PAUSE_WAKE_WORD_ENGINE    = 3,  // INCOMING : request to pause the engine and yield the Mic
    RESUME_WAKE_WORD_ENGINE   = 4,  // INCOMING : request to resume the engine
    CONFIRM                   = 5,  // OUTGOING : sent to AVS client to confirm the engine has stopped
    INIT_WAKE_WORD_ENGINE     = 6,  // OUTGOING : init success WAKE_WORD_ENGINE 
    AUTHORiZED_SUCCESS        = 7,  // INCOMING : Know Authorized success from javaclient
    AUTHORiZED_FAIL           = 8,  // INCOMING : Know Authorized fail from javaclient
    START_AMAZON_RECORD       = 9,  // OUTGOING : START_AMAZON_RECORD
    STOP_AMAZON_RECORD        = 10,  // OUTGOING : STOP_AMAZON_RECORD
    START_AMAZON_PLAY         = 11,  // OUTGOING : START_AMAZON_PLAY
    AMAZON_SETVOLUME_0        = 12,  // OUTGOING : setvolume
    AMAZON_SETVOLUME_1        = 13,  // OUTGOING : setvolume
    AMAZON_SETVOLUME_2        = 14,  // OUTGOING : setvolume
    AMAZON_SETVOLUME_3        = 15,  // OUTGOING : setvolume
    AMAZON_SETVOLUME_4        = 16,  // OUTGOING : setvolume
    AMAZON_SETVOLUME_5        = 17,  // OUTGOING : setvolume
    AMAZON_SETVOLUME_6        = 18,  // OUTGOING : setvolume
    AMAZON_SETVOLUME_7        = 19,  // OUTGOING : setvolume
    AMAZON_SETVOLUME_8        = 20,  // OUTGOING : setvolume
    AMAZON_SETVOLUME_9        = 21,  // OUTGOING : setvolume
    AMAZON_SETVOLUME_10       = 22,  // OUTGOING : setvolume
    AMAZON_SETVOLUME_11       = 23,  // OUTGOING : setvolume
    AMAZON_SETVOLUME_12       = 24,  // OUTGOING : setvolume
    UNKNOWN                   = 25   // n/a : for error & default cases
  };

  virtual ~IPCInterface() = default;

  // This will be called by the IPC handling code when an actual command
  // comes in over IPC from a wake-word agent - the implementation
  // of this function will handle it.
  virtual void onIPCCommandReceived(Command command) = 0;

  // Utility function
  static Command intToCommand(int intCommand);

private:

  // Utility function
  std::string commandToString(const Command command);
};

} // namespace AlexaWakeWord

#endif // ALEXA_VS_WAKE_WORD_IPC_INTERFACE_H_
