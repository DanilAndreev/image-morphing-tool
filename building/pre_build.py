from subprocess import Popen
from os import path
import platform


def run_system_command(*args: str) -> None:
    process = Popen(args, shell=True)
    process.wait()
    if process.returncode != 0:
        stdout, stderr = process.communicate()
        error_message: str = f"An error occurred while evaluating the command:\n{' '.join(args)}" \
                             f"\nProcess exited with return code: {process.returncode}"
        if stdout:
            error_message += f"stdout:\n{stdout}\n"
        if stderr:
            error_message += f"stdout:\n{stderr}\n"
        exit(error_message)


def main():
    shaders_compile_script_path: str = path.abspath(path.join(
        path.dirname(__file__), "..", "src", "tools", "shaders", "compile.py"
    ))
    if platform.system() == "Darwin":
        run_system_command("python3", shaders_compile_script_path, "-os", "Win")
    elif platform.system() == "Windows":
        run_system_command("python", shaders_compile_script_path, "-os", "OSX")
    else:
        raise RuntimeError("Unsupported platform")

    print("Shaders have been successfully built.")


if __name__ == '__main__':
    main()
