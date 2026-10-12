// RUN: %silencer_as_plugin %s | grep -v CHECK | FileCheck %s

// CHECK: int /*xs*/[10],
// expected-warning@+1 {{unused argument}}
int func(int xs[10], int y) {
  return y;
}
