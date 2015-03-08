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

#include <cstring>
#include <stdexcept>

#include <sndfile.h>

#include "Sound/SoundFile.h"

namespace MarathonKit {
namespace Sound {

SoundFile::SoundFile():
  mHandle(),
  mFrameCount(0),
  mChannelCount(0),
  mSampleRate(0) {}

SoundFile::SoundFile(const std::string& fileName):
  mHandle(),
  mFrameCount(0),
  mChannelCount(0),
  mSampleRate(0) {
  SF_INFO info;
  memset(&info, 0, sizeof info);
  mHandle = std::unique_ptr<void, Deleter>(
      sf_open(fileName.c_str(), SFM_READ, &info));
  if (mHandle == nullptr) {
    throw std::runtime_error(sf_strerror(nullptr));
  }
  mFrameCount = info.frames;
  mChannelCount = info.channels;
  mSampleRate = info.samplerate;
}

bool SoundFile::isOpen() const {
  return mHandle != nullptr;
}

long SoundFile::getFrameCount() const {
  return mFrameCount;
}

int SoundFile::getChannelCount() const {
  return mChannelCount;
}

int SoundFile::getSampleRate() const {
  return mSampleRate;
}

SoundFile::Frame SoundFile::getNextFrame() {
  std::vector<double> samples((size_t)mChannelCount, 0.0);
  long items = sf_read_double(
      (SNDFILE*) mHandle.get(),
      samples.data(),
      mChannelCount);
  if (items != mChannelCount) {
    throw std::runtime_error("");
  }
  return Frame(std::move(samples));
}

SoundFile::Frame::Frame(std::vector<double>&& samples):
  mSamples(samples) {}

double SoundFile::Frame::getSample(int channel_) {
  if (channel_ < 0) {
    throw std::runtime_error(
        "SoundFile::Frame::getSample called with a negative value");
  }
  size_t channel = (size_t) channel_;
  if (channel >= mSamples.size()) {
    throw std::runtime_error(
        "SoundFile::Frame::getSample called with too high channel number");
  }
  return mSamples[channel];
}

void SoundFile::Deleter::operator() (void* handle_) {
  SNDFILE* handle = (SNDFILE*) handle_;
  sf_close(handle);
}

}}
