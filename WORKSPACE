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

workspace(name = "com_github_renatoutsch_compnat")

# TODO(renatoutsch): use a tag or commit once there's a release
http_archive(
    name = "com_github_renatoutsch_rules_flatbuffers",
    #sha256 = "",  # TODO(renatoutsch): add once there's a release
    strip_prefix = "rules_flatbuffers-master",
    urls = ["https://github.com/RenatoUtsch/rules_flatbuffers/archive/master.zip"],
)

load(
    "@com_github_renatoutsch_rules_flatbuffers//flatbuffers:defs.bzl",
    "flatbuffers_repositories",
)

flatbuffers_repositories()

# TODO(renatoutsch): use a tag or commit once there's a release
http_archive(
    name = "com_github_renatoutsch_rules_system",
    #sha256 = "",  # TODO(renatoutsch): add once there's a release
    strip_prefix = "rules_system-master",
    urls = ["https://github.com/RenatoUtsch/rules_system/archive/master.zip"],
)

load(
    "@com_github_renatoutsch_rules_system//system:defs.bzl",
    "local_cc_library",
)

new_http_archive(
    name = "googletest",
    build_file = "third_party/googletest.BUILD",
    sha256 = "f3ed3b58511efd272eb074a3a6d6fb79d7c2e6a0e374323d1e6bcbcc1ef141bf",
    strip_prefix = "googletest-release-1.8.0",
    urls = ["https://github.com/google/googletest/archive/release-1.8.0.zip"],
)

http_archive(
    name = "bazel_rules",
    sha256 = "1809280f87836d88420f8b4f36ed65fe197a72ac6d3f0373d9aeb1a425afb4d2",
    strip_prefix = "bazel_rules-157d1fb34fbf5e292fef9501f67c303a5b1e6fe2",
    urls = ["https://github.com/antonovvk/bazel_rules/archive/157d1fb34fbf5e292fef9501f67c303a5b1e6fe2.zip"],
)

http_archive(
    name = "com_github_gflags_gflags",
    sha256 = "4e44b69e709c826734dbbbd5208f61888a2faf63f239d73d8ba0011b2dccc97a",
    strip_prefix = "gflags-2.2.1",
    urls = ["https://github.com/gflags/gflags/archive/v2.2.1.zip"],
)

new_http_archive(
    name = "com_github_google_glog",
    build_file = "third_party/glog.BUILD",
    sha256 = "267103f8a1e9578978aa1dc256001e6529ef593e5aea38193d31c2872ee025e8",
    strip_prefix = "glog-0.3.5",
    urls = ["https://github.com/google/glog/archive/v0.3.5.zip"],
)

bind(
    name = "glog",
    actual = "@com_github_google_glog//:glog",
)

bind(
    name = "gflags",
    actual = "@com_github_gflags_gflags//:gflags",
)
