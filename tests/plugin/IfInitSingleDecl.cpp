// RUN: %silencer_as_plugin %s | grep -v CHECK | FileCheck %s

int makeX();

int func() {
  // expected-warning@+2 {{unused variable}}
  // CHECK: (void)x,
  if (int x = makeX(); true)
    ;

  return 0;
}
