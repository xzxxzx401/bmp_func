#ifndef BMP_FUNC_DEFER_H
#define BMP_FUNC_DEFER_H

#include <functional>

namespace bmp_func {

class Defer {
 public:
  explicit Defer(std::function<void()> run) : run_(run) {
  }
  ~Defer() {
    run_();
  }

  Defer(const Defer &) = delete;
  Defer& operator=(const Defer &) = delete;

 private:
  std::function<void()> run_;
};

#define __DEFER(salt, func) Defer __##salt##_defer((func))
#define _DEFER(salt, func) __DEFER(salt, (func))
#define DEFER(func) _DEFER(__LINE__, (func))

}

#endif //BMP_FUNC_DEFER_H
