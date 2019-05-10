/*
 * ConfigurationNode.cpp
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

#include <rapidjson/istreamwrapper.h>
#include <rapidjson/writer.h>
#include <rapidjson/error/en.h>

#include "LPCommon/Utils/Configuration/ConfigurationNode.h"
#include "LPCommon/Utils/Logger/Logger.h"

namespace localPlayerSDK {
namespace rkpCommon {
namespace utils {
namespace configuration {

/// String to identify log entries originating from this file.
static const std::string TAG("ConfigurationNode");

/**
 * Create a LogEntry using this file's TAG and the specified event string.
 *
 * @param The event string for this @c LogEntry.
 */
#define LX(event) localPlayerSDK::rkpCommon::utils::logger::LogEntry(TAG, event)

using namespace rapidjson;

std::mutex ConfigurationNode::m_mutex;
Document ConfigurationNode::m_document;
ConfigurationNode ConfigurationNode::m_root;

/**
 * Render @c rapidjson::Value as a string.
 *
 * @param value The value to render as a string.
 * @return The string rendered from the @c value.
 */
static std::string valueToString(const Value& value) {
    rapidjson::StringBuffer stringBuffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(stringBuffer);
    if (value.Accept(writer)) {
        return stringBuffer.GetString();
    }
    ACSDK_ERROR(LX("valueToStringFailed").d("reason", "AcceptReturnedFalse"));
    return "";
}

/**
 * Deep (possibly recursive) merge of two @c rapidjson values of type @c rapidjson::Type::kObject. The contents
 * of @c in are merged in to @c out. Values that occur in both are replaced by the values in @c in.
 *
 * @param path String describing the accumulated path to the sub-objects being merged.
 * @param[in,out] out The object that @c in will be merged in to.
 * @param in The object to merge in to @c out.
 * @param allocator Allocator of the @c rapidjson::Document containing @c out.
 */
static void mergeDocument(const std::string& path, Value& out, Value& in, Document::AllocatorType& allocator) {
    for (auto inIt = in.MemberBegin(); inIt != in.MemberEnd(); inIt++) {
        auto outIt = out.FindMember(inIt->name);
        if (outIt != out.MemberEnd() && inIt->value != outIt->value) {
            auto memberPath = path + "." + outIt->name.GetString();
            if (outIt->value.IsObject()) {
                ACSDK_DEBUG(LX("mergeDocument").d("reason", "objectsMerged").d("path", memberPath));
                mergeDocument(memberPath, outIt->value, inIt->value, allocator);
            } else {
                ACSDK_DEBUG(LX("mergeDocument")
                                .d("reason", "valueReplaced")
                                .d("path", memberPath)
                                .sensitive("old", valueToString(outIt->value))
                                .sensitive("new", valueToString(inIt->value)));
                outIt->value = inIt->value;
            }
        } else {
            out.AddMember(inIt->name, inIt->value, allocator);
        }
    }
}

bool ConfigurationNode::initialize(const std::vector<std::istream*>& jsonStreams) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_root) {
        ACSDK_ERROR(LX("initializeFailed").d("reason", "alreadyInitialized"));
        return false;
    }
    m_document.SetObject();
    for (auto jsonStream : jsonStreams) {
        if (!jsonStream) {
            m_document.SetObject();
            return false;
        }
        IStreamWrapper wrapper(*jsonStream);
        Document overlay(&m_document.GetAllocator());
        overlay.ParseStream<kParseCommentsFlag>(wrapper);
        if (overlay.HasParseError()) {
            ACSDK_ERROR(LX("initializeFailed")
                            .d("reason", "parseFailure")
                            .d("offset", overlay.GetErrorOffset())
                            .d("message", GetParseError_En(overlay.GetParseError())));
            m_document.SetObject();
            return false;
        }
        mergeDocument("root", m_document, overlay, m_document.GetAllocator());
    }
    m_root = ConfigurationNode(&m_document);
    ACSDK_INFO(LX("initializeSuccess").sensitive("configuration", valueToString(m_document)));
    return true;
}

void ConfigurationNode::uninitialize() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_document.SetObject();
    m_root = ConfigurationNode();
}

ConfigurationNode ConfigurationNode::getRoot() {
    return m_root;
}

ConfigurationNode::ConfigurationNode() : m_object{nullptr} {
}

bool ConfigurationNode::getBool(const std::string& key, bool* out, bool defaultValue) const {
    return getValue(key, out, defaultValue, &Value::IsBool, &Value::GetBool);
}

bool ConfigurationNode::getInt(const std::string& key, int* out, int defaultValue) const {
    return getValue(key, out, defaultValue, &Value::IsInt, &Value::GetInt);
}

bool ConfigurationNode::getString(const std::string& key, std::string* out, std::string defaultValue) const {
    const char* temp;
    auto result = getString(key, &temp, defaultValue.c_str());
    if (out) {
        *out = temp;
    }
    return result;
}

bool ConfigurationNode::getString(const std::string& key, const char** out, const char* defaultValue) const {
    return getValue(key, out, defaultValue, &Value::IsString, &Value::GetString);
}

template <typename Type>
bool ConfigurationNode::getValue(
    const std::string& key,
    Type* out,
    Type defaultValue,
    bool (rapidjson::Value::*isType)() const,
    Type (rapidjson::Value::*getType)() const) const {
    if (key.empty() || !m_object) {
        if (out) {
            *out = defaultValue;
        }
        return false;
    }
    auto it = m_object->FindMember(key.c_str());
    if (m_object->MemberEnd() == it || !(it->value.*isType)()) {
        if (out) {
            *out = defaultValue;
        }
        return false;
    }
    if (out) {
        *out = (it->value.*getType)();
    }
    return true;
}

ConfigurationNode ConfigurationNode::operator[](const std::string& key) const {
    if (!*this) {
        return ConfigurationNode();
    }
    auto it = m_object->FindMember(key.c_str());
    if (m_object->MemberEnd() == it || !it->value.IsObject()) {
        return ConfigurationNode();
    }
    return ConfigurationNode(&it->value);
}

ConfigurationNode::operator bool() const {
    return m_object;
}

ConfigurationNode::ConfigurationNode(const rapidjson::Value* object) : m_object{object} {
}

}  // namespace configuration
}  // namespace utils
}  // namespace rkpCommon
}  // namespace localPlayerSDK
