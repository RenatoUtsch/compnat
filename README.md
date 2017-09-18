# Compnat

This repository stores the projects developed during the Natural Computing
course at UFMG

# Compiling

To compile, install both [bazel](https://bazel.build/) and
[glog](https://github.com/google/glog) on your system. Then, run from the root
directory:

```bash
$ bazel run compnat/tp1
```

The code used by this project needs a C++17 compliant compiler (preferably
clang 5.0.0+ or gcc 7.2.0+).
