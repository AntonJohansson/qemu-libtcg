#include "dummy.h"
#include "linux-user/qemu.h"
#include "elf.h"
#include "target_syscall.h"

uintptr_t guest_base;
unsigned long reserved_va;

void cpu_loop_exit_sigbus(CPUState *cpu, target_ulong addr,
                          MMUAccessType access_type, uintptr_t ra)
{
}

void cpu_loop_exit_sigsegv(CPUState *cpu, target_ulong addr,
                           MMUAccessType access_type, bool maperr, uintptr_t ra)
{
}

bool have_mmap_lock(void)
{
    return false;
}

void *lock_user(int type, abi_ulong guest_addr, ssize_t len, bool copy)
{
    return NULL;
}

void mmap_lock(void)
{
}

void mmap_unlock(void)
{
}

bool qemu_cpu_is_self(CPUState *cpu)
{
    return false;
}

void qemu_cpu_kick(CPUState *cpu)
{
}

void qemu_plugin_add_dyn_cb_arr(GArray *arr)
{
}

void qemu_plugin_flush_cb(void)
{
}

void qemu_plugin_tb_trans_cb(CPUState *cpu, void *tb)
{
}

void qemu_plugin_vcpu_init_hook(CPUState *cpu) {}
void qemu_plugin_vcpu_exit_hook(CPUState *cpu) {}

void qemu_plugin_vcpu_mem_cb(CPUState *cpu, uint64_t vaddr,
                             MemOpIdx oi, int rw)
{
}

void gdb_register_coprocessor(CPUState *cpu,
                              void * get_reg, void * set_reg,
                              int num_regs, const char *xml, int g_pos)
{
}

#if defined(TARGET_PPC) || defined(TARGET_PPC64)
uint64_t cpu_ppc_load_atbl(CPUArchState *env) {return 0;}
uint32_t cpu_ppc_load_atbu(CPUArchState *env) {return 0;}
uint64_t cpu_ppc_load_tbl(CPUArchState *env) {return 0;}
uint32_t cpu_ppc_load_tbu(CPUArchState *env) {return 0;}
uint64_t cpu_ppc_load_vtb(CPUArchState *env) {return 0;}
#endif

#ifdef TARGET_I386
uint64_t cpu_get_tsc(CPUArchState *env) {return 0;}
#endif



#if defined(TARGET_ALPHA)
void target_cpu_copy_regs(CPUArchState *env, struct target_pt_regs *regs)
{
    int i;

    for(i = 0; i < 28; i++) {
        env->ir[i] = ((abi_ulong *)regs)[i];
    }
    env->ir[IR_SP] = regs->usp;
    env->pc = regs->pc;
}
#elif defined(TARGET_ARM)
    #if defined(TARGET_AARCH64)
void target_cpu_copy_regs(CPUArchState *env, struct target_pt_regs *regs)
{
#if !defined(GEN_LLVM_HELPERS) && !defined(CONFIG_LIBTCG)
    ARMCPU *cpu = env_archcpu(env);
#endif

    CPUState *cs = env_cpu(env);
    TaskState *ts = cs->opaque;
    struct image_info *info = ts->info;
    int i;

    if (!(arm_feature(env, ARM_FEATURE_AARCH64))) {
        fprintf(stderr,
                "The selected ARM CPU does not support 64 bit mode\n");
        exit(EXIT_FAILURE);
    }

    for (i = 0; i < 31; i++) {
        env->xregs[i] = regs->regs[i];
    }
    env->pc = regs->pc;
    env->xregs[31] = regs->sp;
#if TARGET_BIG_ENDIAN
    env->cp15.sctlr_el[1] |= SCTLR_E0E;
    for (i = 1; i < 4; ++i) {
        env->cp15.sctlr_el[i] |= SCTLR_EE;
    }
    arm_rebuild_hflags(env);
#endif

#if !defined(GEN_LLVM_HELPERS) && !defined(CONFIG_LIBTCG)
    if (cpu_isar_feature(aa64_pauth, cpu)) {
        qemu_guest_getrandom_nofail(&env->keys, sizeof(env->keys));
    }
#endif

    ts->stack_base = info->start_stack;
    ts->heap_base = info->brk;
    /* This will be filled in on the first SYS_HEAPINFO call.  */
    ts->heap_limit = 0;
}
    #else
void target_cpu_copy_regs(CPUArchState *env, struct target_pt_regs *regs)
{
    CPUState *cpu = env_cpu(env);
    TaskState *ts = cpu->opaque;
    struct image_info *info = ts->info;
    int i;

    cpsr_write(env, regs->uregs[16], CPSR_USER | CPSR_EXEC,
               CPSRWriteByInstr);
    for(i = 0; i < 16; i++) {
        env->regs[i] = regs->uregs[i];
    }
#if TARGET_BIG_ENDIAN
    /* Enable BE8.  */
    if (EF_ARM_EABI_VERSION(info->elf_flags) >= EF_ARM_EABI_VER4
        && (info->elf_flags & EF_ARM_BE8)) {
        env->uncached_cpsr |= CPSR_E;
        env->cp15.sctlr_el[1] |= SCTLR_E0E;
    } else {
        env->cp15.sctlr_el[1] |= SCTLR_B;
    }
    arm_rebuild_hflags(env);
#endif

    ts->stack_base = info->start_stack;
    ts->heap_base = info->brk;
    /* This will be filled in on the first SYS_HEAPINFO call.  */
    ts->heap_limit = 0;
}
    #endif
#elif defined(TARGET_AVR)
#elif defined(TARGET_CRIS)
void target_cpu_copy_regs(CPUArchState *env, struct target_pt_regs *regs)
{
    CPUState *cpu = env_cpu(env);
    TaskState *ts = cpu->opaque;
    struct image_info *info = ts->info;

    env->regs[0] = regs->r0;
    env->regs[1] = regs->r1;
    env->regs[2] = regs->r2;
    env->regs[3] = regs->r3;
    env->regs[4] = regs->r4;
    env->regs[5] = regs->r5;
    env->regs[6] = regs->r6;
    env->regs[7] = regs->r7;
    env->regs[8] = regs->r8;
    env->regs[9] = regs->r9;
    env->regs[10] = regs->r10;
    env->regs[11] = regs->r11;
    env->regs[12] = regs->r12;
    env->regs[13] = regs->r13;
    env->regs[14] = info->start_stack;
    env->regs[15] = regs->acr;
    env->pc = regs->erp;
}
#elif defined(TARGET_HEXAGON)
void target_cpu_copy_regs(CPUArchState *env, struct target_pt_regs *regs)
{
    env->gpr[HEX_REG_PC] = regs->sepc;
    env->gpr[HEX_REG_SP] = regs->sp;
    env->gpr[HEX_REG_USR] = 0x56000;
}
#elif defined(TARGET_HPPA)
void target_cpu_copy_regs(CPUArchState *env, struct target_pt_regs *regs)
{
    int i;
    for (i = 1; i < 32; i++) {
        env->gr[i] = regs->gr[i];
    }
    env->iaoq_f = regs->iaoq[0];
    env->iaoq_b = regs->iaoq[1];
}
#elif defined(TARGET_I386)
void target_cpu_copy_regs(CPUArchState *env, struct target_pt_regs *regs)
{
    CPUState *cpu = env_cpu(env);
    bool is64 = (env->features[FEAT_8000_0001_EDX] & CPUID_EXT2_LM) != 0;
#if !defined(GEN_LLVM_HELPERS) && !defined(CONFIG_LIBTCG)
    int i;
#endif

    OBJECT(cpu)->free = NULL;
    env->cr[0] = CR0_PG_MASK | CR0_WP_MASK | CR0_PE_MASK;
    env->hflags |= HF_PE_MASK | HF_CPL_MASK;
    if (env->features[FEAT_1_EDX] & CPUID_SSE) {
        env->cr[4] |= CR4_OSFXSR_MASK;
        env->hflags |= HF_OSFXSR_MASK;
    }

    /* enable 64 bit mode if possible */
    if (is64) {
        env->cr[4] |= CR4_PAE_MASK;
        env->efer |= MSR_EFER_LMA | MSR_EFER_LME;
        env->hflags |= HF_LMA_MASK;
    }
#ifndef TARGET_ABI32
    else {
        fprintf(stderr, "The selected x86 CPU does not support 64 bit mode\n");
        exit(EXIT_FAILURE);
    }
#endif

    /* flags setup : we activate the IRQs by default as in user mode */
    env->eflags |= IF_MASK;

    /* linux register setup */
#ifndef TARGET_ABI32
    env->regs[R_EAX] = regs->rax;
    env->regs[R_EBX] = regs->rbx;
    env->regs[R_ECX] = regs->rcx;
    env->regs[R_EDX] = regs->rdx;
    env->regs[R_ESI] = regs->rsi;
    env->regs[R_EDI] = regs->rdi;
    env->regs[R_EBP] = regs->rbp;
    env->regs[R_ESP] = regs->rsp;
    env->eip = regs->rip;
#else
    env->regs[R_EAX] = regs->eax;
    env->regs[R_EBX] = regs->ebx;
    env->regs[R_ECX] = regs->ecx;
    env->regs[R_EDX] = regs->edx;
    env->regs[R_ESI] = regs->esi;
    env->regs[R_EDI] = regs->edi;
    env->regs[R_EBP] = regs->ebp;
    env->regs[R_ESP] = regs->esp;
    env->eip = regs->eip;
#endif

    /* linux interrupt setup */
#ifndef TARGET_ABI32
    env->idt.limit = 511;
#else
    env->idt.limit = 255;
#endif

    cpu_x86_load_seg(env, R_CS, __USER_CS);
    cpu_x86_load_seg(env, R_SS, __USER_DS);
#ifdef TARGET_ABI32
    cpu_x86_load_seg(env, R_DS, __USER_DS);
    cpu_x86_load_seg(env, R_ES, __USER_DS);
    cpu_x86_load_seg(env, R_FS, __USER_DS);
    cpu_x86_load_seg(env, R_GS, __USER_DS);
    /* This hack makes Wine work... */
    env->segs[R_FS].selector = 0;
#else
    cpu_x86_load_seg(env, R_DS, 0);
    cpu_x86_load_seg(env, R_ES, 0);
    cpu_x86_load_seg(env, R_FS, 0);
    cpu_x86_load_seg(env, R_GS, 0);
#endif
}
#elif defined(TARGET_M68K)
void target_cpu_copy_regs(CPUArchState *env, struct target_pt_regs *regs)
{
    CPUState *cpu = env_cpu(env);
    TaskState *ts = cpu->opaque;
    struct image_info *info = ts->info;

    env->pc = regs->pc;
    env->dregs[0] = regs->d0;
    env->dregs[1] = regs->d1;
    env->dregs[2] = regs->d2;
    env->dregs[3] = regs->d3;
    env->dregs[4] = regs->d4;
    env->dregs[5] = regs->d5;
    env->dregs[6] = regs->d6;
    env->dregs[7] = regs->d7;
    env->aregs[0] = regs->a0;
    env->aregs[1] = regs->a1;
    env->aregs[2] = regs->a2;
    env->aregs[3] = regs->a3;
    env->aregs[4] = regs->a4;
    env->aregs[5] = regs->a5;
    env->aregs[6] = regs->a6;
    env->aregs[7] = regs->usp;
    env->sr = regs->sr;

    ts->stack_base = info->start_stack;
    ts->heap_base = info->brk;
    /* This will be filled in on the first SYS_HEAPINFO call.  */
    ts->heap_limit = 0;
}
#elif defined(TARGET_MICROBLAZE)
void target_cpu_copy_regs(CPUArchState *env, struct target_pt_regs *regs)
{
    env->regs[0] = regs->r0;
    env->regs[1] = regs->r1;
    env->regs[2] = regs->r2;
    env->regs[3] = regs->r3;
    env->regs[4] = regs->r4;
    env->regs[5] = regs->r5;
    env->regs[6] = regs->r6;
    env->regs[7] = regs->r7;
    env->regs[8] = regs->r8;
    env->regs[9] = regs->r9;
    env->regs[10] = regs->r10;
    env->regs[11] = regs->r11;
    env->regs[12] = regs->r12;
    env->regs[13] = regs->r13;
    env->regs[14] = regs->r14;
    env->regs[15] = regs->r15;
    env->regs[16] = regs->r16;
    env->regs[17] = regs->r17;
    env->regs[18] = regs->r18;
    env->regs[19] = regs->r19;
    env->regs[20] = regs->r20;
    env->regs[21] = regs->r21;
    env->regs[22] = regs->r22;
    env->regs[23] = regs->r23;
    env->regs[24] = regs->r24;
    env->regs[25] = regs->r25;
    env->regs[26] = regs->r26;
    env->regs[27] = regs->r27;
    env->regs[28] = regs->r28;
    env->regs[29] = regs->r29;
    env->regs[30] = regs->r30;
    env->regs[31] = regs->r31;
    env->pc = regs->pc;
}
#elif defined(TARGET_MIPS)

#include "fpu_helper.h"

void target_cpu_copy_regs(CPUArchState *env, struct target_pt_regs *regs)
{
    CPUState *cpu = env_cpu(env);
    TaskState *ts = cpu->opaque;
    struct image_info *info = ts->info;
    int i;

    struct mode_req {
        bool single;
        bool soft;
        bool fr1;
        bool frdefault;
        bool fre;
    };

    static const struct mode_req fpu_reqs[] = {
        [MIPS_ABI_FP_ANY]    = { true,  true,  true,  true,  true  },
        [MIPS_ABI_FP_DOUBLE] = { false, false, false, true,  true  },
        [MIPS_ABI_FP_SINGLE] = { true,  false, false, false, false },
        [MIPS_ABI_FP_SOFT]   = { false, true,  false, false, false },
        [MIPS_ABI_FP_OLD_64] = { false, false, false, false, false },
        [MIPS_ABI_FP_XX]     = { false, false, true,  true,  true  },
        [MIPS_ABI_FP_64]     = { false, false, true,  false, false },
        [MIPS_ABI_FP_64A]    = { false, false, true,  false, true  }
    };

    /*
     * Mode requirements when .MIPS.abiflags is not present in the ELF.
     * Not present means that everything is acceptable except FR1.
     */
    static struct mode_req none_req = { true, true, false, true, true };

    struct mode_req prog_req;
    struct mode_req interp_req;

    for(i = 0; i < 32; i++) {
        env->active_tc.gpr[i] = regs->regs[i];
    }
    env->active_tc.PC = regs->cp0_epc & ~(target_ulong)1;
    if (regs->cp0_epc & 1) {
        env->hflags |= MIPS_HFLAG_M16;
    }

#ifdef TARGET_ABI_MIPSO32
# define MAX_FP_ABI MIPS_ABI_FP_64A
#else
# define MAX_FP_ABI MIPS_ABI_FP_SOFT
#endif
     if ((info->fp_abi > MAX_FP_ABI && info->fp_abi != MIPS_ABI_FP_UNKNOWN)
        || (info->interp_fp_abi > MAX_FP_ABI &&
            info->interp_fp_abi != MIPS_ABI_FP_UNKNOWN)) {
        fprintf(stderr, "qemu: Unexpected FPU mode\n");
        exit(1);
    }

    prog_req = (info->fp_abi == MIPS_ABI_FP_UNKNOWN) ? none_req
                                            : fpu_reqs[info->fp_abi];
    interp_req = (info->interp_fp_abi == MIPS_ABI_FP_UNKNOWN) ? none_req
                                            : fpu_reqs[info->interp_fp_abi];

    prog_req.single &= interp_req.single;
    prog_req.soft &= interp_req.soft;
    prog_req.fr1 &= interp_req.fr1;
    prog_req.frdefault &= interp_req.frdefault;
    prog_req.fre &= interp_req.fre;

    bool cpu_has_mips_r2_r6 = env->insn_flags & ISA_MIPS_R2 ||
                              env->insn_flags & ISA_MIPS_R6;

    if (prog_req.fre && !prog_req.frdefault && !prog_req.fr1) {
        env->CP0_Config5 |= (1 << CP0C5_FRE);
        if (env->active_fpu.fcr0 & (1 << FCR0_FREP)) {
            env->hflags |= MIPS_HFLAG_FRE;
        }
    } else if ((prog_req.fr1 && prog_req.frdefault) ||
         (prog_req.single && !prog_req.frdefault)) {
        if ((env->active_fpu.fcr0 & (1 << FCR0_F64)
            && cpu_has_mips_r2_r6) || prog_req.fr1) {
            env->CP0_Status |= (1 << CP0St_FR);
            env->hflags |= MIPS_HFLAG_F64;
        }
    } else if (prog_req.fr1) {
        env->CP0_Status |= (1 << CP0St_FR);
        env->hflags |= MIPS_HFLAG_F64;
    } else if (!prog_req.fre && !prog_req.frdefault &&
          !prog_req.fr1 && !prog_req.single && !prog_req.soft) {
        fprintf(stderr, "qemu: Can't find a matching FPU mode\n");
        exit(1);
    }

    if (env->insn_flags & ISA_NANOMIPS32) {
        return;
    }
    if (((info->elf_flags & EF_MIPS_NAN2008) != 0) !=
        ((env->active_fpu.fcr31 & (1 << FCR31_NAN2008)) != 0)) {
        if ((env->active_fpu.fcr31_rw_bitmask &
              (1 << FCR31_NAN2008)) == 0) {
            fprintf(stderr, "ELF binary's NaN mode not supported by CPU\n");
            exit(1);
        }
        if ((info->elf_flags & EF_MIPS_NAN2008) != 0) {
            env->active_fpu.fcr31 |= (1 << FCR31_NAN2008);
        } else {
            env->active_fpu.fcr31 &= ~(1 << FCR31_NAN2008);
        }
        restore_snan_bit_mode(env);
    }
}
#elif defined(TARGET_NIOS2)
void target_cpu_copy_regs(CPUArchState *env, struct target_pt_regs *regs)
{
    env->regs[R_SP] = regs->sp;
    env->pc = regs->ea;
}
#elif defined(TARGET_OPENRISC)
void target_cpu_copy_regs(CPUArchState *env, struct target_pt_regs *regs)
{
    int i;

    for (i = 0; i < 32; i++) {
        cpu_set_gpr(env, i, regs->gpr[i]);
    }
    env->pc = regs->pc;
    cpu_set_sr(env, regs->sr);
}
#elif defined(TARGET_PPC)
void target_cpu_copy_regs(CPUArchState *env, struct target_pt_regs *regs)
{
    int i;

#if defined(TARGET_PPC64)
    int flag = (env->insns_flags2 & PPC2_BOOKE206) ? MSR_CM : MSR_SF;
#if defined(TARGET_ABI32)
    ppc_store_msr(env, env->msr & ~((target_ulong)1 << flag));
#else
    ppc_store_msr(env, env->msr | (target_ulong)1 << flag);
#endif
#endif

    env->nip = regs->nip;
    for(i = 0; i < 32; i++) {
        env->gpr[i] = regs->gpr[i];
    }
}
#elif defined(TARGET_RISCV32) || defined(TARGET_RISCV64)
void target_cpu_copy_regs(CPUArchState *env, struct target_pt_regs *regs)
{
    CPUState *cpu = env_cpu(env);
    TaskState *ts = cpu->opaque;
    struct image_info *info = ts->info;

    env->pc = regs->sepc;
    env->gpr[xSP] = regs->sp;
    env->elf_flags = info->elf_flags;

    if ((env->misa_ext & RVE) && !(env->elf_flags & EF_RISCV_RVE)) {
        exit(EXIT_FAILURE);
    }

    ts->stack_base = info->start_stack;
    ts->heap_base = info->brk;
    /* This will be filled in on the first SYS_HEAPINFO call.  */
    ts->heap_limit = 0;
}
#elif defined(TARGET_RX)
#elif defined(TARGET_S390X)
void target_cpu_copy_regs(CPUArchState *env, struct target_pt_regs *regs)
{
    int i;
    for (i = 0; i < 16; i++) {
        env->regs[i] = regs->gprs[i];
    }
    env->psw.mask = regs->psw.mask;
    env->psw.addr = regs->psw.addr;
}
#elif defined(TARGET_SH4)
void target_cpu_copy_regs(CPUArchState *env, struct target_pt_regs *regs)
{
    int i;

    for(i = 0; i < 16; i++) {
        env->gregs[i] = regs->regs[i];
    }
    env->pc = regs->pc;
}
#elif defined(TARGET_SPARC)
void target_cpu_copy_regs(CPUArchState *env, struct target_pt_regs *regs)
{
    int i;
    env->pc = regs->pc;
    env->npc = regs->npc;
    env->y = regs->y;
    for(i = 0; i < 8; i++)
        env->gregs[i] = regs->u_regs[i];
    for(i = 0; i < 8; i++)
        env->regwptr[i] = regs->u_regs[i + 8];
}
#elif defined(TARGET_TRICORE)
#elif defined(TARGET_XTENSA)
void target_cpu_copy_regs(CPUArchState *env, struct target_pt_regs *regs)
{
    int i;
    for (i = 0; i < 16; ++i) {
        env->regs[i] = regs->areg[i];
    }
    env->sregs[WINDOW_START] = regs->windowstart;
    env->pc = regs->pc;
}
#elif defined(TARGET_LOONGARCH64)
void target_cpu_copy_regs(CPUArchState *env, struct target_pt_regs *regs)
{
    int i;

    for (i = 0; i < 32; i++) {
        env->gpr[i] = regs->regs[i];
    }
    env->pc = regs->csr.era;

}
#else
    #error Unhandled target
#endif
