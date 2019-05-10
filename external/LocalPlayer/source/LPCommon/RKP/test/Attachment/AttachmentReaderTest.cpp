/*
 * AttachmentReaderTest.cpp
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

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "../include/LPCommon/RKP/Attachment/InProcessAttachmentReader.h"
#include "../include/LPCommon/RKP/Attachment/AttachmentWriter.h"

#include "Common/Common.h"

namespace localPlayerSDK {
namespace rkpCommon {
namespace rkp {
namespace test {

using namespace ::testing;
using namespace localPlayerSDK::rkpCommon::rkp::attachment;
using namespace localPlayerSDK::rkpCommon::utils::sds;

/// A test seek position, defined in terms of above variables.
static const int TEST_SDS_SEEK_POSITION = TEST_SDS_BUFFER_SIZE_IN_BYTES - (TEST_SDS_PARTIAL_READ_AMOUNT_IN_BYTES + 10);
/// A test seek position which is bad.
static const int TEST_SDS_BAD_SEEK_POSITION = TEST_SDS_BUFFER_SIZE_IN_BYTES + 1;

/**
 * A class which helps drive this unit test suite.
 */
class AttachmentReaderTest : public ::testing::Test {
public:
    /**
     * Constructor.
     */
    AttachmentReaderTest() :
            m_readerPolicy{AttachmentReader::Policy::NON_BLOCKING},
            m_writerPolicy{InProcessSDS::Writer::Policy::ALL_OR_NOTHING} {
    }

    /**
     * Initialization function to set up and test the class members.
     *
     * @param createReader Whether the reader data member should be constructed from m_sds.
     */
    void init(bool createReader = true);

    /**
     * Utility function to test multiple reads from an SDS.
     *
     * @param closeWriterBeforeReading Allows the writer to be closed before the reader performs its multiple reads.
     */
    void testMultipleReads(bool closeWriterBeforeReading);

    /**
     * Utility function to read data from an SDS and verify its results againts m_pattern.
     *
     * @param reader The reader with which to read data.
     * @param resultSize The size of the data the reader should read.
     * @param dataOffset The offset into the pattern which is being read from.
     */
    void readAndVerifyResult(
        std::shared_ptr<InProcessAttachmentReader> reader,
        size_t resultSize,
        size_t dataOffset = 0);

    /// The commonly used AttachmentReader policy.
    AttachmentReader::Policy m_readerPolicy;
    /// The commonly used SDSWriter policy.
    InProcessSDS::Writer::Policy m_writerPolicy;
    /// The commonly used SDS in these tests.
    std::shared_ptr<InProcessSDS> m_sds;
    /// The commonly used reader in these tests.
    std::unique_ptr<InProcessAttachmentReader> m_reader;
    /// The commonly used writer in these tests.
    std::unique_ptr<InProcessSDS::Writer> m_writer;
    /// The commonly used test pattern in these tests.
    std::vector<uint8_t> m_testPattern;
};

void AttachmentReaderTest::init(bool createReader) {
    m_sds = createSDS(TEST_SDS_BUFFER_SIZE_IN_BYTES);
    ASSERT_NE(m_sds, nullptr);
    m_writer = m_sds->createWriter(m_writerPolicy);
    ASSERT_NE(m_writer, nullptr);
    if (createReader) {
        m_reader = InProcessAttachmentReader::create(m_readerPolicy, m_sds);
        ASSERT_NE(m_reader, nullptr);
    }
    m_testPattern = createTestPattern(TEST_SDS_BUFFER_SIZE_IN_BYTES);
}

void AttachmentReaderTest::testMultipleReads(bool closeWriterBeforeReading) {
    init();

    auto numWritten = m_writer->write(m_testPattern.data(), m_testPattern.size());
    ASSERT_EQ(numWritten, static_cast<ssize_t>(m_testPattern.size()));

    AttachmentReader::ReadStatus terminalStatus = AttachmentReader::ReadStatus::OK_WOULDBLOCK;
    if (closeWriterBeforeReading) {
        m_writer->close();
        terminalStatus = AttachmentReader::ReadStatus::CLOSED;
    }

    std::vector<uint8_t> result(TEST_SDS_PARTIAL_READ_AMOUNT_IN_BYTES);
    auto readStatus = InProcessAttachmentReader::ReadStatus::OK;

    int totalBytesRead = 0;
    bool done = false;
    int iterations = 0;
    int iterationsMax = 10;

    while (!done && iterations < iterationsMax) {
        auto bytesRead = m_reader->read(result.data(), result.size(), &readStatus);

        if (terminalStatus == readStatus) {
            done = true;
        }

        for (size_t i = 0; i < bytesRead; ++i) {
            EXPECT_EQ(result[i], m_testPattern[i + totalBytesRead]);
        }

        totalBytesRead += bytesRead;
        iterations++;
    }

    // Not only was all the data read, but the reader remained open.
    ASSERT_NE(iterations, iterationsMax);
    ASSERT_EQ(readStatus, terminalStatus);
    ASSERT_EQ(totalBytesRead, static_cast<ssize_t>(m_testPattern.size()));
}

void AttachmentReaderTest::readAndVerifyResult(
    std::shared_ptr<InProcessAttachmentReader> reader,
    size_t resultSize,
    size_t dataOffset) {
    std::vector<uint8_t> result(resultSize);
    auto readStatus = InProcessAttachmentReader::ReadStatus::OK;
    auto numRead = reader->read(result.data(), result.size(), &readStatus);
    ASSERT_EQ(numRead, result.size());
    ASSERT_EQ(readStatus, InProcessAttachmentReader::ReadStatus::OK);

    for (size_t i = 0; i < result.size(); ++i) {
        EXPECT_EQ(result[i], m_testPattern[i + dataOffset]);
    }
}

/**
 * Test reading an invalid SDS.
 */
TEST_F(AttachmentReaderTest, testAttachmentReaderWithInvalidSDS) {
    auto reader = InProcessAttachmentReader::create(m_readerPolicy, nullptr);
    ASSERT_EQ(reader, nullptr);
}

/**
 * Test reading an SDS with a bad seek position.
 */
TEST_F(AttachmentReaderTest, testAttachmentReaderWithBadSeekPosition) {
    auto reader = InProcessAttachmentReader::create(m_readerPolicy, m_sds, TEST_SDS_BAD_SEEK_POSITION);
    ASSERT_EQ(reader, nullptr);
}

/**
 * Test a one-pass write and read.
 */
TEST_F(AttachmentReaderTest, testAttachmentReaderReadInOnePass) {
    init();

    auto testPattern = createTestPattern(TEST_SDS_BUFFER_SIZE_IN_BYTES);
    auto numWritten = m_writer->write(m_testPattern.data(), m_testPattern.size());
    ASSERT_EQ(numWritten, static_cast<ssize_t>(m_testPattern.size()));

    readAndVerifyResult(std::shared_ptr<InProcessAttachmentReader>(std::move(m_reader)), TEST_SDS_BUFFER_SIZE_IN_BYTES);
}

/**
 * Test a partial read.
 */
TEST_F(AttachmentReaderTest, testAttachmentReaderPartialRead) {
    init();

    auto numWritten = m_writer->write(m_testPattern.data(), m_testPattern.size());
    ASSERT_EQ(numWritten, static_cast<ssize_t>(m_testPattern.size()));

    readAndVerifyResult(std::shared_ptr<InProcessAttachmentReader>(std::move(m_reader)), TEST_SDS_BUFFER_SIZE_IN_BYTES);
}

/**
 * Test a partial read with a seek.
 */
TEST_F(AttachmentReaderTest, testAttachmentReaderPartialReadWithSeek) {
    init(false);

    // test a single write & read.
    auto numWritten = m_writer->write(m_testPattern.data(), m_testPattern.size());
    ASSERT_EQ(numWritten, static_cast<ssize_t>(m_testPattern.size()));

    auto reader = InProcessAttachmentReader::create(m_readerPolicy, m_sds, TEST_SDS_SEEK_POSITION);
    ASSERT_NE(reader, nullptr);

    readAndVerifyResult(
        std::shared_ptr<InProcessAttachmentReader>(std::move(reader)),
        TEST_SDS_PARTIAL_READ_AMOUNT_IN_BYTES,
        TEST_SDS_SEEK_POSITION);
}

/**
 * Test multiple partial reads of complete data, where the writer closes.
 */
TEST_F(AttachmentReaderTest, testAttachmentReaderMultipleReads) {
    testMultipleReads(false);
}

/**
 * Test multiple partial reads of complete data, where the writer remains open.
 */
TEST_F(AttachmentReaderTest, testAttachmentReaderMultipleReadsOfUnfinishedData) {
    testMultipleReads(true);
}

}  // namespace test
}  // namespace rkp
}  // namespace rkpCommon
}  // namespace localPlayerSDK
