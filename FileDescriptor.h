#ifndef MARATHON_KIT_FILE_DESCRIPTOR_H_
#define MARATHON_KIT_FILE_DESCRIPTOR_H_

#include <memory>
#include <set>
#include <string>

namespace MarathonKit {

class FileDescriptor {
public:

  FileDescriptor();
  FileDescriptor(const FileDescriptor& other);
  FileDescriptor& operator = (const FileDescriptor& other);
  FileDescriptor(FileDescriptor&& other);
  FileDescriptor& operator = (FileDescriptor&& other);
  ~FileDescriptor();

  void swapWith(FileDescriptor& other);

  bool isValid() const;

  std::string read() const;
  void write(const std::string& data) const;

  static FileDescriptor createOwnerOf(int fd);
  static FileDescriptor createCopyOf(int fd);

  class Set {
  public:

    void add(const FileDescriptor& fd);
    bool contains(const FileDescriptor& fd) const;

    void selectReadable();

  private:

    std::set<int> fds;

  };

private:

  explicit FileDescriptor(int fd);

  int mFd;

};

void swap(FileDescriptor& fd1, FileDescriptor& fd2);

}

#endif
