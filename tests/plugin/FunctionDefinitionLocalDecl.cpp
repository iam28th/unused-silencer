// RUN: %silencer_as_plugin %s | grep -v CHECK | FileCheck %s

int func(int x, int y);

int func(int x, int y) {
  // CHECK: (void)z;
  // expected-warning@+1 {{unused variable}}
  int z = func(x, y);
  return x;
}
