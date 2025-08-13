kernel_src = files(
  'init.c',
  'logging.c',
  'main.c',
)

subdir('boot')
subdir('driver')
subdir('mem')

kernel_ld = compiler.preprocess(
  'kernel.ld.template',
  include_directories: kernel_inc,
  output: 'kernel.ld',
).get(0)

kernel_grub_cfg = files('grub.cfg').get(0)