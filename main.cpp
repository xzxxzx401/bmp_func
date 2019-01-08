#include <vector>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <getopt.h>

#include "defer.h"
#include "pic_file.h"
#include "bmp_pic.h"

struct Ops {
  enum Op {
    Lightness,
    Contrast,
    Zoom,
    Rotate,
    Mirror
  } op;
  union Opr {
    /**
     * double value, for lightness0, contrast1, zoom2
     */
    double d;
    /**
     * int value, for rotate3
     */
    uint64_t i;
    /**
     * struct value, for mirror4
     */
    struct S {
      bool h, v;
    } s;
  } opr;
};

int main(int argc, char **argv) {
  int opt;
  std::string in_path = "/Users/peter/Downloads/timg_83225.bmp";
  std::string out_path;
  std::vector<Ops> ops;
  int mirror_h = 0, mirror_v = 0;
  while ((opt = getopt(argc, argv, "i:o:l:c:z:r:hv")) != -1) {
    switch (opt) {
      case 'i': {
        in_path = optarg;
        break;
      }
      case 'o': {
        out_path = optarg;
        break;
      }
      case 'l': {
        auto v = atof(optarg);
        if (v < -2 || v > 2) {
          fprintf(stdout,
                  "adjust too many lightness(%lf) may not a good choice.\n", v);
        }
        Ops tmp;
        tmp.op = Ops::Op::Lightness, tmp.opr.d = v;
        ops.emplace_back(tmp);
        break;
      }
      case 'c': {
        auto v = atof(optarg);
        if (v < -2 || v > 2) {
          fprintf(stdout,
                  "adjust too many contrast(%lf) may not a good choice.\n", v);
        }
        Ops tmp;
        tmp.op = Ops::Op::Contrast, tmp.opr.d = v;
        ops.emplace_back(tmp);
        break;
      }
      case 'z': {
        auto v = atof(optarg);
        if (v < 0) {
          fprintf(stderr, "cannot zoom a negative number.(%lf)\n", v);
          exit(EXIT_FAILURE);
        }
        if (v < 0.25 || v > 4) {
          fprintf(stdout, "zoom too many(%lf) may not a good choice.\n", v);
        }
        Ops tmp;
        tmp.op = Ops::Op::Zoom, tmp.opr.d = v;
        ops.emplace_back(tmp);
        break;
      }
      case 'r': {
        auto v = atoll(optarg);
        int64_t k = v / 360;
        if (k < 0) v -= (k * 360 - 360);
        assert(v >= 0);
        if (v % 90) {
          fprintf(stderr, "must rotate +-90, +-180, +-270..., not(%lld)\n", v);
          exit(EXIT_FAILURE);
        }
        v = v % 360;
        Ops tmp;
        tmp.op = Ops::Op::Rotate, tmp.opr.i = static_cast<uint64_t >(v);
        ops.emplace_back(tmp);
        break;
      }
      case 'h': {
        mirror_h ^= 1;
        break;
      }
      case 'v': {
        mirror_v ^= 1;
        break;
      }
      default:
        goto usage;
    }
  }
  if (mirror_h || mirror_v) {
    Ops tmp;
    tmp.op = Ops::Op::Mirror, tmp.opr.s.h = (mirror_h != 0), tmp.opr.s.v = (
        mirror_v != 0);
    ops.emplace_back(tmp);
  }
  if (in_path.empty() || out_path.empty()) {
    usage:
    fprintf(stdout, "Bmp Func\n"
                    "Copyright 2018 Xiao Zhixuan, BUPT SCS 2015211255\n"
                    "<==============================================>\n"
                    "Usage: %s -i <InputFile> -o <OutputFile> [<Options>]\n\n"
                    "Options:\n"
                    "\t -l <lightness>     adjust lightness\n"
                    "\t -c <contrast>      adjust contrast\n"
                    "\t -z <zoom rate>     zoom(>1 for larger, <1 for smaller)\n"
                    "\t -r <rotate angle>  rotate\n"
                    "\t -h                 mirror horizontal\n"
                    "\t -v                 mirror vertical\n"
                    "\n\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  bmp_func::BmpPic bmp_pic(in_path);
  std::for_each(ops.begin(), ops.end(), [&](const Ops &op) {
    switch (op.op) {
      case Ops::Lightness: {
        bmp_pic.AdjustBrightness(op.opr.d);
        break;
      }
      case Ops::Contrast: {
        bmp_pic.AdjustContrast(op.opr.d);
        break;
      }
      case Ops::Zoom: {
        bmp_pic.Zoom(op.opr.d);
        break;
      }
      case Ops::Rotate: {
        bmp_pic.Rotate(static_cast<int32_t >(op.opr.i));
        break;
      }
      case Ops::Mirror: {
        bmp_pic.Mirror(op.opr.s.h, op.opr.s.v);
        break;
      }
    }
  });
  bmp_pic.Save(out_path);

  return 0;
}
