syscall_nr_generators += {
  'hppa': generator(sh,
                    arguments: [ meson.current_source_dir() / 'syscallhdr.sh', '@INPUT@', '@OUTPUT@', '@EXTRA_ARGS@' ],
                    output: '@BASENAME@_nr.h')
}

if have_llvm_helpers
  vdso_inc = gen_vdso.process('vdso.so',
                              extra_args: [ '-r', '__kernel_sigtramp_rt' ])
  linux_user_ss.add(when: 'TARGET_HPPA', if_true: vdso_inc)
endif
