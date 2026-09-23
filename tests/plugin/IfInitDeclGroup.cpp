// RUN: %silencer_as_plugin %s | grep -v CHECK | FileCheck %s

int makeX();

int func() {
  // expected-warning@+2 {{unused variable(s)}}
  // CHECK: (void)x, (void)y, (void)z,
  if (int x = makeX(), y = 0, z; true)
    ;

  // expected-warning@+2 {{unused variable(s)}}
  // CHECK: (void)y, (void)z,
  if (int x = makeX(), y = 0, *z = &x; true)
    ;

  return 0;
}
