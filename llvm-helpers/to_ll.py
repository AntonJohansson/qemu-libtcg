#!/usr/bin/env python3

import argparse
import json
import os
import shlex
import sys
import subprocess

helpers = {
    "arm" : [
        "target/arm/helper.c",
        "target/arm/tcg/crypto_helper.c",
        "target/arm/tcg/helper-a64.c",
        "target/arm/tcg/hflags.c",
        "target/arm/tcg/iwmmxt_helper.c",
        "target/arm/tcg/m_helper.c",
        "target/arm/tcg/mte_helper.c",
        "target/arm/tcg/mve_helper.c",
        "target/arm/tcg/neon_helper.c",
        "target/arm/tcg/op_helper.c",
        "target/arm/tcg/pauth_helper.c",
        "target/arm/tcg/psci.c",
        "target/arm/tcg/sme_helper.c",
        "target/arm/tcg/sve_helper.c",
        "target/arm/tcg/tlb_helper.c",
        "target/arm/tcg/translate-a64.c",
        "target/arm/tcg/translate-m-nocp.c",
        "target/arm/tcg/translate-mve.c",
        "target/arm/tcg/translate-neon.c",
        "target/arm/tcg/translate-sme.c",
        "target/arm/tcg/translate-sve.c",
        "target/arm/tcg/translate-vfp.c",
        "target/arm/tcg/translate.c",
        "target/arm/tcg/vec_helper.c",
        "target/arm/vfp_helper.c",
        "linux-user/arm/nwfpe/fpa11.c",
        "linux-user/arm/nwfpe/fpa11_cpdo.c",
        "linux-user/arm/nwfpe/fpa11_cpdt.c",
        "linux-user/arm/nwfpe/fpa11_cprt.c",
        "linux-user/arm/nwfpe/fpopcode.c",
        "linux-user/arm/nwfpe/single_cpdo.c",
        "linux-user/arm/nwfpe/double_cpdo.c",
        "linux-user/arm/nwfpe/extended_cpdo.c",
        "linux-user/arm/cpu_loop.c",
        "linux-user/arm/signal.c",
        "linux-user/main.c",
        "linux-user/syscall.c",
        "linux-user/mmap.c",
        "linux-user/signal.c",
        "linux-user/uaccess.c",
        "linux-user/uname.c",
    ],
    "aarch64" : [
        "target/arm/helper.c",
        "target/arm/tcg/crypto_helper.c",
        "target/arm/tcg/helper-a64.c",
        "target/arm/tcg/hflags.c",
        "target/arm/tcg/iwmmxt_helper.c",
        "target/arm/tcg/m_helper.c",
        "target/arm/tcg/mte_helper.c",
        "target/arm/tcg/mve_helper.c",
        "target/arm/tcg/neon_helper.c",
        "target/arm/tcg/op_helper.c",
        "target/arm/tcg/pauth_helper.c",
        "target/arm/tcg/psci.c",
        "target/arm/tcg/sme_helper.c",
        "target/arm/tcg/sve_helper.c",
        "target/arm/tcg/tlb_helper.c",
        "target/arm/tcg/translate-a64.c",
        "target/arm/tcg/translate-m-nocp.c",
        "target/arm/tcg/translate-mve.c",
        "target/arm/tcg/translate-neon.c",
        "target/arm/tcg/translate-sme.c",
        "target/arm/tcg/translate-sve.c",
        "target/arm/tcg/translate-vfp.c",
        "target/arm/tcg/translate.c",
        "target/arm/tcg/vec_helper.c",
        "target/arm/vfp_helper.c",
        "linux-user/arm/nwfpe/fpa11.c",
        "linux-user/arm/nwfpe/fpa11_cpdo.c",
        "linux-user/arm/nwfpe/fpa11_cpdt.c",
        "linux-user/arm/nwfpe/fpa11_cprt.c",
        "linux-user/arm/nwfpe/fpopcode.c",
        "linux-user/arm/nwfpe/single_cpdo.c",
        "linux-user/arm/nwfpe/double_cpdo.c",
        "linux-user/arm/nwfpe/extended_cpdo.c",
        "linux-user/arm/cpu_loop.c",
        "linux-user/arm/signal.c",
        "linux-user/main.c",
        "linux-user/syscall.c",
        "linux-user/mmap.c",
        "linux-user/signal.c",
        "linux-user/uaccess.c",
        "linux-user/uname.c",
    ],
    "s390x" : [
        "target/s390x/tcg/cc_helper.c",
        "target/s390x/tcg/crypto_helper.c",
        "target/s390x/tcg/excp_helper.c",
        "target/s390x/tcg/fpu_helper.c",
        "target/s390x/tcg/int_helper.c",
        "target/s390x/tcg/mem_helper.c",
        "target/s390x/tcg/misc_helper.c",
        "target/s390x/tcg/translate.c",
        "target/s390x/tcg/vec_fpu_helper.c",
        "target/s390x/tcg/vec_helper.c",
        "target/s390x/tcg/vec_int_helper.c",
        "target/s390x/tcg/vec_string_helper.c",
    ],
    "i386" : [
        "target/i386/tcg/misc_helper.c",
        "target/i386/tcg/translate.c",
        "target/i386/tcg/bpt_helper.c",
        "target/i386/tcg/cc_helper.c",
        "target/i386/tcg/excp_helper.c",
        "target/i386/tcg/fpu_helper.c",
        "target/i386/tcg/int_helper.c",
        "target/i386/tcg/mem_helper.c",
        "target/i386/tcg/mpx_helper.c",
        "target/i386/tcg/seg_helper.c",
    ],
    "x86_64" : [
        "target/i386/tcg/misc_helper.c",
        "target/i386/tcg/translate.c",
        "target/i386/tcg/bpt_helper.c",
        "target/i386/tcg/cc_helper.c",
        "target/i386/tcg/excp_helper.c",
        "target/i386/tcg/fpu_helper.c",
        "target/i386/tcg/int_helper.c",
        "target/i386/tcg/mem_helper.c",
        "target/i386/tcg/mpx_helper.c",
        "target/i386/tcg/seg_helper.c",
    ],
    "mips" : [
        "target/mips/tcg/dsp_helper.c",
        "target/mips/tcg/exception.c",
        "target/mips/tcg/fpu_helper.c",
        "target/mips/tcg/lcsr_translate.c",
        "target/mips/tcg/ldst_helper.c",
        "target/mips/tcg/lmmi_helper.c",
        "target/mips/tcg/msa_helper.c",
        "target/mips/tcg/msa_translate.c",
        "target/mips/tcg/op_helper.c",
        "target/mips/tcg/sysemu/cp0_helper.c",
        "target/mips/tcg/sysemu/lcsr_helper.c",
        "target/mips/tcg/sysemu/special_helper.c",
        "target/mips/tcg/sysemu/tlb_helper.c",
        "target/mips/tcg/translate.c",
        "target/mips/tcg/vr54xx_helper.c",
        "target/mips/tcg/vr54xx_translate.c",
    ],
    "mipsel" : [
        "target/mips/tcg/dsp_helper.c",
        "target/mips/tcg/exception.c",
        "target/mips/tcg/fpu_helper.c",
        "target/mips/tcg/lcsr_translate.c",
        "target/mips/tcg/ldst_helper.c",
        "target/mips/tcg/lmmi_helper.c",
        "target/mips/tcg/msa_helper.c",
        "target/mips/tcg/msa_translate.c",
        "target/mips/tcg/op_helper.c",
        "target/mips/tcg/sysemu/cp0_helper.c",
        "target/mips/tcg/sysemu/lcsr_helper.c",
        "target/mips/tcg/sysemu/special_helper.c",
        "target/mips/tcg/sysemu/tlb_helper.c",
        "target/mips/tcg/translate.c",
        "target/mips/tcg/vr54xx_helper.c",
        "target/mips/tcg/vr54xx_translate.c",
    ],
    "hexagon" : [
        "target/hexagon/decode.c",
        "target/hexagon/genptr.c",
        "target/hexagon/op_helper.c",
        "target/hexagon/translate.c",
    ],
    "loongarch64" : [
        "target/loongarch/csr_helper.c",
        "target/loongarch/fpu_helper.c",
        "target/loongarch/iocsr_helper.c",
        "target/loongarch/op_helper.c",
        "target/loongarch/tlb_helper.c",
        "target/loongarch/translate.c",
        "target/loongarch/vec_helper.c",
    ],
    "aarch64_be" : [],
    "alpha" : [],
    "armeb" : [],
    "cris" : [],
    "hppa" : [],
    "m68k" : [],
    "microblazeel" : [],
    "microblaze" : [],
    "mips64el" : [],
    "mips64" : [],
    "mipsn32el" : [],
    "mipsn32" : [],
    "nios2" : [],
    "or1k" : [],
    "ppc64le" : [],
    "ppc64" : [],
    "ppc" : [],
    "riscv32" : [],
    "riscv64" : [],
    "s390x" : [],
    "sh4eb" : [],
    "sh4" : [],
    "sparc32plus" : [],
    "sparc64" : [],
    "sparc" : [],
    "xtensaeb" : [],
    "xtensa" : [],
}

def run_command(argv):
    proc = subprocess.Popen(argv, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    out,err = proc.communicate()
    if proc.wait() != 0:
        print("Command exited with %d:\n%s\n" % (proc.returncode," ".join(argv)))
        print(f"out:\n{out}\n")
        print(f"err:\n{err}\n")

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

    argv += ["-Wno-unknown-warning-option",  "-emit-llvm", "-O0", "-Xclang", "-disable-O0-optnone", "-o", output_path]

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

def should_skip_file(source_dir, target, path):
    #print("  Looking for ")
    #print(f"  {path}")
    #print("    in ")
    #print(f"  {source_dir}{helpers[target]}")

    return not os.path.relpath(path, source_dir) in helpers[target]

def main():
    parser = argparse.ArgumentParser(description='Produce the LLVM IR for a given source file.')
    parser.add_argument('source_dir',  metavar='SOURCE_DIR',            help='qemu source directory')
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

    # Check that all paths in `helper` actually exist
    has_nonexistent_files = False
    for target in helpers:
        for file in helpers[target]:
            if not os.path.exists(os.path.join(args.source_dir, file)):
                has_nonexistent_files = True
                print(f"Error for {target}: File does not exist {file}")
    assert(not has_nonexistent_files)

    # Generate LLVM IR
    ll_paths = []
    print(args.source_dir)
    for input_path in args.input_paths:
        should_skip = should_skip_file(args.source_dir, args.target_name, os.path.abspath(input_path))
        print(f"[{should_skip}] {os.path.abspath(input_path)}")
        if should_skip:
            continue
        output_path = temp_file(output_folder, input_path)
        print(f"  -> {output_path}")
        ll_paths.append(output_path)
        generate_llvm_ir(args.target, args.clang, input_path, output_path)

    link(args.llvm_link, ll_paths, args.output)

if __name__ == "__main__":
    sys.exit(main())
