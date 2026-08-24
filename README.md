An out-of-tree clang plugin and a tool that performs 2 kinds of transformation:

1) Comments out unused function arguments:
```cpp
int first(int a, int b) { return a; }
->
int first(int a, int /*b*/) { return a; }
```
2) Casts unused local variables to `void`:
```cpp
int hello() {
    int ret = printf("hello world");
}
->
int hello() {
    int ret = printf("hello world");
    void (ret);
}
```

Made for educational purposes.

See Makefile for building rules.
