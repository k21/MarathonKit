/*
 * This file is part of MarathonKit.
 * Copyright (C) 2015 Jakub Zika
 *
 * MarathonKit is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * I am providing code in this repository to you under an open source license.
 * Because this is my personal repository, the license you receive to my code is
 * from me and not from my employer (Facebook).
 */

#include <memory>
#include <utility>

#include <gmock/gmock.h>

#include "Core/LineBuffer.h"

#include "MockFileDescriptor.h"

using MarathonKit::Core::LineBuffer;
using std::make_shared;
using std::shared_ptr;
using std::swap;
using testing::InSequence;
using testing::Return;

TEST(LineBufferTest, noDataAvailable) {
  shared_ptr<MockFileDescriptor> fd = make_shared<MockFileDescriptor>();
  LineBuffer lineBuffer(fd);

  EXPECT_CALL(*fd, isReadyForReading())
    .WillRepeatedly(Return(false));

  EXPECT_EQ(0, lineBuffer.linesReady());
  EXPECT_EQ(0, lineBuffer.charsReady());
}

TEST(LineBufferTest, charsReady) {
  shared_ptr<MockFileDescriptor> fd = make_shared<MockFileDescriptor>();
  LineBuffer lineBuffer(fd);

  {
    InSequence seq;

    EXPECT_CALL(*fd, isReadyForReading())
      .WillOnce(Return(true));
    EXPECT_CALL(*fd, read())
      .WillOnce(Return("abcd"));
  }

  ASSERT_GT(lineBuffer.charsReady(), 0);
  EXPECT_EQ('a', lineBuffer.getChar());

  ASSERT_GT(lineBuffer.charsReady(), 0);
  EXPECT_EQ('b', lineBuffer.getChar());

  ASSERT_GT(lineBuffer.charsReady(), 0);
  EXPECT_EQ('c', lineBuffer.getChar());

  ASSERT_GT(lineBuffer.charsReady(), 0);
  EXPECT_EQ('d', lineBuffer.getChar());
}

TEST(LineBufferTest, linesReady) {
  shared_ptr<MockFileDescriptor> fd = make_shared<MockFileDescriptor>();
  LineBuffer lineBuffer(fd);

  {
    InSequence seq;

    EXPECT_CALL(*fd, isReadyForReading())
      .WillOnce(Return(true));
    EXPECT_CALL(*fd, read())
      .WillOnce(Return("abcd\n"));
  }

  ASSERT_EQ(1, lineBuffer.linesReady());
  EXPECT_EQ("abcd", lineBuffer.getLine());
}

TEST(LineBufferTest, linesReadyAfterMultipleReads) {
  shared_ptr<MockFileDescriptor> fd = make_shared<MockFileDescriptor>();
  LineBuffer lineBuffer(fd);

  {
    InSequence seq;

    EXPECT_CALL(*fd, isReadyForReading())
      .WillOnce(Return(true));
    EXPECT_CALL(*fd, read())
      .WillOnce(Return("abcd"));
    EXPECT_CALL(*fd, isReadyForReading())
      .WillOnce(Return(true));
    EXPECT_CALL(*fd, read())
      .WillOnce(Return("efgh\n"));
  }

  ASSERT_EQ(1, lineBuffer.linesReady());
  EXPECT_EQ("abcdefgh", lineBuffer.getLine());
}

TEST(LineBufferTest, multipleLinesInASingleRead) {
  shared_ptr<MockFileDescriptor> fd = make_shared<MockFileDescriptor>();
  LineBuffer lineBuffer(fd);

  {
    InSequence seq;

    EXPECT_CALL(*fd, isReadyForReading())
      .WillOnce(Return(true));
    EXPECT_CALL(*fd, read())
      .WillOnce(Return("abcd\nefgh\nij"));
  }

  ASSERT_EQ(2, lineBuffer.linesReady());
  EXPECT_EQ("abcd", lineBuffer.getLine());

  ASSERT_EQ(1, lineBuffer.linesReady());
  EXPECT_EQ("efgh", lineBuffer.getLine());

  {
    InSequence seq;

    EXPECT_CALL(*fd, isReadyForReading())
      .WillOnce(Return(true));
    EXPECT_CALL(*fd, read())
      .WillOnce(Return("kl\nmnop\n"));
    EXPECT_CALL(*fd, isReadyForReading())
      .WillRepeatedly(Return(false));
  }

  ASSERT_EQ(2, lineBuffer.linesReady());
  EXPECT_EQ("ijkl", lineBuffer.getLine());

  ASSERT_EQ(1, lineBuffer.linesReady());
  EXPECT_EQ("mnop", lineBuffer.getLine());

  EXPECT_EQ(0, lineBuffer.linesReady());
}

TEST(LineBufferTest, charsReadyButNoLinesReady) {
  shared_ptr<MockFileDescriptor> fd = make_shared<MockFileDescriptor>();
  LineBuffer lineBuffer(fd);

  {
    InSequence seq;

    EXPECT_CALL(*fd, isReadyForReading())
      .WillOnce(Return(true));
    EXPECT_CALL(*fd, read())
      .WillOnce(Return("ab"));
    EXPECT_CALL(*fd, isReadyForReading())
      .WillOnce(Return(true));
    EXPECT_CALL(*fd, read())
      .WillOnce(Return("cd"));
    EXPECT_CALL(*fd, isReadyForReading())
      .WillRepeatedly(Return(false));
  }

  EXPECT_EQ(0, lineBuffer.linesReady());
  ASSERT_GT(lineBuffer.charsReady(), 0);
  EXPECT_EQ('a', lineBuffer.getChar());

  EXPECT_EQ(0, lineBuffer.linesReady());
  ASSERT_GT(lineBuffer.charsReady(), 0);
  EXPECT_EQ('b', lineBuffer.getChar());

  EXPECT_EQ(0, lineBuffer.linesReady());
  ASSERT_GT(lineBuffer.charsReady(), 0);
  EXPECT_EQ('c', lineBuffer.getChar());

  EXPECT_EQ(0, lineBuffer.linesReady());
  ASSERT_GT(lineBuffer.charsReady(), 0);
  EXPECT_EQ('d', lineBuffer.getChar());

  EXPECT_EQ(0, lineBuffer.linesReady());
  EXPECT_EQ(0, lineBuffer.charsReady());
}

TEST(LineBufferTest, resumesReadingWhenDataIsAvailableAgain) {
  shared_ptr<MockFileDescriptor> fd = make_shared<MockFileDescriptor>();
  LineBuffer lineBuffer(fd);

  {
    InSequence seq;

    EXPECT_CALL(*fd, isReadyForReading())
      .WillOnce(Return(true));
    EXPECT_CALL(*fd, read())
      .WillOnce(Return("ab"));
    EXPECT_CALL(*fd, isReadyForReading())
      .WillRepeatedly(Return(false));
  }

  EXPECT_GT(lineBuffer.charsReady(), 0);
  EXPECT_EQ(0, lineBuffer.linesReady());

  {
    InSequence seq;

    EXPECT_CALL(*fd, isReadyForReading())
      .WillOnce(Return(true));
    EXPECT_CALL(*fd, read())
      .WillOnce(Return("cd\n"));
    EXPECT_CALL(*fd, isReadyForReading())
      .WillRepeatedly(Return(false));
  }

  EXPECT_GT(lineBuffer.charsReady(), 0);
  ASSERT_EQ(1, lineBuffer.linesReady());
  EXPECT_EQ("abcd", lineBuffer.getLine());
}

TEST(LineBufferTest, supportsMixedReadingModes) {
  shared_ptr<MockFileDescriptor> fd = make_shared<MockFileDescriptor>();
  LineBuffer lineBuffer(fd);

  {
    InSequence seq;

    EXPECT_CALL(*fd, isReadyForReading())
      .WillOnce(Return(true));
    EXPECT_CALL(*fd, read())
      .WillOnce(Return("abcd\nef"));
    EXPECT_CALL(*fd, isReadyForReading())
      .WillRepeatedly(Return(false));
  }

  EXPECT_EQ(1, lineBuffer.linesReady());
  ASSERT_GT(lineBuffer.charsReady(), 0);
  EXPECT_EQ('a', lineBuffer.getChar());

  ASSERT_EQ(1, lineBuffer.linesReady());
  EXPECT_GT(lineBuffer.charsReady(), 0);
  EXPECT_EQ("bcd", lineBuffer.getLine());

  EXPECT_EQ(0, lineBuffer.linesReady());
  ASSERT_GT(lineBuffer.charsReady(), 0);
  EXPECT_EQ('e', lineBuffer.getChar());

  EXPECT_EQ(0, lineBuffer.linesReady());
  ASSERT_GT(lineBuffer.charsReady(), 0);
  EXPECT_EQ('f', lineBuffer.getChar());

  EXPECT_EQ(0, lineBuffer.linesReady());
  EXPECT_EQ(0, lineBuffer.charsReady());
}

TEST(LineBufferTest, isSwappable) {
  shared_ptr<MockFileDescriptor> fd1 = make_shared<MockFileDescriptor>();
  shared_ptr<MockFileDescriptor> fd2 = make_shared<MockFileDescriptor>();
  LineBuffer lineBuffer1(fd1);
  LineBuffer lineBuffer2(fd2);

  {
    InSequence seq;

    EXPECT_CALL(*fd1, isReadyForReading())
      .WillOnce(Return(true));
    EXPECT_CALL(*fd1, read())
      .WillOnce(Return("abcd\nef"));
    EXPECT_CALL(*fd1, isReadyForReading())
      .WillRepeatedly(Return(false));
  }

  {
    InSequence seq;

    EXPECT_CALL(*fd2, isReadyForReading())
      .WillOnce(Return(true));
    EXPECT_CALL(*fd2, read())
      .WillOnce(Return("ijkl\nmn"));
    EXPECT_CALL(*fd2, isReadyForReading())
      .WillRepeatedly(Return(false));
  }

  ASSERT_GT(lineBuffer1.linesReady(), 0);
  EXPECT_EQ("abcd", lineBuffer1.getLine());
  EXPECT_EQ(0, lineBuffer1.linesReady());

  ASSERT_GT(lineBuffer2.linesReady(), 0);
  EXPECT_EQ("ijkl", lineBuffer2.getLine());
  EXPECT_EQ(0, lineBuffer2.linesReady());

  swap(lineBuffer1, lineBuffer2);

  {
    InSequence seq;

    EXPECT_CALL(*fd1, isReadyForReading())
      .WillOnce(Return(true));
    EXPECT_CALL(*fd1, read())
      .WillOnce(Return("gh\n"));
    EXPECT_CALL(*fd1, isReadyForReading())
      .WillRepeatedly(Return(false));
  }

  {
    InSequence seq;

    EXPECT_CALL(*fd2, isReadyForReading())
      .WillOnce(Return(true));
    EXPECT_CALL(*fd2, read())
      .WillOnce(Return("op\n"));
    EXPECT_CALL(*fd2, isReadyForReading())
      .WillRepeatedly(Return(false));
  }

  ASSERT_GT(lineBuffer1.linesReady(), 0);
  EXPECT_EQ("mnop", lineBuffer1.getLine());
  EXPECT_EQ(0, lineBuffer1.linesReady());

  ASSERT_GT(lineBuffer2.linesReady(), 0);
  EXPECT_EQ("efgh", lineBuffer2.getLine());
  EXPECT_EQ(0, lineBuffer2.linesReady());
}

TEST(LineBufferTest, isMovable) {
  shared_ptr<MockFileDescriptor> fd = make_shared<MockFileDescriptor>();
  LineBuffer lineBuffer1(fd), lineBuffer2;

  {
    InSequence seq;

    EXPECT_CALL(*fd, isReadyForReading())
      .WillOnce(Return(true));
    EXPECT_CALL(*fd, read())
      .WillOnce(Return("abcd\nef"));
    EXPECT_CALL(*fd, isReadyForReading())
      .WillRepeatedly(Return(false));
  }

  ASSERT_GT(lineBuffer1.linesReady(), 0);
  EXPECT_EQ("abcd", lineBuffer1.getLine());
  EXPECT_EQ(0, lineBuffer1.linesReady());

  lineBuffer2 = std::move(lineBuffer1);

  {
    InSequence seq;

    EXPECT_CALL(*fd, isReadyForReading())
      .WillOnce(Return(true));
    EXPECT_CALL(*fd, read())
      .WillOnce(Return("gh\n"));
    EXPECT_CALL(*fd, isReadyForReading())
      .WillRepeatedly(Return(false));
  }

  ASSERT_GT(lineBuffer2.linesReady(), 0);
  EXPECT_EQ("efgh", lineBuffer2.getLine());
  EXPECT_EQ(0, lineBuffer2.linesReady());
}
