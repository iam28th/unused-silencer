// COM: RUN: %silencer_as_plugin %s | grep -v CHECK | FileCheck %s

int makeX();

int func() {
  if (true)
    int x = makeX();

  if (true)
    int x = 0, y, z = x;

  return 0;
}
