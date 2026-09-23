// RUN: %silencer_as_plugin %s | grep -v CHECK | FileCheck %s

int func(int x, int y);

int func(int x, int y) {
  {
    // CHECK: (void)z; (void)a; (void)b;
    // expected-warning@+1 {{unused variable(s)}}
    int z = func(x, y), a, b;
  }

  {
    // CHECK: (void)z; (void)b;
    // expected-warning@+1 {{unused variable(s)}}
    int z = func(x, y), a, *b = &a;
  }
  return x;
}
