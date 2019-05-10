/*
 * TaskThread.h
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

#ifndef LOCAL_PLAYER_SDK_RKPCOMMON_UTILS_INCLUDE_RKPCOMMON_UTILS_THREADING_TASKTHREAD_H_
#define LOCAL_PLAYER_SDK_RKPCOMMON_UTILS_INCLUDE_RKPCOMMON_UTILS_THREADING_TASKTHREAD_H_

#include <atomic>
#include <memory>
#include <thread>

#include "LPCommon/Utils/Threading/TaskQueue.h"

namespace localPlayerSDK {
namespace rkpCommon {
namespace utils {
namespace threading {

/**
 * A TaskThread is a thread which reads from a TaskQueue and executes those tasks.
 */
class TaskThread {
public:
    /**
     * Constructs a TaskThread to read from the given TaskQueue. This does not start the thread.
     *
     * @params taskQueue A TaskQueue to take tasks from to execute.
     */
    TaskThread(std::shared_ptr<TaskQueue> taskQueue);

    /**
     * Destructs the TaskThread.
     */
    ~TaskThread();

    /**
     * Starts executing tasks from the queue on the thread.
     */
    void start();

    /**
     * Returns whether or not the TaskThread has been shutdown.
     *
     * @returns whether or not the TaskThread has been shutdown.
     */
    bool isShutdown();

private:
    /**
     * Loops over the TaskQueue executing all tasks until either the TaskQueue is lost, or the thread is shutdown.
     */
    void processTasksLoop();

    /// A weak pointer to the TaskQueue, if the task queue is no longer accessible, there is no reason to execute tasks.
    std::weak_ptr<TaskQueue> m_taskQueue;

    /// A flag to message the task thread to stop executing.
    std::atomic_bool m_shutdown;

    /// The thread to run tasks on.
    std::thread m_thread;
};

}  // namespace threading
}  // namespace utils
}  // namespace rkpCommon
}  // namespace localPlayerSDK

#endif  // LOCAL_PLAYER_SDK_RKPCOMMON_UTILS_INCLUDE_RKPCOMMON_UTILS_THREADING_TASKTHREAD_H_
