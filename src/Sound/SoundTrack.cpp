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

#include <cmath>
#include <cstring>
#include <stdexcept>

#include <sndfile.h>

#include "Sound/SoundFile.h"

#include "Sound/SoundTrack.h"

namespace MarathonKit {
namespace Sound {

SoundTrack::SoundTrack():
  mSamples(),
  mSampleRate(0) {}

SoundTrack::SoundTrack(const std::vector<double>& samples, int sampleRate):
  mSamples(samples),
  mSampleRate(sampleRate) {
  if (mSampleRate < 0) {
    throw std::runtime_error("Sample rate cannot be negative");
  }
}

SoundTrack::SoundTrack(std::vector<double>&& samples, int sampleRate):
  mSamples(std::move(samples)),
  mSampleRate(sampleRate) {
  if (mSampleRate < 0) {
    throw std::runtime_error("Sample rate cannot be negative");
  }
}

int SoundTrack::getSampleCount() const {
  return (int)mSamples.size();
}

int SoundTrack::getSampleRate() const {
  return mSampleRate;
}

double SoundTrack::getSample(int i) const {
  if (i < 0 || (std::size_t)i >= mSamples.size()) {
    throw std::runtime_error("Sample index is out of range");
  }
  return mSamples[(std::size_t)i];
}

double SoundTrack::getSampleWithoutThrowing(int i) const {
  if (i < 0 || (std::size_t)i >= mSamples.size()) {
    return 0.0;
  }
  return mSamples[(std::size_t)i];
}

double SoundTrack::getDuration() const {
  checkSampleRateKnown();
  return (double)mSamples.size() / mSampleRate;
}

int SoundTrack::getSampleIndex(double time) const {
  checkSampleRateKnown();
  double index = time * mSampleRate;
  return (int)std::round(index);
}

std::vector<SoundTrack> SoundTrack::loadFromFile(const std::string& fileName) {
  SoundFile file(fileName);
  std::vector<std::vector<double>> channels;

  for (int channel = 0; channel < file.getChannelCount(); ++channel) {
    channels.emplace_back();
  }

  for (int frameI = 0; frameI < file.getFrameCount(); ++frameI) {
    SoundFile::Frame frame = file.getNextFrame();
    for (int channelI = 0; channelI < file.getChannelCount(); ++channelI) {
      channels[(size_t)channelI].push_back(frame.getSample(channelI));
    }
  }

  std::vector<SoundTrack> tracks;
  for (std::vector<double>& samples : channels) {
    tracks.emplace_back(std::move(samples), file.getSampleRate());
  }
  return tracks;
}

void SoundTrack::checkSampleRateKnown() const {
  if (mSampleRate == 0) {
    throw std::runtime_error("Unknown sample rate");
  }
}

}}
