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
import sys
from zipfile import ZipFile
import shutil
from os import path
import argparse
from datetime import datetime


def assemble_release_dir(release_binary_dir: str, cmake_build_dir: str, qt_binary_dir: str):
    executable_name: str = "image-morphing-tool.exe"

    if path.exists(release_binary_dir):
        shutil.rmtree(release_binary_dir)
    os.makedirs(release_binary_dir)

    executable_path = path.join(cmake_build_dir, executable_name)
    if path.abspath(executable_path) != path.abspath(release_binary_dir):
        shutil.copy(executable_path, release_binary_dir)

    process = subprocess.Popen([path.join(qt_binary_dir, "windeployqt.exe"),
                                "--no-system-d3d-compiler",
                                "--no-opengl-sw",
                                "--force",
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

    if not path.exists(path.join(release_binary_dir, "libgcc_s_seh-1.dll")):
        shutil.copy(path.join(qt_binary_dir, "libgcc_s_seh-1.dll"), release_binary_dir)
    if not path.exists(path.join(release_binary_dir, "libstdc++-6.dll")):
        shutil.copy(path.join(qt_binary_dir, "libstdc++-6.dll"), release_binary_dir)
    if not path.exists(path.join(release_binary_dir, "libwinpthread-1.dll")):
        shutil.copy(path.join(qt_binary_dir, "libwinpthread-1.dll"), release_binary_dir)


def assemble_package(release_binary_dir: str,
                     packed_out_dir: str,
                     package_name: str = "release",
                     no_timestamp: bool = False):
    print("Assembling ZIP")
    os.makedirs(packed_out_dir, exist_ok=True)
    timestamp = datetime.today().strftime(".%Y.%m.%d.%H%M%S")
    archive_filename: str = package_name + (timestamp if no_timestamp is False else "") + ".zip"
    with ZipFile(path.join(packed_out_dir, archive_filename), "w") as zip_obj:
        for folder_name, subfolders, filenames in os.walk(path.join(release_binary_dir)):
            for filename in filenames:
                if filename == archive_filename:
                    continue
                source_filepath = os.path.join(folder_name, filename)
                dest_filepath = os.path.join(folder_name.replace(packed_out_dir, "")[1:], filename)
                zip_obj.write(source_filepath, dest_filepath)
                print(f"Processed: '{source_filepath}' -> '{dest_filepath}'")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-qbd", "--qt6-binary-dir", type=str, required=True,
                        help="Path to QT6 binary directory. Example: .../Qt/6.2.3/mingw_64/bin")
    parser.add_argument("-cbd", "--cmake-build-dir", type=str, required=True,
                        help="Path to CMake project build directory.")
    parser.add_argument("-od", "--out-dir", type=str,
                        default=path.abspath(path.join(path.dirname(__file__), "..", "release")),
                        help="Path to output directory.")
    parser.add_argument("--no-pack", type=bool, default=False,
                        help="Path to output directory.")
    parser.add_argument("-o", "--out-packed-dir", type=str,
                        default=None,
                        help="Path to output directory for packed archive.")
    parser.add_argument("--package-name", type=str, default="CurveMorphingTool",
                        help="Packed archive name.")
    parser.add_argument("--no-timestamp", type=bool, default=False,
                        help="Disable timestamp in name of packed archive.")
    try:
        args = parser.parse_args()
        assemble_release_dir(release_binary_dir=args.out_dir,
                             cmake_build_dir=args.cmake_build_dir,
                             qt_binary_dir=args.qt6_binary_dir)
        if not args.no_pack:
            args.out_packed_dir = args.out_packed_dir if args.out_packed_dir is not None else args.out_dir
            assemble_package(release_binary_dir=args.out_dir,
                             packed_out_dir=args.out_packed_dir,
                             package_name=args.package_name,
                             no_timestamp=args.no_timestamp)
    except Exception as e:
        print("Fatal error: " + str(e), file=sys.stderr)
        exit(-1)
