// RUN: %silencer_as_plugin %s | grep -v CHECK | FileCheck %s

int makeX();

int func() {
  if (true)
    // expected-warning@+2 {{unused variable}}
    // CHECK: { int x = makeX(); (void)x; }
    int x = makeX();
  else
    // expected-warning@+2 {{unused variable}}
    // CHECK: { int x = makeX(); (void)x; }
    int x = makeX();

  if (int x = 0; x != 28)
    ++x;

  return 0;
}
