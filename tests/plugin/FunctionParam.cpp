// COM: because plugin prints input file %s, including comments, to stdout,
// COM: we need to strip lines with CHECK to prevent them matching themselves
// RUN: %silencer_as_plugin %s | grep -v CHECK | FileCheck %s

int func(int x, int y);

// CHECK: int func(int x, int /*y*/, int /*w*/)
// expected-warning@+2 {{unused argument}}
// expected-warning@+1 {{unused argument}}
int func(int x, int y, int w) {
  return x;
}

struct ClsWithInlineMethods {
  // ClsWithInlineMethods(int /*x*/) {};
  // expected-warning@+1 {{unused argument}}
  ClsWithInlineMethods(int x) {};
  // int foo(int /*x*/) {};
  // expected-warning@+1 {{unused argument}}
  int foo(int x) { return 0; }
};

struct Cls {
  Cls(int x);
  int foo(int x);
};

// CHECK: Cls::Cls(int /*x*/)
// expected-warning@+1 {{unused argument}}
Cls::Cls(int x) {}
// CHECK: int Cls::foo(int /*x*/)
// expected-warning@+1 {{unused argument}}
int Cls::foo(int x) { return 0; }
