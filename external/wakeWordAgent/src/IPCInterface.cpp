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

#include "IPCInterface.h"

namespace AlexaWakeWord {

std::string IPCInterface::commandToString(
        const Command command) {

  switch(command) {
    case DISCONNECT:
      return "DISCONNECT";
    case WAKE_WORD_DETECTED:
      return "WAKE_WORD_DETECTED";
    case PAUSE_WAKE_WORD_ENGINE:
      return "PAUSE_WAKE_WORD_ENGINE";
    case RESUME_WAKE_WORD_ENGINE:
      return "RESUME_WAKE_WORD_ENGINE";
    case CONFIRM:
      return "CONFIRM";
    case INIT_WAKE_WORD_ENGINE:
      return "INIT_WAKE_WORD_ENGINE";
    default:
      return "UNKNOWN";
  }
}

IPCInterface::Command IPCInterface::intToCommand(int intCommand) {
  switch(intCommand) {
    case DISCONNECT:
      return DISCONNECT;
    case WAKE_WORD_DETECTED:
      return WAKE_WORD_DETECTED;
    case PAUSE_WAKE_WORD_ENGINE:
      return PAUSE_WAKE_WORD_ENGINE;
    case RESUME_WAKE_WORD_ENGINE:
      return RESUME_WAKE_WORD_ENGINE;
    case CONFIRM:
      return CONFIRM;
    case INIT_WAKE_WORD_ENGINE:
      return INIT_WAKE_WORD_ENGINE;
    case AUTHORiZED_SUCCESS:
      return AUTHORiZED_SUCCESS;
    case AUTHORiZED_FAIL:
      return AUTHORiZED_FAIL;
    case START_AMAZON_RECORD:
      return START_AMAZON_RECORD;
    case STOP_AMAZON_RECORD:
      return STOP_AMAZON_RECORD;
    case START_AMAZON_PLAY:
      return START_AMAZON_PLAY;
    case AMAZON_SETVOLUME_0 :
      return AMAZON_SETVOLUME_0;
    case AMAZON_SETVOLUME_1 :
      return AMAZON_SETVOLUME_1;
    case AMAZON_SETVOLUME_2 :
      return AMAZON_SETVOLUME_2;
    case AMAZON_SETVOLUME_3 :
      return AMAZON_SETVOLUME_3;
    case AMAZON_SETVOLUME_4 :
      return AMAZON_SETVOLUME_4;
    case AMAZON_SETVOLUME_5 :
      return AMAZON_SETVOLUME_5;
    case AMAZON_SETVOLUME_6 :
      return AMAZON_SETVOLUME_6;
    case AMAZON_SETVOLUME_7 :
      return AMAZON_SETVOLUME_7;
    case AMAZON_SETVOLUME_8 :
      return AMAZON_SETVOLUME_8;
    case AMAZON_SETVOLUME_9 :
      return AMAZON_SETVOLUME_9;
    case AMAZON_SETVOLUME_10 :
      return AMAZON_SETVOLUME_10;
    case AMAZON_SETVOLUME_11 :
      return AMAZON_SETVOLUME_11;
    case AMAZON_SETVOLUME_12 :
      return AMAZON_SETVOLUME_12;
    default:
      return UNKNOWN;
  }
}

} // namespace AlexaWakeWord
