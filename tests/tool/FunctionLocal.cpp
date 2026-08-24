// RUN: silencer %s 2>&1 -- | FileCheck -implicit-check-not warning: %s

// COM: can also do this to specify line number: CHECK: {{.*}}:[[@LINE+1]]:{{.*}}: warning: unused variable
// COM: ...but syntax is ass

int func(int x, int y);

int func(int x, int y) {
  // CHECK: warning: unused variable
  // CHECK: (void)z;
  int z = func(x, y);

  // CHECK: warning: unused variable
  // CHECK: (void)z1;
  static int z1 = 0;

  x += y;

  // CHECK: warning: unused variable
  // CHECK: (void)x1;
  // CHECK: warning: unused variable
  // CHECK: (void)y1;
  int x1, y1;
  return x;
}

int declInIf(int x) {
  // CHECK: warning: unused variable
  // CHECK: (void)z,
  // CHECK: warning: unused variable
  // CHECK: (void)w,
  if (int y = x + 1, *z = nullptr, w = 0; y > 0) {
    return y;
  }
  return x;
}
