project('passes', ['cpp'],
        meson_version: '>=0.63.0',
        version: '0.7',
        default_options: ['cpp_std=none', 'optimization=2'])

llvm_dep  = dependency('llvm', modules: ['support'])

ldflags = llvm_dep.get_variable(configtool: 'ldflags').strip().split()
libs = llvm_dep.get_variable(configtool: 'libs').strip().split()
syslibs = llvm_dep.get_variable(configtool: 'system-libs').strip().split()
cxxflags = llvm_dep.get_variable(configtool: 'cxxflags').strip().split()

message('ARST')

dump_fp = shared_library('dump-fp',
  sources: files('dump-fp.cpp'),
  include_directories: llvm_dep.get_variable(configtool: 'includedir'),
  cpp_args: ['-fno-rtti'] + cxxflags,
  link_args: ['-fno-rtti'] + ldflags + libs + syslibs,
  dependencies: llvm_dep,
)
