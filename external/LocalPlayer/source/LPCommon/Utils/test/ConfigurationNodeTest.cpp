/*
 * ConfigurationNodeTest.cpp
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
// @file ConfigurationNodeTest.cpp

#include <sstream>

#include <gtest/gtest.h>

#include "LPCommon/Utils/Configuration/ConfigurationNode.h"

namespace localPlayerSDK {
namespace rkpCommon {
namespace utils {
namespace configuration {
namespace test {

using namespace ::testing;

/// Name of non-existent object for exercising failure to find a @c ConfigurationNode.
static const std::string NON_OBJECT = "non-existent-object";

/// Name of first root level object.
static const std::string OBJECT1 = "object1";

/// Name of first bool value in first root level object.
static const std::string BOOL1_1 = "bool1.1";

/// Value of first bool value in first root level object.
static const bool BOOL_VALUE1_1 = true;

/// Name of first object inside first root level object.
static const std::string OBJECT1_1 = "object1.1";

/// Name of first string value in first object inside first root level object.
static const std::string STRING1_1_1 = "string1.1.1";

/// Value of first string value in first object inside first root level object.
static const std::string STRING_VALUE1_1_1 = "stringValue1.1.1";

/// Name of second root level object.
static const std::string OBJECT2 = "object2";

/// Name of first string in second root level object.
static const std::string STRING2_1 = "string2.1";

/// Replaced value of first string in second root level object.
static const std::string NEW_STRING_VALUE2_1 = "new-stringValue2.1";

/// Name for  non-existent int value in second root level object.
static const std::string NON_EXISTENT_INT2_1 = "non-existent-int2.1";

/// Default value for non-existent int value in second root level object.
static const int NON_EXISTENT_INT_VALUE2_1 = 123;

/// Name of first int value in second root level object.
static const std::string INT2_1 = "int2.1";

/// Name of first object inside second root level object.
static const std::string OBJECT2_1 = "object2.1";

/// Name of first string inside first object inside second root level object.
static const std::string STRING2_1_1 = "string2.1.1";

/// Replaced value of first string inside first object inside second root level object.
static const std::string NEW_STRING_VALUE2_1_1 = "new-stringValue2.1.1";

/// Bad JSON string to verify handling the failure to parse JSON
static const std::string BAD_JSON = "{ bad json }";

/// First JSON string to parse, serving as default for configuration values.
// clang-format off
static const std::string FIRST_JSON = R"(
    {
        "object1" : {
            "bool1.1" : true
        },
        "object2" : {
            "int2.1" : 21,
            "string2.1" : "stringValue2.1",
            "object2.1" : {
                "string2.1.1" : "stringValue2.1.1"
            }
        }
    })";
// clang-format on

/// Second JSON string to parse, overlaying configuration values from FIRST_JSON.
// clang-format off
static const std::string SECOND_JSON = R"(
    {
        "object1" : {
            "object1.1" : {
                "string1.1.1" : "stringValue1.1.1"
            },
            "int1.1" : 11
        }
    })";
// clang-format on

/// Third JSON string to parse, overlaying configuration values from FIRST_JSON and SECOND_JSON.
// clang-format off
static const std::string THIRD_JSON = R"(
    {
        "object2" : {
            "int2.1" : 21,
            "string2.1" : "new-stringValue2.1",
            "object2.1" : {
                "string2.1.1" : "new-stringValue2.1.1"
            }
        }
    })";
// clang-format on

/**
 * Class for testing the ConfigurationNode class
 */
class ConfigurationNodeTest : public ::testing::Test {};

/**
 * Verify initialization a configuration. Verify both the implementation of accessor methods and the results
 * of merging JSON streams.
 */
TEST_F(ConfigurationNodeTest, testInitializationAndAccess) {
    // Verify a null configuration results in failure
    ASSERT_FALSE(ConfigurationNode::initialize({nullptr}));

    // Verify invalid JSON results in failure
    std::stringstream badStream;
    badStream << BAD_JSON;
    ASSERT_FALSE(ConfigurationNode::initialize({&badStream}));

    // Combine valid JSON streams with overlapping values. Verify reported success.
    std::stringstream firstStream;
    firstStream << FIRST_JSON;
    std::stringstream secondStream;
    secondStream << SECOND_JSON;
    std::stringstream thirdStream;
    thirdStream << THIRD_JSON;
    ASSERT_TRUE(ConfigurationNode::initialize({&firstStream, &secondStream, &thirdStream}));

    // Verify failure reported for subsequent initializations.
    firstStream << FIRST_JSON;
    ASSERT_FALSE(ConfigurationNode::initialize({&firstStream}));

    // Verify non-found name results in a ConfigurationNode that evaluates to false.
    ASSERT_FALSE(ConfigurationNode::getRoot()[NON_OBJECT]);

    // Verify found name results in a ConfigurationNode that evaluates to true.
    ASSERT_TRUE(ConfigurationNode::getRoot()[OBJECT1]);

    // Verify extraction of bool value.
    bool bool11 = true;
    ASSERT_TRUE(ConfigurationNode::getRoot()[OBJECT1].getBool(BOOL1_1, &bool11));
    ASSERT_EQ(bool11, BOOL_VALUE1_1);

    // Verify traversal of multiple levels of ConfigurationNode and extraction of a string value.
    std::string string111;
    ASSERT_TRUE(ConfigurationNode::getRoot()[OBJECT1][OBJECT1_1].getString(STRING1_1_1, &string111));
    ASSERT_EQ(string111, STRING_VALUE1_1_1);

    // Verify retrieval of default value when name does not match any value.
    int nonExistentInt21 = 0;
    ASSERT_NE(nonExistentInt21, NON_EXISTENT_INT_VALUE2_1);
    ASSERT_FALSE(ConfigurationNode::getRoot()[OBJECT2].getInt(
        NON_EXISTENT_INT2_1, &nonExistentInt21, NON_EXISTENT_INT_VALUE2_1));
    ASSERT_EQ(nonExistentInt21, NON_EXISTENT_INT_VALUE2_1);

    // Verify extraction if an integer value.
    int int21;
    ASSERT_TRUE(ConfigurationNode::getRoot()[OBJECT2].getInt(INT2_1, &int21));
    ASSERT_EQ(int21, 21);

    // Verify overwrite of string value by subsequent JSON.
    std::string newString21;
    ASSERT_TRUE(ConfigurationNode::getRoot()[OBJECT2].getString(STRING2_1, &newString21));
    ASSERT_EQ(newString21, NEW_STRING_VALUE2_1);

    // Verify retrieval of default value when type does not match an existing value.
    nonExistentInt21 = 0;
    ASSERT_NE(nonExistentInt21, NON_EXISTENT_INT_VALUE2_1);
    ASSERT_FALSE(ConfigurationNode::getRoot()[OBJECT2].getInt(STRING2_1, &nonExistentInt21, NON_EXISTENT_INT_VALUE2_1));
    ASSERT_EQ(nonExistentInt21, NON_EXISTENT_INT_VALUE2_1);

    // Verify overwrite of string value in nested Configuration node.
    std::string string211;
    ASSERT_TRUE(ConfigurationNode::getRoot()[OBJECT2][OBJECT2_1].getString(STRING2_1_1, &string211));
    ASSERT_EQ(string211, NEW_STRING_VALUE2_1_1);
}

}  // namespace test
}  // namespace configuration
}  // namespace utils
}  // namespace rkpCommon
}  // namespace localPlayerSDK
