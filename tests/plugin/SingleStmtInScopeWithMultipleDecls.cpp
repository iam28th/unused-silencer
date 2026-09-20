// RUN: %silencer_as_plugin %s | grep -v CHECK | FileCheck %s

int printf(const char*);

int x1, y1;

int func() {
  if (true)
    int x = printf("if "), y = printf("branch");
  else
    int x = printf("else "), y = printf("branch");


  if (true) {
    int xComp = printf("hello "), yComp = printf("world");
  }

  if (int xInIfConditionVariable = 0) // this shall always be used
    ;

  if (int xInInitStatement = 0, yInInitStatement; false)
    ;

  if (true)
    int xInIfBody = 0;

  int x1, y1, z1;
  (void)z1;

  return 0;
}
