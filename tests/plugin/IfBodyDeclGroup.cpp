// RUN: %silencer_as_plugin %s | grep -v CHECK | FileCheck %s

int makeX();

int func() {
  if (true)
    // expected-warning@+2 {{unused variable(s)}}
    // CHECK: { int x = makeX(), y; (void)x; (void)y; }
    int x = makeX(), y;
  else
    // expected-warning@+2 {{unused variable(s)}}
    // CHECK: { int x = makeX(), y; (void)x; (void)y; }
    int x = makeX(), y;

  if (true)
    // expected-warning@+2 {{unused variable(s)}}
    // CHECK: { int x = makeX(), y, *z = &x; (void)y; (void)z; }
    int x = makeX(), y, *z = &x;


  if (int x = 0, y = 0; x != 28)
    ++x, ++y;

  return 0;
}
