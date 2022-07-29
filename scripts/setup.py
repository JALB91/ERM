import argparse, os, platform, subprocess
from enum import Enum

class TargetAPI(Enum):
    VULKAN  = "Vulkan"
    OPEN_GL = "OpenGL"
    DX12    = "DX12"

    def __str__(self):
        return str(self.value)

class Config(Enum):
    DEBUG   = "Debug"
    RELEASE = "Release"

    def __str__(self):
        return str(self.value)

class OS(Enum):
    WINDOWS = "Windows"
    MAC     = "MacOS"

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
        "--config",
        default=Config.DEBUG,
        dest="config",
        type=Config,
        choices=list(Config),
        help=f"Set build configuration ({Config.DEBUG} Default)")

    parser.add_argument(
        "-o", "--open",
        default=False,
        action="store_true",
        dest="open",
        help="Open the project in the IDE after generation")

    parser.add_argument(
        "-r",
        default=False,
        action="store_true",
        dest="release",
        help="Generate with release configuration")

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
        "--verbose",
        default=False,
        action="store_true",
        dest="verbose",
        help="Print CMake variables")

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

    target_config = Config.RELEASE if args.release else args.config
    
    root_path = os.path.dirname(os.path.abspath(__file__))
    root_path = root_path[:root_path.rfind(os.path.sep)]
    
    builds_path = os.path.join(root_path, "builds")

    target_build_path = os.path.join(builds_path, f"ERM_{generator_name}_{args.target_api}")

    try:
        os.makedirs(os.path.join(target_build_path, target_config))
    except:
        pass

    cmake_args = [
        "ccmake" if args.interactive else "cmake", 
        f"-DPRINT_VARIABLES={('ON' if args.verbose else 'OFF')}",
        f"-DCMAKE_BUILD_TYPE={target_config}",
        f"-DTARGET_API={args.target_api}",
        f"-DERM_RAY_TRACING_ENABLED={('ON' if args.rtx else 'OFF')}",
        "-G", generator
    ]

    if (architecture):
        cmake_args.append("-A", architecture)

    if (target_config == Config.RELEASE):
        cmake_args.append("-DNDEBUG=ON")
    
    cmake_args.append("../../")
    
    subprocess.run(cmake_args, cwd=target_build_path)

    if args.compile:
        subprocess.run(["cmake", "--build", "."], cwd=target_build_path)

    if args.open:
        subprocess.run(["cmake", "--open", "."], cwd=target_build_path)

if __name__ == "__main__":
    main()