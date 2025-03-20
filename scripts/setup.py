import argparse, os, platform, subprocess
from enum import Enum

class TargetAPI(Enum):
    VULKAN  = "Vulkan"
    OPEN_GL = "OpenGL"
    DX12    = "DX12"

    def __str__(self):
        return str(self.value)

class OS(Enum):
    WINDOWS = "Windows"
    MAC     = "MacOS"

    def __str__(self):
        return str(self.value)

    @staticmethod
    def current():
        currSys = platform.system()
        if currSys == "Darwin":
            return OS.MAC
        elif currSys == "Windows":
            return OS.WINDOWS

CURRENT_SYSTEM = OS.current()

def main():
    parser = argparse.ArgumentParser()

    parser.add_argument(
        "-t", "--target-api",
        default=TargetAPI.VULKAN,
        dest="target_api",
        type=TargetAPI,
        choices=list(TargetAPI),
        help=f"Set target rendering API ({TargetAPI.VULKAN} Default)")

    parser.add_argument(
        "-o", "--open",
        default=False,
        action="store_true",
        dest="open",
        help="Open the project in the IDE after generation")

    parser.add_argument(
        "--rtx",
        default=False,
        action="store_true",
        dest="rtx",
        help="Enable ray tracing")

    parser.add_argument(
        "--interactive",
        default=False,
        action="store_true",
        dest="interactive",
        help="Run CMake in interactive mode")

    parser.add_argument(
        "--compile",
        default=False,
        action="store_true",
        dest="compile",
        help="Compile the project after generation")

    parser.add_argument(
        "--tracy",
        default=False,
        action="store_true",
        dest="tracy_enabled",
        help="Enable tracy")
    
    parser.add_argument(
        "--verbose",
        default=False,
        action="store_true",
        dest="verbose",
        help="Debug cmake output")
    
    parser.add_argument(
        "--trace",
        default=False,
        action="store_true",
        dest="trace",
        help="Trace cmake output")

    args = parser.parse_args()

    if CURRENT_SYSTEM == OS.MAC:
        generator_name = "Xcode"
        generator = "Xcode"
        architecture = ""
    elif CURRENT_SYSTEM == OS.WINDOWS:
        generator_name = "MSVC"
        generator = "Visual Studio 17 2022"
        architecture = "x64"
    else:
        print(f"OS not supported: {platform.system()}")
        exit(1)

    if args.rtx and args.target_api != TargetAPI.VULKAN:
        print(f"RayTracing not available when using {args.target_api} APIs")
        exit(1)
    
    erm_root_path = os.path.dirname(os.path.abspath(__file__))
    erm_root_path = erm_root_path[:erm_root_path.rfind(os.path.sep)]
    
    builds_root_path = os.path.join(erm_root_path, "builds")

    target_build_path = os.path.join(builds_root_path, f"ERM_{generator_name}_{args.target_api}")

    if (not os.path.exists(target_build_path)):
        os.mkdir(target_build_path)

    cmake_args = [
        "ccmake" if args.interactive else "cmake", 
        f"-DERM_RENDERING_API={args.target_api}",
        f"-DERM_HOST_PLATFORM={CURRENT_SYSTEM}",
        f"-DERM_RAY_TRACING_ENABLED={('ON' if args.rtx else 'OFF')}",
        f"-DERM_TRACY_ENABLED={('ON' if args.tracy_enabled else 'OFF')}"
    ]

    if args.verbose:
        cmake_args.append("--debug-output")
    
    if args.trace:
        cmake_args.append("--trace")
    

    if (generator):
        cmake_args.append("-G")
        cmake_args.append(generator)

    if (architecture):
        cmake_args.append("-A")
        cmake_args.append(architecture)
    
    cmake_args.append("../../")
    
    result = subprocess.run(cmake_args, cwd=target_build_path)
    result.check_returncode()

    if args.compile:
        cmake_args = ["cmake", "--build", "."]
        if args.verbose:
            cmake_args.append("--debug-output")
        
        if args.trace:
            cmake_args.append("--trace")
        
        result = subprocess.run(cmake_args, cwd=target_build_path)
        result.check_returncode()

    if args.open:
        cmake_args = ["cmake", "--open", "."]
        if args.verbose:
            cmake_args.append("--debug-output")
        
        if args.trace:
            cmake_args.append("--trace")
        
        result = subprocess.run(cmake_args, cwd=target_build_path)
        result.check_returncode()

if __name__ == "__main__":
    main()