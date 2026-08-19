// RUN: %silencer_as_plugin %s | grep -v CHECK | FileCheck %s

// CHECK: templateFunc(int /*x*/)
// expected-warning@+1 {{unused argument}}
template <typename T> T templateFunc(int x) {
  // CHECK: (void)locvar;
  // expected-warning@+1 {{unused variable}}
  int locvar;
  return T{};
}
