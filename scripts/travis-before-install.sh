#!/bin/bash
set -eux

add_llvm_repository() {
  local VERSION="$1"
  local URL="http://llvm.org/apt/trusty/"
  local DIST="llvm-toolchain-trusty-${VERSION}"

  # Downloading a key over insecure connection...
  # Do not do this on any machine you like.
  wget -O - http://llvm.org/apt/llvm-snapshot.gpg.key | sudo apt-key add -

  sudo add-apt-repository "deb ${URL} ${DIST} main"
}

escape() {
  sed "s/'/'\\\\''/g;s/.*/'&'/"
}

install_tool() {
  local SOURCE="$1"
  local TARGET="$2"
  {
    echo "#!/bin/sh"
    echo "exec $(echo "${SOURCE}" | escape) "'"$@"'
  } >"${TARGET}"
  chmod +x "${TARGET}"
  cat "${TARGET}"
}

setup_linux() {
  if [ "x${CXX}" = "xg++" ]
  then
    sudo add-apt-repository ppa:ubuntu-toolchain-r/test -y
  elif [ "x${CXX}" = "xclang++" ]
  then
    add_llvm_repository "${CLANG_VERSION}"
  else
    echo "Unknown CXX: ${CXX}" >&2
    exit 1
  fi

  sudo apt-get update -qq
  sudo apt-get install -qq libsndfile-dev

  if [ "x${CXX}" = "xg++" ]
  then
    sudo apt-get install -qq "g++-${GCC_VERSION}"
    install_tool "/usr/bin/gcc-${GCC_VERSION}" ~/bin/cc
    install_tool "/usr/bin/g++-${GCC_VERSION}" ~/bin/cxx
  elif [ "x${CXX}" = "xclang++" ]
  then
    sudo apt-get install -qq "clang-${CLANG_VERSION}"
    install_tool "/usr/bin/clang-${CLANG_VERSION}" ~/bin/cc
    install_tool "/usr/bin/clang++-${CLANG_VERSION}" ~/bin/cxx
  fi
}

setup_osx() {
  brew update
  brew install libsndfile
  if [ "x${CXX}" = "xg++" ]
  then
    install_tool "$(which gcc)" ~/bin/cc
    install_tool "$(which g++)" ~/bin/cxx
  elif [ "x${CXX}" = "xclang++" ]
  then
    install_tool "$(which clang)" ~/bin/cc
    install_tool "$(which clang++)" ~/bin/cxx
  else
    echo "Unknown CXX: ${CXX}" >&2
    exit 1
  fi
}

mkdir -p ~/bin

if [ "x${TRAVIS_OS_NAME}" = 'xosx' ]
then
  setup_osx
elif [ "x${TRAVIS_OS_NAME}" = 'xlinux' ]
then
  setup_linux
else
  echo "Unknown TRAVIS_OS_NAME: ${TRAVIS_OS_NAME}" >&2
  exit 1
fi
