#pragma once

namespace erm {
struct CMakeBuildArgs;
struct CMakeGenerateArgs;
struct CMakeInstallArgs;
struct CMakeOpenArgs;
}

namespace erm::cmake {

int generate(const CMakeGenerateArgs& args);
int install(const CMakeInstallArgs& args);
int build(const CMakeBuildArgs& args);
int open(const CMakeOpenArgs& args);

}
