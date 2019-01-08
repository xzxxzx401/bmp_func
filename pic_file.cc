//
// Created by 肖智轩 on 2018-12-21.
//

#include "pic_file.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

#include "defer.h"

namespace bmp_func {

void PicFile::Load(const std::string& path) {
  struct stat stat_buf;
  if (stat(path.c_str(), &stat_buf) == -1) {
    perror("stat");
    exit(EXIT_FAILURE);
  }
  switch (stat_buf.st_mode & S_IFMT) {
    case S_IFREG:
      break;
    default: {
      fprintf(stderr, "Please set path as a regularly file! path:%s\n",
              path.c_str());
      exit(EXIT_FAILURE);
    }
  }
  uint64_t size = stat_buf.st_size;
  pic_bytes_.resize(size);
  mode_ = stat_buf.st_mode;

  int fd = open(path.c_str(), O_RDONLY);
  if (fd == -1) {
    perror("open");
    exit(EXIT_FAILURE);
  }
  DEFER([&]() { close(fd); });
  read(fd, pic_bytes_.data(), pic_bytes_.size());
}

uint8_t PicFile::GetByte() {
  assert(pos_ <= pic_bytes_.size());
  uint8_t ret = pic_bytes_[pos_];
  pos_++;
  return ret;
}

uint16_t PicFile::Get2Byte() {
  uint16_t ret = GetByte();
  ret |= (static_cast<uint64_t >(GetByte()) << 8);
  return ret;
}

uint32_t PicFile::Get4Byte() {
  uint32_t ret = GetByte();
  ret |= (static_cast<uint64_t >(GetByte()) << 8);
  ret |= (static_cast<uint64_t >(GetByte()) << 16);
  ret |= (static_cast<uint64_t >(GetByte()) << 24);
  return ret;
}

uint64_t PicFile::Get8Byte() {
  uint64_t ret = GetByte();
  ret |= (static_cast<uint64_t >(GetByte()) << 8);
  ret |= (static_cast<uint64_t >(GetByte()) << 16);
  ret |= (static_cast<uint64_t >(GetByte()) << 24);
  ret |= (static_cast<uint64_t >(GetByte()) << 32);
  ret |= (static_cast<uint64_t >(GetByte()) << 40);
  ret |= (static_cast<uint64_t >(GetByte()) << 48);
  ret |= (static_cast<uint64_t >(GetByte()) << 56);
  return ret;
}

std::vector<char> PicFile::GetNBytes(size_t n) {
  assert(pos_ + n < pic_bytes_.size());
  std::vector<char> vec;
  std::copy(pic_bytes_.begin() + pos_, pic_bytes_.begin() + (pos_ + n), vec.begin());
  pos_ += n;
  return vec;
}

void PicFile::PutByte(uint8_t byte) {
  pic_bytes_.push_back(byte);
}

void PicFile::Put2Byte(uint16_t byte) {
  uint8_t tmp = byte & 0xFF;
  pic_bytes_.push_back(tmp);
  byte >>= 8;
  tmp = byte & 0xFF;
  pic_bytes_.push_back(tmp);
}

void PicFile::Put4Byte(uint32_t byte) {
  uint8_t tmp = byte & 0xFF;
  pic_bytes_.push_back(tmp);
  byte >>= 8;
  tmp = byte & 0xFF;
  pic_bytes_.push_back(tmp);
  byte >>= 8;
  tmp = byte & 0xFF;
  pic_bytes_.push_back(tmp);
  byte >>= 8;
  tmp = byte & 0xFF;
  pic_bytes_.push_back(tmp);

}

void PicFile::Put8Byte(uint64_t byte) {
  uint8_t tmp = byte & 0xFF;
  pic_bytes_.push_back(tmp);
  byte >>= 8;
  tmp = byte & 0xFF;
  pic_bytes_.push_back(tmp);
  byte >>= 8;
  tmp = byte & 0xFF;
  pic_bytes_.push_back(tmp);
  byte >>= 8;
  tmp = byte & 0xFF;
  pic_bytes_.push_back(tmp);
  byte >>= 8;
  tmp = byte & 0xFF;
  pic_bytes_.push_back(tmp);
  byte >>= 8;
  tmp = byte & 0xFF;
  pic_bytes_.push_back(tmp);
  byte >>= 8;
  tmp = byte & 0xFF;
  pic_bytes_.push_back(tmp);
  byte >>= 8;
  tmp = byte & 0xFF;
  pic_bytes_.push_back(tmp);
}

void PicFile::Save(const std::string &path, const mode_t& mode) {
  struct stat stat_buf;
  if (stat(path.c_str(), &stat_buf) != -1) {
    fprintf(stderr, "file exist!\n");
    exit(EXIT_FAILURE);
  }

  int fd = creat(path.c_str(), mode);
  if (fd == -1) {
    perror("create");
    exit(EXIT_FAILURE);
  }
  DEFER([&]() { close(fd); });
  write(fd, pic_bytes_.data(), pic_bytes_.size());
}

}