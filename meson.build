project(
  'OrdOS',
  'c',
  version: '0.1.0',
  meson_version: '>=1.4.0',
  default_options: ['warning_level=3', 'c_std=gnu17'],
)

compiler = meson.get_compiler('c')
sh = find_program('sh', required: true)

subdir('include')
subdir('src')
subdir('script')

kernel_args = [
  '-Wall',
  '-Werror',
  '-W',
  '-Wstrict-prototypes',
  '-Wno-empty-translation-unit',
  '-Wno-pedantic',
  '-mno-sse',
  '-fno-builtin',
  '-fno-pie',
  '-fno-pic',
  '-fno-stack-protector',
  '-nostdinc',
  '-nostdlib',
  '-z', 'noexecstack',
  '-m32',
]

kernel_ld_args = kernel_args + [
  '-T' + kernel_ld.full_path(),
  '-Wl,-Map=syms.map',
]

kernel = executable(
  'vmordos',
  kernel_src + lib_src,
  c_args: kernel_args,
  link_args: kernel_ld_args,
  link_depends: kernel_ld,
  include_directories: kernel_inc,
)

custom_target(
  'build-grub-image',
  build_by_default: true,
  command: [
    sh,
    script_mkimage.full_path(),
    kernel.full_path(),
    kernel_grub_cfg,
    '@OUTPUT@',
  ],
  output: 'boot.img',
  depend_files: [kernel_grub_cfg],
  depends: [kernel, kernel_ld],
)