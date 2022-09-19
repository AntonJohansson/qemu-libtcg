#!/usr/bin/env python3

import argparse
import json
import os
import shlex
import sys
import subprocess

def run_command(argv):
    proc = subprocess.Popen(argv)
    if proc.wait() != 0:
        print("Command exited with %d:\n%s\n" % (proc.returncode," ".join(argv)))

def find_compile_commands(target_name, clang_path, input_path):
    if os.path.exists("compile_commands.json"):
        with open("compile_commands.json", "r") as compile_commands_file:
            compile_commands = json.load(compile_commands_file)
            for compile_command in compile_commands:
                path = compile_command["file"]
                #if os.path.basename(path) != os.path.basename(input_path):
                if path != input_path:
                    continue


                os.chdir(compile_command["directory"])
                command = compile_command["command"]
                if not target_name in command:
                    continue

                argv = shlex.split(command)
                argv[0] = clang_path
                return argv

    # If we coulnd't find "compile_commands" just default
    # to the following.
    assert(False)
    return [clang_path, input_path]

def generate_llvm_ir(target_name, clang_path, input_path, output_path):
    argv = find_compile_commands(target_name, clang_path, input_path)

    for i, arg in reversed(list(enumerate(argv))):
        if arg in {'-MQ', '-o', '-MF'}:
            del argv[i:i+2]

    argv += ["-Wno-unknown-warning-option",  "-emit-llvm", "-disable-O0-optnone -O0", "-o", output_path]

    run_command(argv)

def link(llvm_link_path, ll_paths, output_path):
    argv = [llvm_link_path] + ll_paths + ['-o', output_path]
    run_command(argv)

def temp_file(output_folder, file):
    _, containing_folder = os.path.split(os.path.abspath(os.path.dirname(file)))
    dir = os.path.join(output_folder, containing_folder)
    os.makedirs(dir, exist_ok=True)
    filename, _ = os.path.splitext(os.path.basename(file))
    new_file = os.path.join(dir, filename) + ".bc"
    return new_file

def main():
    parser = argparse.ArgumentParser(description='Produce the LLVM IR for a given source file.')
    parser.add_argument('target_name', metavar='TARGET_NAME',           help='Name of target')
    parser.add_argument('target',      metavar='TARGET',                help='Target')
    parser.add_argument("clang",       metavar="CLANG_PATH",            help="Path to clang.")
    parser.add_argument("llvm_link",   metavar="LLVM_LINK_PATH",        help="Path to llvm-link")
    parser.add_argument("opt",         metavar="OPT_PATH",              help="Path to opt")
    parser.add_argument('output',      metavar='OUTPUT_PATH',           help='Path to output file')
    parser.add_argument('input_paths', metavar='INPUT_PATH', nargs='+', help='Source file input path')
    args = parser.parse_args()

    output_folder = os.path.join("llvm-helpers", args.target_name)
    os.makedirs(output_folder, exist_ok=True)

    # Generate LLVM IR
    ll_paths = []
    for input_path in args.input_paths:
        if "accel/" in input_path or \
           "libtcg/" in input_path or \
           "plugins/" in input_path or \
           "semihosting/" in input_path or \
           input_path.endswith(".inc") or \
           input_path.endswith(".h") or \
           input_path.endswith(".pyinc") or \
           input_path.endswith(".py") or \
           "gdb" in input_path:
            continue
        print(input_path)
        output_path = temp_file(output_folder, input_path)
        ll_paths.append(output_path)
        generate_llvm_ir(args.target, args.clang, input_path, output_path)

    link(args.llvm_link, ll_paths, args.output)

if __name__ == "__main__":
    sys.exit(main())
