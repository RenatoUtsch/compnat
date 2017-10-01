# Compnat

This repository stores the projects developed during the Natural Computing
course at UFMG

# Compiling

To compile, install [bazel](https://bazel.build/) and OpenMP on your system.
Then, run from the root directory:

```bash
$ bazel run -c opt compnat/tp1 -- <command line flags...>
```

The code used by this project needs a C++17 compliant compiler (preferably
clang 5.0.0+ or gcc 7.2.0+).

# Python 3 scripts
To run the python scripts, install the requirements.txt file in the script's
folder:

```bash
$ pip3 install -r requirements.txt
```
