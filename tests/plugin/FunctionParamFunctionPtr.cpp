// RUN: %silencer_as_plugin %s | grep -v CHECK | FileCheck %s

int func1(int (*f)(int), int y) {
  return (*f)(y);
}

// CHECK: int (*/*f*/)(int)
// expected-warning@+1 {{unused argument}}
int func2(int (*f)(int), int y) {
  return y;
}
