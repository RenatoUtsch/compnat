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
    name = "tp2",
    srcs = ["tp2.cpp"],
    copts = COMPNAT_CPP_COPTS,
    linkopts = COMPNAT_CPP_LINKOPTS,
    deps = [
        ":representation",
        "//third_party:gflags",
        "//third_party:glog",
    ],
)

cc_library(
    name = "aco",
    srcs = ["aco.cpp"],
    hdrs = ["aco.hpp"],
    copts = COMPNAT_CPP_COPTS,
    linkopts = COMPNAT_CPP_LINKOPTS,
    deps = [
        ":representation",
        "//third_party:glog",
    ],
)

cc_library(
    name = "representation",
    srcs = ["representation.cpp"],
    hdrs = ["representation.hpp"],
    copts = COMPNAT_CPP_COPTS,
    linkopts = COMPNAT_CPP_LINKOPTS,
    deps = [
        "//third_party:glm",
        "//third_party:glog",
    ],
)
