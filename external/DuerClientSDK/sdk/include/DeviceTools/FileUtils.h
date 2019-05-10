/*
* Copyright (c) 2017 Baidu, Inc. All Rights Reserved.
*
*  Licensed under the Apache License, Version 2.0 (the "License");
*  you may not use this file except in compliance with the License.
*  You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
*  Unless required by applicable law or agreed to in writing, software
*  distributed under the License is distributed on an "AS IS" BASIS,
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*  See the License for the specific language governing permissions and
*  limitations under the License.
*/

#ifndef DEVICE_COMMON_LIB_DEVICE_TOOLS_INCLUDE_FILE_UTILS_H_
#define DEVICE_COMMON_LIB_DEVICE_TOOLS_INCLUDE_FILE_UTILS_H_

#include <string>
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

namespace deviceCommonLib {
namespace deviceTools {

/**
 * A small utility function to help determine if a file exists.
 *
 * @param filePath The path to the file being queried about.
 * @return Whether the file exists and is accessible.
 */
bool fileExists(const std::string& filePath);

/**
 * A utility function to delete a file.
 *
 * @param filePath The path to the file being deleted.
 * @return Whether the file was deleted ok.
 */
bool removeFile(const std::string& filePath);

/**
 * A utility function to parse a directory.
 *
 * @param path The path to the directory being parsed.
 * @return the directory string.
 */
std::string parseDir(const std::string& path);

/**
 * A utility function to judge whether the directory exists.
 *
 * @param dir The path to the directory.
 * @return True or False.
 */
bool isDirExist(const std::string& dir);

/**
* A utility function to make directory.
*
* @param dir The path to the directory.
* @return True or False.
*/
bool mkdirR(const char* dir);
/**
 * A utility function to get directory from a path.
 * @param filePath The source file path.
 * @return The directory of the file path.
 */
std::string getDirectoryFromPath(const std::string& filePath);

}  // namespace deviceTools
}  // namespace deviceCommonLib

#endif  // DEVICE_COMMON_LIB_DEVICE_TOOLS_INCLUDE_FILE_UTILS_H_
