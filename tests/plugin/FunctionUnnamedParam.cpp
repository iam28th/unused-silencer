// RUN: %silencer_as_plugin %s

int func(int) {
  return 0;
}

// expected-no-diagnostics
