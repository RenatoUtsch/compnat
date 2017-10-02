# Copyright 2017 Renato Utsch
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

load("//compnat:defs.bzl", "COMPNAT_CPP_COPTS", "COMPNAT_CPP_LINKOPTS")

package(default_visibility = ["//visibility:private"])

cc_binary(
    name = "tp1",
    srcs = ["tp1.cpp"],
    copts = COMPNAT_CPP_COPTS,
    linkopts = COMPNAT_CPP_LINKOPTS,
    deps = [
        ":parser",
        ":primitives",
        ":representation",
        ":simulation",
    ],
)

cc_library(
    name = "generators",
    srcs = ["generators.cpp"],
    hdrs = ["generators.hpp"],
    copts = COMPNAT_CPP_COPTS,
    deps = [
        ":representation",
    ],
)

cc_test(
    name = "generators_test",
    size = "small",
    srcs = ["generators_test.cpp"],
    copts = COMPNAT_CPP_COPTS,
    linkopts = COMPNAT_CPP_LINKOPTS,
    deps = [
        ":generators",
        ":primitives",
        ":utils",
        "//third_party:gtest",
    ],
)

cc_library(
    name = "operators",
    srcs = ["operators.cpp"],
    hdrs = ["operators.hpp"],
    copts = COMPNAT_CPP_COPTS,
    deps = [
        ":generators",
        ":representation",
        ":statistics",
    ],
)

cc_test(
    name = "operators_test",
    size = "small",
    srcs = ["operators_test.cpp"],
    copts = COMPNAT_CPP_COPTS,
    data = ["//compnat/tp1/datasets"],
    linkopts = COMPNAT_CPP_LINKOPTS,
    deps = [
        ":generators",
        ":operators",
        ":parser",
        ":primitives",
        ":statistics",
        "//third_party:gtest",
    ],
)

cc_library(
    name = "parser",
    srcs = ["parser.cpp"],
    hdrs = ["parser.hpp"],
    copts = COMPNAT_CPP_COPTS,
    deps = [
        ":representation",
        ":utils",
    ],
)

cc_test(
    name = "parser_test",
    size = "small",
    srcs = ["parser_test.cpp"],
    copts = COMPNAT_CPP_COPTS,
    data = ["//compnat/tp1/datasets"],
    linkopts = COMPNAT_CPP_LINKOPTS,
    deps = [
        ":parser",
        "//third_party:gmock",
        "//third_party:gtest",
    ],
)

cc_library(
    name = "primitives",
    srcs = ["primitives.cpp"],
    hdrs = ["primitives.hpp"],
    copts = COMPNAT_CPP_COPTS,
    deps = [
        ":representation",
        ":utils",
    ],
)

cc_test(
    name = "primitives_test",
    size = "small",
    srcs = ["primitives_test.cpp"],
    copts = COMPNAT_CPP_COPTS,
    linkopts = COMPNAT_CPP_LINKOPTS,
    deps = [
        ":primitives",
        ":representation",
        "//third_party:gtest",
    ],
)

cc_library(
    name = "representation",
    hdrs = ["representation.hpp"],
    copts = COMPNAT_CPP_COPTS,
    deps = [
        "//third_party:gflags",
        "//third_party:glog",
    ],
)

cc_test(
    name = "representation_test",
    size = "small",
    srcs = ["representation_test.cpp"],
    copts = COMPNAT_CPP_COPTS,
    linkopts = COMPNAT_CPP_LINKOPTS,
    deps = [
        ":primitives",
        ":representation",
        "//third_party:gtest",
    ],
)

cc_library(
    name = "simulation",
    srcs = ["simulation.cpp"],
    hdrs = ["simulation.hpp"],
    copts = COMPNAT_CPP_COPTS,
    deps = [
        ":generators",
        ":operators",
        ":representation",
        ":statistics",
        "//third_party:glog",
    ],
)

cc_test(
    name = "simulation_test",
    size = "small",
    srcs = ["simulation_test.cpp"],
    copts = COMPNAT_CPP_COPTS,
    data = ["//compnat/tp1/datasets"],
    linkopts = COMPNAT_CPP_LINKOPTS,
    deps = [
        ":parser",
        ":primitives",
        ":representation",
        ":simulation",
        "//third_party:gtest",
    ],
)

cc_library(
    name = "statistics",
    srcs = ["statistics.cpp"],
    hdrs = ["statistics.hpp"],
    copts = COMPNAT_CPP_COPTS,
    deps = [
        ":representation",
        ":utils",
        "//compnat/tp1/results",
        "//third_party:glog",
    ],
)

cc_test(
    name = "statistics_test",
    size = "small",
    srcs = ["statistics_test.cpp"],
    copts = COMPNAT_CPP_COPTS,
    data = ["//compnat/tp1/datasets"],
    linkopts = COMPNAT_CPP_LINKOPTS,
    deps = [
        ":generators",
        ":parser",
        ":primitives",
        ":representation",
        ":statistics",
        "//third_party:gmock",
        "//third_party:gtest",
    ],
)

cc_library(
    name = "utils",
    hdrs = ["utils.hpp"],
    copts = COMPNAT_CPP_COPTS,
)

cc_test(
    name = "utils_test",
    size = "small",
    srcs = ["utils_test.cpp"],
    copts = COMPNAT_CPP_COPTS,
    linkopts = COMPNAT_CPP_LINKOPTS,
    deps = [
        ":utils",
        "//third_party:gtest",
    ],
)
