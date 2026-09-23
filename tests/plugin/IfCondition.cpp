// RUN: %silencer_as_plugin %s
int makeX();

int func() {
  if (int x = 0)
    ;

  return 0;
}

// expected-no-diagnostics
