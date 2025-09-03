kernel_src = files(
  'init.c',
  'main.c',
  'module.c',
)

kernel_ld = compiler.preprocess(
  'kernel.ldt',
  include_directories: kernel_inc,
  output: 'kernel.ld',
).get(0)

kernel_grub_cfg = files('grub.cfg').get(0)

subdir('boot')
subdir('core')
subdir('drv')
subdir('lib')