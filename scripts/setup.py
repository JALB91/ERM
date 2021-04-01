import sys, argparse, shutil, os, platform, subprocess

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-t", "--target-api",   default="Vulkan",                           dest="api",         type=str,   choices=["OpenGl", "DX12", "Vulkan"],   help="Set target rendering API [OpenGl/DX12/Vulkan (Default)]")
    parser.add_argument("-o", "--open",         default=False,      action="store_true",    dest="open",                                                            help="Open the project in the IDE after generation")
    parser.add_argument("-r",                   default=False,      action="store_true",    dest="release",                                                         help="Generate with release configuration")
    parser.add_argument("--rtx",                default=False,      action="store_true",    dest="rtx",                                                             help="Enable ray tracing")
    parser.add_argument("--config",             default="Debug",                            dest="config",      type=str,   choices=["Debug", "Release"],           help="Set build configuration [Release/Debug (Default)]")
    parser.add_argument("--interactive",        default=False,      action="store_true",    dest="interactive",                                                     help="Run CMake in interactive mode")
    parser.add_argument("--compile",            default=False,      action="store_true",    dest="compile",                                                         help="Compile the project after generation")
    parser.add_argument("--verbose",            default=False,      action="store_true",    dest="verbose",                                                         help="Print CMake variables")
    args = parser.parse_args()

    currSys = platform.system()
    if currSys == "Darwin":
        generatorName = "Xcode"
        generator = "Xcode"
        osName = "OSX"
        architecture = ""
    elif currSys == "Windows":
        generatorName = "MSVC"
        generator = "Visual Studio 16 2019"
        architecture = "x64"
        osName = "WINDOWS"
    else:
        print("OS not supported")
        exit(1)

    if args.release:
        args.config = "Release"

    release = args.config == "Release"
    
    if args.interactive:
        cmakeCmd = "ccmake"
    else:
        cmakeCmd = "cmake"
    
    rootPath = os.path.dirname(os.path.abspath(__file__))
    rootPath = rootPath[:rootPath.rfind(os.path.sep)]
    
    buildsPath = os.path.join(rootPath, "builds")

    targetBuildPath = os.path.join(buildsPath, generatorName + "_" + args.api + "_" + args.config)

    try:
        os.makedirs(os.path.join(targetBuildPath, args.config))
    except:
        pass
    
    subprocess.run([
        cmakeCmd, 
        '-DPRINT_VARIABLES=' + ('ON' if args.verbose else 'OFF'),
        '-DCMAKE_BUILD_TYPE=' + args.config,
        '-DTARGET_API=' + args.api,
        '-DERM_RAY_TRACING_ENABLED=' + ('ON' if args.rtx else 'OFF'),
        '-DNDEBUG=' + ('ON' if release else 'OFF'),
        '-G', generator,
        '-A', architecture,
        '../..'], cwd=targetBuildPath)

    if args.compile:
        subprocess.run(['cmake', '--build', '.'], cwd=targetBuildPath)

    if args.open:
        subprocess.run(['cmake', '--open', '.'], cwd=targetBuildPath)