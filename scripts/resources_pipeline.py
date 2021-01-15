import sys, argparse, shutil, os

def clean_shaders(root, shaders):
    for shader in shaders:
        if shader.find("_tmp"):
            os.remove(os.path.join(root, shader))
        elif shader.endswith(".cmp"):
            source_path = os.path.join(root, shader[:-4])
            shader_path = os.path.join(root, shader)
            source_time = os.path.getmtime(source_path)
            shader_time = os.path.getmtime(shader_path)
            if not os.path.exists(source_path) or source_time > shader_time:
                os.remove(shader_path)

def clean_resources(res_folder):
    shaders_path = os.path.join(res_folder, "shaders")
    for root, subdir, files in os.walk(shaders_path):
        clean_shaders(root, files)

def copy_resources(res_src, res_dest):
    shutil.copytree(res_src, res_dest, dirs_exist_ok=True)

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

    clean_resources(args.res_dest)
    copy_resources(args.res_src, args.res_dest)

    if (args.shaders_compiler):
        compile_shaders(args.shaders_compiler, args.res_dest)