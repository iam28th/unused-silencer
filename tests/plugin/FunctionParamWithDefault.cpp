// RUN: %silencer_as_plugin %s | grep -v CHECK | FileCheck %s

// CHECK: int /*x*/ = 10
// expected-warning@+1 {{unused argument}}
int func(int x = 10) {
  return 0;
}
