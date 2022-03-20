from os import environ
import os
from os import path
import subprocess


def file_to_header(source_filepath: str, header_filepath: str, variable_name: str):
    with open(source_filepath, "rb") as binary_file:
        with open(header_filepath, "w") as header_file:
            header_file.write("\nconst unsigned char " + variable_name + "[] = {\n")
            line_width = 0
            for collection in binary_file:
                for byte in collection:
                    header_file.write(str(byte) + ",")
                    line_width += 1
                    if line_width >= 30:
                        header_file.write("\n")
                        line_width = 0
            header_file.write("};\n")


def compile_shaders() -> None:
    spirv_compiler_path: str = path.abspath(path.join(environ["VULKAN_SDK"], "Bin", "glslangValidator.exe"))
    source_directory: str = path.abspath(path.join(path.dirname(__file__), "src"))
    binary_directory: str = path.abspath(path.join(path.dirname(__file__), "bin"))
    main_header_filepath: str = path.join(binary_directory)
    temp_spirv_directory = path.join(binary_directory, "SPIRV")
    include_spv_directory_suffix = ["include", "SPIRV"]
    include_glsl_directory_suffix = ["include", "GLSL"]
    include_spv_directory = path.join(binary_directory, *include_spv_directory_suffix)
    include_glsl_directory = path.join(binary_directory, *include_glsl_directory_suffix)

    os.makedirs(binary_directory, exist_ok=True)

    generated_spv_headers_paths: list[str] = []
    generated_glsl_headers_paths: list[str] = []
    for root, subFolders, files in os.walk(source_directory):
        os.makedirs(temp_spirv_directory, exist_ok=True)
        os.makedirs(include_spv_directory, exist_ok=True)
        os.makedirs(include_glsl_directory, exist_ok=True)
        for filename in files:
            shader_filepath: str = path.join(root, filename)
            out_spv_filepath: str = path.join(temp_spirv_directory, filename + ".spv")
            process = subprocess.Popen([spirv_compiler_path, "-V", shader_filepath, "-o", out_spv_filepath, "-g", ],
                                       stdout=subprocess.PIPE,
                                       stderr=subprocess.PIPE)
            process.wait()
            if process.returncode != 0:
                output, error = process.communicate()
                raise RuntimeError(f"Failed to compile shader: {filename}.\n"
                                   f"Compiler exited with following message:\n"
                                   f"STDERR:\n{error.decode()}"
                                   f"STDOUT:\n{output.decode()}")

            gen_spv_header_filename = filename + ".spv.h"
            gen_spv_header_filepath: str = path.join(include_spv_directory, gen_spv_header_filename)
            gen_glsl_header_filename = filename + ".glsl.h"
            gen_glsl_header_filepath: str = path.join(include_glsl_directory, gen_glsl_header_filename)
            file_to_header(out_spv_filepath, gen_spv_header_filepath, filename.replace(".", "_"))
            file_to_header(shader_filepath, gen_glsl_header_filepath, filename.replace(".", "_"))
            generated_spv_headers_paths.append(gen_spv_header_filename)
            generated_glsl_headers_paths.append(gen_glsl_header_filename)

    with open(path.join(main_header_filepath, "SPIRVShaders.h"), "w") as main_header_file:
        for header_filename in generated_spv_headers_paths:
            main_header_file.write(f"#include \"{path.join(*include_spv_directory_suffix, header_filename)}\"\n")
    with open(path.join(main_header_filepath, "GLSLShaders.h"), "w") as main_header_file:
        for header_filename in generated_glsl_headers_paths:
            main_header_file.write(f"#include \"{path.join(*include_glsl_directory_suffix, header_filename)}\"\n")


if __name__ == "__main__":
    try:
        compile_shaders()
    except Exception as error:
        print("Failed to compile shaders, an error occurred: ", error)
        exit(-1);
