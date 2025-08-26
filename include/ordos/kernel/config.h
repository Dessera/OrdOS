/**
 * @file config.h
 * @author Dessera (dessera@qq.com)
 * @brief OrdOS kernel basic definitions.
 * @version 0.1.0
 * @date 2025-08-13
 *
 * @copyright Copyright (c) 2025 Dessera
 *
 */

#pragma once

#define ORDOS_KERNEL_NAME "OrdOS"          /**< Kernel name string. */
#define ORDOS_KERNEL_VERSION "v0.1.0"      /**< Kernel version string. */
#define ORDOS_KERNEL_LADDR 0x100000        /**< Kernel load address. */
#define ORDOS_KERNEL_VADDR 0xc0000000      /**< Kernel virtual address. */
#define ORDOS_KERNEL_PAGE_SIZE 0x1000      /**< Kernel pagetable size. */
#define ORDOS_INIT_STACKSIZE 0x1000        /**< Init stack size when boot. */
#define ORDOS_INIT_MMAP_CNT 64             /**< Init memory map size. */
#define ORDOS_INIT_ARGS_BUFSIZE 0x200      /**< Init args size, */
#define ORDOS_MEM_GDT_DESC_CNT 5           /**< GDT count. */
#define ORDOS_MEM_BUDDY_MAX_ORDER 10       /**< Memory pages max order. */
#define ORDOS_MEM_SSLAB_MAX_ORDER 10       /**< SSLAB max order. */
#define ORDOS_INTR_IDT_DESC_CNT 0x81       /**< IDT count. */
#define ORDOS_INTR_SYSCALL 0x80            /**< IDT syscall index. */
#define ORDOS_INTR_EXCEPTIONS_CNT 0x14     /**< Interrupt exceptions count. */
#define ORDOS_INTR_SYSCALLS_CNT 0x10       /**< Interrupt syscalls count. */
#define ORDOS_DEV_PIC_MASK 0xF8BF          /**< PIC master & slave mask. */
#define ORDOS_DEV_PIT_MAX_TICKS 0xFFFFFFFF /**< PIT watcher max ticks. */
#define ORDOS_IO_BUFSIZE 0x200             /**< IO buffer size. */
#define ORDOS_MODULE_MAGIC 0xAC84D5EC      /**< Kernel module magic. */
#define ORDOS_MODULE_NAME_LENGTH 0x10      /**< Kernel module name max length.*/
#define ORDOS_MODULE_BUFSIZE 0x20          /**< Kernel module buffer length.*/
#define ORDOS_SYS_DEFAULT_FREQ 1000        /**< System default frequency. */
