// #include <utility>

int func(int x, int y);

int func(int x, int y) {
  int z = func(x, y);
  static int z1 = 0;

  x += y;
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
  if (int y = x + 1, *z = nullptr; (void)x, (void)z, y > 0) {
    return y;
  }

  if (1 > 0) {
  }
  return x;
}
