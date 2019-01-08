//
// Created by 肖智轩 on 2018-12-21.
//

#include "bmp_pic.h"

#include <cmath>
#include <functional>

namespace bmp_func {

BmpPic::BmpPic(const std::string &path) {
  PicFile pic_file;
  pic_file.Load(path);
  mode_ = pic_file.Mode();
  // pic info head
  assert(pic_file.Get2Byte() == 0x4d42);
  pic_size_ = pic_file.Get4Byte();
  assert(pic_file.Get2Byte() == 0);
  assert(pic_file.Get2Byte() == 0);
  pic_offset_ = pic_file.Get4Byte();

  // bitmap info head
  bitmap_info_head_size_ = pic_file.Get4Byte();
  bitmap_width_ = pic_file.Get4Byte();
  if (bitmap_width_ & 0x8000) {
    bitmap_width_mask_ = 0xFFFF;
    bitmap_width_ = bitmap_width_ ^ bitmap_width_mask_;
  }
  bitmap_hidth_ = pic_file.Get4Byte();
  if (bitmap_hidth_ & 0x8000) {
    bitmap_hidth_mask_ = 0xFFFF;
    bitmap_hidth_ = bitmap_hidth_ ^ bitmap_hidth_mask_;
  }
  pic_.reserve(bitmap_hidth_ * bitmap_width_);
  assert(pic_file.Get2Byte() == 0x0001);
  pix_bit_ = pic_file.Get2Byte();
  if (pix_bit_ != 24 && pix_bit_ != 32) {
    fprintf(stderr, "pix_bit = %d, cannot handle.\n", pix_bit_);
    exit(EXIT_FAILURE);
  }
  zip_mode_ = pic_file.Get4Byte();
  bitmap_info_size_ = pic_file.Get4Byte();
  horizontal_resolution_ = pic_file.Get4Byte();
  vertical_resolution_ = pic_file.Get4Byte();
  bitmap_color_index_ = pic_file.Get4Byte();
  important_color_ = pic_file.Get4Byte();

  if (pix_bit_ == 24) {
    assert(bitmap_width_ * bitmap_hidth_ == pic_file.Remain() / 3);
    // pic
    for (auto i = 0; i < bitmap_hidth_; i++) {
      for (auto j = 0; j < bitmap_width_; j++) {
        pic_.emplace_back(ColorRGB(pic_file.GetByte(), pic_file.GetByte(), pic_file.GetByte()));
      }
      for (auto j = 0; (j + bitmap_width_ * 3) % 4 != 0; j++) {
        pic_file.GetByte();
      }
    }
  } else if (pix_bit_ == 32) {
    assert(bitmap_width_ * bitmap_hidth_ == pic_file.Remain() / 4);
    // pic
    for (auto i = 0; i < bitmap_hidth_; i++) {
      for (auto j = 0; j < bitmap_width_; j++) {
        pic_.emplace_back(ColorRGB(pic_file.GetByte(), pic_file.GetByte(), pic_file.GetByte()));
        pic_file.GetByte();
      }
    }
  } else {
    fprintf(stderr, "pix_byte is %d, cannot handle\n", pix_bit_);
    exit(EXIT_FAILURE);
  }
}

BmpPic::ColorRGB BmpPic::PixAt(double h, double w) const {
  auto w1 = static_cast<uint32_t>(floor(w)), w2 = static_cast<uint32_t>(ceil(w));
  auto h1 = static_cast<uint32_t>(floor(h)), h2 = static_cast<uint32_t>(ceil(h));
  w2 = w2 < this->bitmap_width_ ? w2 : this->bitmap_width_ - 1;
  h2 = h2 < this->bitmap_hidth_ ? h2 : this->bitmap_hidth_ - 1;
  return BmpPic::ColorRGB::AvgColor(
      BmpPic::ColorRGB::AvgColor(PixAt(h1, w1), PixAt(h2, w1)),
      BmpPic::ColorRGB::AvgColor(PixAt(h1, w2), PixAt(h2, w2)));
}

BmpPic::ColorRGB BmpPic::PixAt(uint32_t h, uint32_t w) const {
  assert(w >= 0 && w < bitmap_width_);
  assert(h >= 0 && h < bitmap_hidth_);
  assert(h * bitmap_width_ + w < pic_.size());
  return pic_[h * bitmap_width_ + w];
}

void BmpPic::Zoom(double rate) {
  this->Zoom(rate, rate);
}

void BmpPic::Zoom(double h_rate, double w_rate) {
  if (this->pix_bit_ != 24 && this->pix_bit_ != 32) {
    fprintf(stderr, "cannot zoom a pic with pix bit %d, 24 or 32 only.\n",
            this->pix_bit_);
    return;
  }
  auto w = static_cast<uint32_t >(this->bitmap_width_ * w_rate);
  auto h = static_cast<uint32_t >(this->bitmap_hidth_ * h_rate);
  std::vector<ColorRGB> new_color;
  new_color.reserve(w * h);

  for (auto i = 0; i < h; i++) {
    for (auto j = 0; j < w; j++) {
      new_color.emplace_back(this->PixAt(double(i) / h_rate, double(j) / w_rate));
    }
  }
  std::swap(this->pic_, new_color);
  this->pic_size_ = pix_bit_ / 8 * w * h + (pic_size_ - pix_bit_ / 8 * this->bitmap_hidth_ * this->bitmap_width_);
  this->bitmap_width_ = w;
  this->bitmap_hidth_ = h;
}

void BmpPic::Save(const std::string& path) {
  PicFile pic_file;
  // pic info head
  pic_file.Put2Byte(0x4d42);
  pic_file.Put4Byte(pic_size_);
  pic_file.Put2Byte(0);
  pic_file.Put2Byte(0);
  pic_file.Put4Byte(pic_offset_);

  // bitmap info head
  pic_file.Put4Byte(bitmap_info_head_size_);
  pic_file.Put4Byte(bitmap_width_ ^ bitmap_width_mask_);
  pic_file.Put4Byte(bitmap_hidth_ ^ bitmap_hidth_mask_);
  pic_file.Put2Byte(0x0001);
  pic_file.Put2Byte(pix_bit_);
  pic_file.Put4Byte(zip_mode_);
  pic_file.Put4Byte(bitmap_info_size_);
  pic_file.Put4Byte(horizontal_resolution_);
  pic_file.Put4Byte(vertical_resolution_);
  pic_file.Put4Byte(bitmap_color_index_);
  pic_file.Put4Byte(important_color_);

  if (pix_bit_ == 24) {
    assert(bitmap_width_ * bitmap_hidth_ == pic_.size());
    // pic
    for (auto i = 0; i < bitmap_hidth_; i++) {
      for (auto j = 0; j < bitmap_width_; j++) {
        pic_file.PutByte(pic_[i * bitmap_width_ + j].red_);
        pic_file.PutByte(pic_[i * bitmap_width_ + j].green_);
        pic_file.PutByte(pic_[i * bitmap_width_ + j].blue_);
      }
      for (auto j = 0; (j + bitmap_width_ * 3) % 4 != 0; j++) {
        pic_file.PutByte(0);
      }
    }
  } else if (pix_bit_ == 32) {
    assert(bitmap_width_ * bitmap_hidth_ == pic_.size());
    // pic
    for (auto i = 0; i < bitmap_hidth_; i++) {
      for (auto j = 0; j < bitmap_width_; j++) {
        pic_file.PutByte(pic_[i * bitmap_width_ + j].red_);
        pic_file.PutByte(pic_[i * bitmap_width_ + j].green_);
        pic_file.PutByte(pic_[i * bitmap_width_ + j].blue_);
        pic_file.PutByte(0);
      }
    }
  } else {
    fprintf(stderr, "pix_byte is %d, cannot handle\n", pix_bit_);
    exit(EXIT_FAILURE);
  }
  pic_file.Save(path, mode_);
}

void BmpPic::Rotate(int32_t degree) {
  degree %= 360;
  if (degree < 0) degree += 360;
  if (degree % 90 != 0) {
    fprintf(stderr, "can only rotate +-90, +-180, +-270 degree, not %d.",
            degree);
    return;
  }
  if (degree == 0) return;
  assert(degree == 90 || degree == 180 || degree == 270);
  switch (degree) {
    case 90: {
      std::vector<ColorRGB> new_color;
      uint32_t h = this->bitmap_width_;
      uint32_t w = this->bitmap_hidth_;
      new_color.reserve(this->bitmap_hidth_ * this->bitmap_width_);
      for (uint32_t i = 0; i < h; i++) {
        for (uint32_t j = 0; j < w; j++) {
          new_color.push_back(this->PixAt(w - 1 - j, i));
        }
      }
      std::swap(this->pic_, new_color);
      std::swap(this->bitmap_hidth_, this->bitmap_width_);
      break;
    }
    case 180: {
      std::vector<ColorRGB> new_color;
      uint32_t h = this->bitmap_hidth_;
      uint32_t w = this->bitmap_width_;
      new_color.reserve(this->bitmap_hidth_ * this->bitmap_width_);
      for (uint32_t i = 0; i < h; i++) {
        for (uint32_t j = 0; j < w; j++) {
          new_color.push_back(this->PixAt(this->bitmap_hidth_ - i - 1, this->bitmap_width_ - j - 1));
        }
      }
      std::swap(this->pic_, new_color);
      break;
    }
    case 270: {
      std::vector<ColorRGB> new_color;
      uint32_t h = this->bitmap_width_;
      uint32_t w = this->bitmap_hidth_;
      new_color.reserve(this->bitmap_hidth_ * this->bitmap_width_);
      for (uint32_t i = 0; i < h; i++) {
        for (uint32_t j = 0; j < w; j++) {
          new_color.push_back(this->PixAt(j, h - i - 1));
        }
      }
      std::swap(this->pic_, new_color);
      std::swap(this->bitmap_hidth_, this->bitmap_width_);
      break;
    }
    default:
      assert(false);
  }
}

void BmpPic::AdjustBrightnessContrast(double brightness, double contrast) {
  CalculateAvgColor();

  std::function <uint8_t(double)> f = [] (double d) { return d < 0 ? 0 :(d > 255 ? 255 : (static_cast<uint8_t> (d))); };
  std::for_each(pic_.begin(), pic_.end(), [&] (ColorRGB& t) {
    double r = t.red_, g = t.green_, b = t.blue_;
    r -= avg_r_; r *= contrast; r += (brightness * avg_r_);
    g -= avg_g_; g *= contrast; g += (brightness * avg_g_);
    b -= avg_b_; b *= contrast; b += (brightness * avg_b_);
    t.red_ = f(r);
    t.green_ = f(g);
    t.blue_ = f(b);
  });
}

void BmpPic::CalculateAvgColor() {
  avg_r_ = avg_b_ = avg_g_ = 0.0;
  std::for_each(pic_.begin(), pic_.end(), [&] (const ColorRGB& t) {
    avg_r_ += t.red_;
    avg_g_ += t.green_;
    avg_b_ += t.blue_;
  });
  avg_r_ /= pic_.size();
  avg_g_ /= pic_.size();
  avg_b_ /= pic_.size();
}

void BmpPic::Mirror(bool horizontal, bool vertical) {
  std::function<std::pair<uint32_t, uint32_t>(uint32_t, uint32_t)> f;
  int t = (static_cast<int>(horizontal) << 1) | (static_cast<int>(vertical));
  switch (t) {
    case 0: {
      return;
    }
    case 1: {
      f = [&] (uint32_t h, uint32_t w) { return std::make_pair(this->bitmap_hidth_ - 1 - h, w); };
      break;
    }
    case 2: {
      f = [&] (uint32_t h, uint32_t w) { return std::make_pair(h, this->bitmap_width_ - 1 - w); };
      break;
    }
    case 3: {
      f = [&] (uint32_t h, uint32_t w) { return std::make_pair(this->bitmap_hidth_ - 1 - h, this->bitmap_width_ - 1 - w); };
      break;
    }
    default: assert(false);
  }
  std::vector<ColorRGB> new_pic;
  for (uint32_t i = 0; i < this->bitmap_hidth_; i++) {
    for (uint32_t j = 0; j < this->bitmap_width_; j++) {
      auto tmp = f(i, j);
      new_pic.push_back(this->PixAt(tmp.first, tmp.second));
    }
  }
  std::swap(new_pic, this->pic_);
}

BmpPic::ColorRGB BmpPic::ColorRGB::AvgColor(BmpPic::ColorRGB a, BmpPic::ColorRGB b)  {
  auto red = static_cast<uint32_t>((a.red_ + b.red_) / 2);
  auto green = static_cast<uint32_t>((a.green_ + b.green_) / 2);
  auto blue = static_cast<uint32_t>((a.blue_ + b.blue_) / 2);

  return { static_cast<uint8_t>(red),
           static_cast<uint8_t>(green),
           static_cast<uint8_t>(blue) };
}

}