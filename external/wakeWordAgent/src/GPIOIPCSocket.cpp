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

#include "GPIOIPCSocket.h"
#include "Logger.h"
#include "WakeWordException.h"
#include "WakeWordUtils.h"

#include <sys/socket.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <unistd.h>
#include <cstring>

const u_short PORT_NUMBER = 5188;
const int SECONDS_BETWEEN_RETRIES = 2;

namespace AlexaWakeWord {

GPIOIPCSocket::GPIOIPCSocket() : 
    m_isRunning{false}, m_socketHandle{-1} {

  clearSocketMembers();

  try {
    m_isRunning = true;
    m_thread = make_unique<std::thread>(&GPIOIPCSocket::mainLoop, this);
  } catch (std::bad_alloc &e) {
    log(Logger::WARNING, "GPIOIPCSocket: Could not allocate memory");
    throw;
  } catch (WakeWordException &e) {
    log(Logger::WARNING,
        std::string("GPIOIPCSocket: other error:") + e.what());
    throw;
  }
}

GPIOIPCSocket::~GPIOIPCSocket() {

  log(Logger::DEBUG, "GPIOIPCSocket: joining on thread.");

  if(m_socketConnected) {
    sendCommand(IPCInterface::Command::DISCONNECT);
  }
  m_isRunning = false;
  m_thread->join();

  if(m_socketHandle >= 0) {
    close(m_socketHandle);
  }
}

void GPIOIPCSocket::sendCommand(const IPCInterface::Command command) {

  if(!m_socketConnected) {
    throw WakeWordException("GPIOIPCSocket::sendCommand: not connected.");
  }

  // Convert to network byte layout
  auto networkCommand = htonl(command);
  log(Logger::INFO,"networkCommand:"+std::to_string(networkCommand));
  if(send(m_socketHandle, &networkCommand, sizeof(networkCommand), 0) !=
          sizeof(networkCommand)) {
    throw WakeWordException("GPIOIPCSocket::sendCommand: error sending.");
  }
}

void GPIOIPCSocket::clearSocketMembers() {

  // This is called either on initialization (to set initial values of these
  // related data members), or to reset so we can try creating a new
  // connection.

  if(m_socketHandle >= 0) {
    close(m_socketHandle);
  }
  m_socketHandle = -1;      // 0 can be a valid socket
  m_socketConnected = false;
  memset(&m_socketAddr, 0, sizeof(sockaddr_in));
}

void GPIOIPCSocket::init() {

  clearSocketMembers();

  log(Logger::INFO, "GPIOIPCSocket: init socket on port:" +
          std::to_string(PORT_NUMBER));

  // Appropriately low-level C socket code follows

  m_socketHandle = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
  if(m_socketHandle < 0) {

    std::string errorStr = "GPIOIPCSocket: could not open IPC socket.";

    log(Logger::WARNING, errorStr);
    throw WakeWordException(errorStr);
  }

  struct ifaddrs* ifAddrStruct = nullptr;
  getifaddrs(&ifAddrStruct);
  for(struct ifaddrs* ifAddr = ifAddrStruct; ifAddr; ifAddr = ifAddr->ifa_next) {

      if (  ifAddr->ifa_addr &&
           (ifAddr->ifa_addr->sa_family == AF_INET) &&
           (ifAddr->ifa_flags & (IFF_LOOPBACK | IFF_UP | IFF_RUNNING) ) ) {

          memcpy(&m_socketAddr, ifAddr->ifa_addr, sizeof(sockaddr_in));
          m_socketAddr.sin_family = AF_INET;
          m_socketAddr.sin_port = htons(PORT_NUMBER);

        std::string address = inet_ntoa(m_socketAddr.sin_addr);
        log(Logger::DEBUG, std::string("Found interface ") +
            " | name: " + ifAddr->ifa_name +
            " | loop back interface on IPv4:" + address);
        break;
      }
  }
  if (ifAddrStruct) {
      freeifaddrs(ifAddrStruct);
  }
}

bool GPIOIPCSocket::initializeSocket() {

  try {
    init();
  } catch (WakeWordException &e) {
    log(Logger::WARNING, std::string("Failed to initialize socket: ") +
            e.what());
    return false;
  }

  return true;
}

bool GPIOIPCSocket::makeConnection() {

  if(connect(m_socketHandle,
             (struct sockaddr*)&m_socketAddr,
             sizeof(m_socketAddr)) < 0) {
    return false;
  }

  m_socketConnected = true;
  log(Logger::INFO, "===> Connected to AVS client <===");
  return true;
}

bool GPIOIPCSocket::receiveCommand() {

  uint32_t command = 0;
  int bytesRemaining = sizeof(command);
  uint8_t* pCommand = reinterpret_cast<uint8_t*>(&command);

  // The entire payload may not arrive in one pass.
  while(bytesRemaining > 0) {

    auto numBytesReceived = recv(m_socketHandle, pCommand, bytesRemaining, 0);

    if(numBytesReceived <= 0) {
        log(Logger::DEBUG, "Socket disconnected.");
        return false;
    }

    // Book-keeping
    bytesRemaining -= numBytesReceived;
    log(Logger::DEBUG,
            "Received:" + std::to_string(numBytesReceived) + " | remaining:"
                    + std::to_string(bytesRemaining));
    pCommand += numBytesReceived;
  }

  // All bytes for the command has been received
  auto commandReceivedLocal = ntohl(command);
  if (commandReceivedLocal != 0) {
//    ipcCommandReceived(IPCInterface::intToCommand(commandReceivedLocal));
  }

  return true;
}

// The main function for the thread.
void GPIOIPCSocket::mainLoop() {

  log(Logger::INFO, "GPIOIPCSocket::mainLoop thread started");

  // Outer loop - on disconnects we will continue to try re-connecting unless
  // the thread is no longer running
  while(m_isRunning) {

    // Initialize socket
    while(m_isRunning && !initializeSocket()) {
      sleep(SECONDS_BETWEEN_RETRIES);
    }

    // Connect
    while(m_isRunning && !makeConnection()) {
      log(Logger::INFO, "GPIO wake Not connected! Make sure gpio is running");
      sleep(SECONDS_BETWEEN_RETRIES);
    }

    // Process commands received until possible server disconnect
    while(m_isRunning && receiveCommand()) {
      // no-op
    }
  }

  log(Logger::INFO, "GPIOIPCSocket::mainLoop thread ended");
}

} // AlexaWakeWord
