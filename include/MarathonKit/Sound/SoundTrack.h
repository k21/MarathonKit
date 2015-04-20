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

#ifndef MARATHON_KIT_SOUND_SOUND_TRACK_H_
#define MARATHON_KIT_SOUND_SOUND_TRACK_H_

#include <string>
#include <vector>

namespace MarathonKit {
namespace Sound {

class SoundTrack {
public:

  SoundTrack();
  SoundTrack(const std::vector<double>& samples, int sampleRate);
  SoundTrack(std::vector<double>&& samples, int sampleRate);

  int getSampleCount() const;
  int getSampleRate() const;

  double getSample(int i) const;
  double getSampleWithoutThrowing(int i) const;

  double getDuration() const;
  int getSampleIndex(double time) const;

  static std::vector<SoundTrack> loadFromFile(const std::string& fileName);

private:

  void checkSampleRateKnown() const;

  std::vector<double> mSamples;
  int mSampleRate;

};

}}

#endif
