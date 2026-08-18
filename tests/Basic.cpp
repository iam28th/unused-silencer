// #include <utility>

int func(int x, int y);

int func(int x, int y) {
  int z = func(x, y);
  static int z1 = 0;

  x += y;

  int x1, y1;
  return x;
}

struct ClsWithInlineMethods {
  ClsWithInlineMethods(int x) {};
  int foo(int x) { return 0; }
};

struct Cls {
  Cls(int x);
  int foo(int x);
};
Cls::Cls(int x) {}
int Cls::foo(int x) { return 0; }

template <typename T> T templateFunc(int x) {
  int locvar;
  return T{};
}

int foo(int x) {
  if (int y = x + 1, *z = nullptr, w = 0; y > 0) {
    return y;
  }

  return x;
}
