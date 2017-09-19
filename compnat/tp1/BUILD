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

load("//compnat:defs.bzl", "COMPNAT_CPP_COPTS")

package(default_visibility = ["//visibility:private"])

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
    deps = [
        ":representation",
        "//third_party:gtest",
    ],
)
