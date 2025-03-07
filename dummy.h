#include "qemu/osdep.h"
#include "hw/core/cpu.h"
#include "cpu-param.h"
#include "exec/target_long.h"
#include "exec/user/abitypes.h"
#include "exec/memopidx.h"

struct target_pt_regs;

void cpu_loop_exit_sigbus(CPUState *cpu, target_ulong addr,
                          MMUAccessType access_type, uintptr_t ra);
void cpu_loop_exit_sigsegv(CPUState *cpu, target_ulong addr,
                           MMUAccessType access_type, bool maperr, uintptr_t ra);
bool have_mmap_lock(void);
//void *lock_user(int type, abi_ulong guest_addr, ssize_t len, bool copy);
void mmap_lock(void);
void mmap_unlock(void);
//bool qemu_cpu_is_self(CPUState *cpu);
//void qemu_cpu_kick(CPUState *cpu);
void qemu_plugin_add_dyn_cb_arr(GArray *arr);
void qemu_plugin_flush_cb(void);
void qemu_plugin_tb_trans_cb(CPUState *cpu, void *tb);
void qemu_plugin_vcpu_init_hook(CPUState *cpu);
void qemu_plugin_vcpu_exit_hook(CPUState *cpu);
void qemu_plugin_vcpu_mem_cb(CPUState *cpu, uint64_t vaddr,
                             MemOpIdx oi, int rw);

void target_cpu_copy_regs(CPUArchState *env, struct target_pt_regs *regs);
void gdb_register_coprocessor(CPUState *cpu,
                              void * get_reg, void * set_reg,
                              int num_regs, const char *xml, int g_pos);
