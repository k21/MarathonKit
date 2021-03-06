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

#ifndef MARATHON_KIT_SOUND_SOUND_FILE_H_
#define MARATHON_KIT_SOUND_SOUND_FILE_H_

#include <memory>
#include <string>
#include <vector>

namespace MarathonKit {
namespace Sound {

class SoundFile {
public:

  SoundFile();
  SoundFile(const std::string& fileName);

  bool isOpen() const;

  long getFrameCount() const;
  int getChannelCount() const;
  int getSampleRate() const;

  class Frame {
  public:

    Frame(std::vector<double>&& samples);

    double getSample(int channel);

  private:

    std::vector<double> mSamples;

  };

  Frame getNextFrame();

private:

  class Deleter {
  public:

    void operator() (void* handle);

  };

  std::unique_ptr<void, Deleter> mHandle;
  long mFrameCount;
  int mChannelCount;
  int mSampleRate;

};

}}

#endif
