.section ".text"
.globl thread_switch_to
thread_switch_to:
  pushl %esi
  pushl %edi
  pushl %ebx
  pushl %ebp
  
  movl 20(%esp), %eax
  movl %esp, (%eax)

  movl 24(%esp), %eax
  movl (%eax), %esp

  popl %ebp
  popl %ebx
  popl %edi
  popl %esi
  ret