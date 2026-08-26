// RUN: %silencer_as_plugin %s
// COM: RUN: %silencer_as_plugin %s | grep -v CHECK | FileCheck %s

int makeX();

int func() {
  // expected-warning@+2 {{unused variable}}
  if (true)
    int x = makeX();

  if (int x = 0; x != 28)
    ++x;

  // expected-warning@+2 {{unused variable}}
  if (int y = 0; y == 0)
    int x = makeX();

  return 0;
}
