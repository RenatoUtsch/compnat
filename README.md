# Compnat

This repository stores the projects developed during the Natural Computing
course at UFMG

# Compiling

To compile, install [bazel](https://bazel.build/). If on Linux, also install
OpenMP on your system. It's recommended to run this project on Linux to have the
performance improvement from multi-threading.

Then, run from the root directory:

```bash
$ bazel run -c opt compnat/tp2 -- --alsologtostderr <other command line flags...>
```

View `bazel run -c opt compnat/tp2 -- --help` for more information about the
flags. You're required to set at least `--dataset` and `--output_file`.

The code used by this project needs a C++17 compliant compiler (preferably
clang 5.0.0+ or gcc 7.2.0+).

# Python 3 scripts
To run the python scripts, install the requirements.txt file in the script's
folder before running them:

```bash
$ pip3 install -r requirements.txt
```
