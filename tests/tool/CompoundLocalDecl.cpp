// RUN: silencer %s 2>&1 -- | FileCheck -implicit-check-not warning: %s

// COM: can also do this to specify line number: CHECK: {{.*}}:[[@LINE+1]]:{{.*}}: warning: unused variable
// COM: ...but syntax is ass

int func(int x, int y);

int func(int x, int y) {
  // CHECK: warning: unused variable
  // CHECK: (void)z;
  int z = func(x, y);
  return x;
}
