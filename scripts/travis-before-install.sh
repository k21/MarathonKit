#!/bin/sh
set -eux

sudo add-apt-repository ppa:ubuntu-toolchain-r/test -y
sudo apt-get update -qq
sudo apt-get install -qq libsndfile-dev
if [ "x$CXX" = "xg++" ]
then
  sudo apt-get install -qq g++-4.8
  sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-4.8 90
fi
