int func(int x, int y) {
  x += y;
  return x;
}

// int func(int x, int y, int z) { // z isn't used, should flag
//   x += y;
//   return x;
// }
//
// int func2(int x, int y); // declaration only, shouldn't flag
//
// int main() {
//   int a = 0; // a is used
//
//   int b = a + 1; // b is not, so should add std::ignore b below
//   return 0;
// }
