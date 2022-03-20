#  Copyright (c) 2022.
#  License: CC0 1.0 Universal
#  Permissions:
#  - Commercial use
#  - Modification
#  - Distribution
#  - Private use
#
#  Limitations:
#  - Liability
#  - Trademark use
#  - Patent use
#  - Warranty
#
#  Author: Danil Andreev | danssg08@gmail.com | https://github.com/DanilAndreev
import os
import subprocess
from zipfile import ZipFile
import shutil
from os import path


def assemble_release_dir(release_binary_dir: str, cmake_build_dir: str, qt_binary_dir: str):
    executable_name: str = "image-morphing-tool.exe"

    if path.exists(release_binary_dir):
        shutil.rmtree(release_binary_dir)
    os.makedirs(release_binary_dir)

    executable_path = path.join(cmake_build_dir, executable_name)
    shutil.copy(executable_path, release_binary_dir)

    process = subprocess.Popen([path.join(qt_binary_dir, "windeployqt.exe"),
                                "--release",
                                "--dir", release_binary_dir,
                                path.join(release_binary_dir, executable_name)],
                               stdout=subprocess.PIPE,
                               stderr=subprocess.PIPE,
                               env=dict(os.environ, PATH=qt_binary_dir))
    process.wait()
    if process.returncode != 0:
        output, error = process.communicate()
        raise RuntimeError(f"STDERR:\n{error.decode()}"
                           f"STDOUT:\n{output.decode()}")

    shutil.copy(path.join(qt_binary_dir, "libgcc_s_seh-1.dll"), release_binary_dir)
    shutil.copy(path.join(qt_binary_dir, "libstdc++-6.dll"), release_binary_dir)
    shutil.copy(path.join(qt_binary_dir, "libwinpthread-1.dll"), release_binary_dir)



def post_build():
    release_binary_dir: str = path.abspath(path.join(path.dirname(__file__), "..", "release"))
    cmake_build_dir = path.join("C:\\", "Projects", "image-morphing-tool", "cmake-build-release-mingw-qt6")
    qbd = path.join("C:\\", "Qt", "6.2.3", "mingw_64", "bin")
    assemble_release_dir(path.join(release_binary_dir, "bin"), cmake_build_dir, qbd)

    print("Assembling ZIP")
    with ZipFile(path.join(release_binary_dir, "release.zip"), "w") as zip_obj:
        for folder_name, subfolders, filenames in os.walk(path.join(release_binary_dir, "bin")):
            for filename in filenames:
                source_filepath = os.path.join(folder_name, filename)
                dest_filepath = os.path.join(folder_name.replace(path.join(release_binary_dir, "bin"), "")[1:], filename)
                zip_obj.write(source_filepath, dest_filepath)
                print(f"Processed: '{source_filepath}' -> '{dest_filepath}'")


if __name__ == "__main__":
    post_build()
