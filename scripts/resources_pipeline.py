import argparse, shutil, os

def error(descr: str):
    print(f"=== ERROR === Resources pipeline failed: {descr}")
    exit(1)

def compile_shaders(shaders_compiler, res_folder, rtx_enabled):
    for root, subdir, files in os.walk(os.path.join(res_folder, "shaders")):
        for shader in files:
            if (root.endswith("rasterization") or (rtx_enabled and root.endswith("ray_tracing"))) and not shader.endswith(".cmp"):
                shader_path = os.path.join(root, shader)
                if not os.path.exists(shader_path + ".cmp"):
                    os.system(shaders_compiler + " -c " + shader_path + " -o " + shader_path + ".cmp")

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--shaders-compiler", dest="shaders_compiler", type=str, help="Path to the shaders compilation command for Vulkan")
    parser.add_argument("--res-dest", dest="res_dest", type=str, help="Resources destination path")
    parser.add_argument("--res-src", dest="res_src", type=str, help="Resources source path")
    parser.add_argument("--rtx-enabled", dest="rtx_enabled", type=bool, help="Is ray tracing enabled")
    args = parser.parse_args()

    if not os.path.exists(args.res_src):
        error(f'Could not find res folder in "{args.res_src}"')

    try:
        shutil.rmtree(args.res_dest)
    except:
        pass

    try:
        shutil.copytree(args.res_src, args.res_dest)
    except Exception as e:
        error(f'Failed to copy resources.\n{e}')

    if (args.shaders_compiler):
        try:
            compile_shaders(args.shaders_compiler, args.res_dest, args.rtx_enabled)
        except Exception as e:
            error(f'Shaders compilation failed.\n{e}')
