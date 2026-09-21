// RUN: %silencer_as_plugin %s | grep -v CHECK | FileCheck %s

int func(int x) {
  // CHECK: (void)z1;
  // expected-warning@+1 {{unused variable}}
  static int z1 = 0;
  return x;
}
