// COM: because plugin prints input file %s, including comments, to stdout,
// COM: we need to strip lines with CHECK to prevent them matching themselves
// RUN: %silencer_as_plugin %s | grep -v CHECK | FileCheck %s

int func(int x, int y);

int func(int x, int y) {
  // CHECK: (void)z;
  // expected-warning@+1 {{unused variable}}
  int z = func(x, y);

  // CHECK: (void)z1;
  // expected-warning@+1 {{unused variable}}
  static int z1 = 0;

  x += y;

  // CHECK: (void)x1;
  // CHECK: (void)y1;
  // expected-warning@+2 {{unused variable}}
  // expected-warning@+1 {{unused variable}}
  int x1, y1;
  return x;
}

int declInIf(int x) {
  // CHECK: if (int y = x + 1, *z = nullptr, w = 0; (void)z, (void)w, y > 0) {
  // z, w are unused
  // expected-warning@+2 {{unused variable}}
  // expected-warning@+1 {{unused variable}}
  if (int y = x + 1, *z = nullptr, w = 0; y > 0) {
    return y;
  }
  return x;
}
