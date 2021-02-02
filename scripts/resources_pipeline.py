import sys, argparse, shutil, os

def update_asset(src_folder, dest_folder, subdir, asset):
    dest_path = os.path.join(dest_folder, subdir, asset)
    if "_tmp" in asset:
        os.remove(dest_path)
        return
    src_path = os.path.join(src_folder, subdir, asset)
    if src_path.endswith(".cmp"):
        src_path = src_path[:-4]
    if not os.path.exists(src_path):
        os.remove(dest_path)
        return
    src_time = os.path.getmtime(src_path)
    dest_time = os.path.getmtime(dest_path)
    if not os.path.exists(src_path) or src_time > dest_time:
        os.remove(dest_path)

def update_resources(src_folder, dest_folder):
    for root, subdir, files in os.walk(dest_folder):
        for asset in files:
            subdir = ""
            if len(root) > len(dest_folder):
                subdir = root[len(dest_folder) + 1:]
            update_asset(src_folder, dest_folder, subdir, asset)
    shutil.copytree(src_folder, dest_folder)

def compile_shaders(shaders_compiler, res_folder):
    for root, subdir, files in os.walk(os.path.join(res_folder, "shaders")):
        for shader in files:
            if shader.startswith("vk_") and not shader.endswith(".cmp"):
                shader_path = os.path.join(root, shader)
                if not os.path.exists(shader_path + ".cmp"):
                    os.system(shaders_compiler + " -c " + shader_path + " -o " + shader_path + ".cmp")

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--shaders-compiler", dest="shaders_compiler", type=str, help="Path to the shaders compilation command for Vulkan")
    parser.add_argument("--res-dest", dest="res_dest", type=str, help="Resources destination path")
    parser.add_argument("--res-src", dest="res_src", type=str, help="Resources source path")
    args = parser.parse_args()

    if not os.path.exists(args.res_src):
        parser.print_help()
        exit(1)

    shutil.rmtree(args.res_dest)
    update_resources(args.res_src, args.res_dest)

    if (args.shaders_compiler):
        compile_shaders(args.shaders_compiler, args.res_dest)