//
// Created by 肖智轩 on 2018-12-21.
//

#ifndef BMP_FUNC_BMP_PIC_H
#define BMP_FUNC_BMP_PIC_H

#include "pic_file.h"

#include <string>
#include <cmath>

namespace bmp_func {

class BmpPic {

  struct ColorRGB {
    uint8_t red_{0};
    uint8_t green_{0};
    uint8_t blue_{0};

    ColorRGB(uint8_t r, uint8_t g, uint8_t b) : red_(r), green_(g), blue_(b) {}
    static ColorRGB AvgColor(ColorRGB a, ColorRGB b);
  };

 public:
  BmpPic(const std::string &path);
  uint32_t GetWidth() const { return bitmap_width_; };
  uint32_t GetHidth() const { return bitmap_hidth_; };
  void Zoom(double rate);
  void Zoom(double w_rate, double h_rate);
  void Save(const std::string& path);
  void Rotate(int32_t degree);
  void AdjustBrightnessContrast(double brightness, double contrast);
  void AdjustBrightness(double brightness) { AdjustBrightnessContrast(brightness, 1.0); }
  void AdjustContrast(double contrast) { AdjustBrightnessContrast(1.0, contrast); }
  void Mirror(bool horizontal, bool vertical);
  void MirrorVertical() { Mirror(false, true); }
  void Mirrorhorizontal() { Mirror(true, false); }

 private:
  ColorRGB PixAt(double w, double h) const;
  ColorRGB PixAt(uint32_t w, uint32_t h) const;
  void CalculateAvgColor();


 private:
  mode_t mode_;
  // pic info head
  uint32_t pic_size_;
  uint32_t pic_offset_;

  // bitmap info head
  uint32_t bitmap_info_head_size_;
  uint32_t bitmap_width_;
  uint32_t bitmap_width_mask_;
  uint32_t bitmap_hidth_;
  uint32_t bitmap_hidth_mask_;
  uint16_t pix_bit_;
  uint32_t zip_mode_;
  uint32_t bitmap_info_size_;
  uint32_t horizontal_resolution_;
  uint32_t vertical_resolution_;
  uint32_t bitmap_color_index_;
  uint32_t important_color_;

  // palette
  std::vector<ColorRGB> palette_;

  // pic data
  std::vector<ColorRGB> pic_;

 private:
  double avg_r_ {NAN};
  double avg_g_ {NAN};
  double avg_b_ {NAN};
};

}

#endif //BMP_FUNC_BMP_PIC_H
