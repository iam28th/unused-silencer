A clang plugin and a tool that performs 2 kinds of transformation:
1) comments out unused function arguments:
```cpp
int first(int a, int b) { return a; }
->
int first(int a, int /*b*/) { return a; }
```
2) assignes unused local variables to `std::ignore`:
```cpp
int hello() {
    int ret = printf("hello world");
}
->
int hello() {
    int ret = printf("hello world");
    std::ignore = ret;
}
```
2.1) check if `std::ignore` is available; include <utility> if it's not

P.S. I'm actually curious how the second substitution will behave in these cases:
```cpp
// 1
if (condition)
    int ret = printf("hello world");

// 2
if (condition) { int ret = printf("hello world") }

// 3
if (int ret = printf("hello"); condition)
    printf(" world");
```
