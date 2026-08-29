## (unused) silencer

An out-of-tree clang plugin and a tool that finds unused variables and performs 2 kinds of transformation:

1) Comments out unused function arguments:
```cpp
int first(int a, int b) { return a; }
->
int first(int a, int /*b*/) { return a; }
```
2) Casts unused local variables to `void`:
```cpp
// 2.1 simple case
int hello() {
    int ret = printf("hello world");
}
->
int hello() {
    int ret = printf("hello world");
    (void)ret;
}


// 2.2 declaration in if condition
if (int x = 0; condition) {
    ...
}
->
if (int x = 0; (void)x, condition) {
    ...
}

// 2.3 declaration in if body without compound statement
if (condition)
    int ret = printf("hello world");
->
if (condition)
    { int ret = printf("hello world"); (void)ret; }  // formatting is out of scope...
```

Made for educational purposes.

See Makefile for building rules.
