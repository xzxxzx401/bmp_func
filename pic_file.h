//
// Created by 肖智轩 on 2018-12-21.
//

#ifndef BMP_FUNC_PIC_FILE_H
#define BMP_FUNC_PIC_FILE_H

#include <vector>
#include <string>

namespace bmp_func {

class PicFile {
 private:
  std::vector<char> pic_bytes_;
  uint64_t pos_ {0};
  mode_t mode_;

 public:
  PicFile() {}
  void Load(const std::string& path);
  void Save(const std::string& path, const mode_t& mode);

  uint8_t GetByte();
  uint16_t Get2Byte();
  uint32_t Get4Byte();
  uint64_t Get8Byte();
  std::vector<char> GetNBytes(size_t n);
  uint64_t Remain() const {
    return pic_bytes_.size() - pos_;
  }
  void PutByte(uint8_t byte);
  void Put2Byte(uint16_t byte);
  void Put4Byte(uint32_t byte);
  void Put8Byte(uint64_t byte);

  mode_t Mode() const { return mode_; }
};

}
#endif //BMP_FUNC_PIC_FILE_H
