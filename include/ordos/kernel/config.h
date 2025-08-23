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

#define ORDOS_KERNEL_NAME "OrdOS"     /**< Kernel name string. */
#define ORDOS_KERNEL_VERSION "v0.1.0" /**< Kernel version string. */
#define ORDOS_KERNEL_LADDR 0x100000   /**< Kernel load address. */
#define ORDOS_KERNEL_VADDR 0xc0000000 /**< Kernel virtual address. */
#define ORDOS_KERNEL_PAGE_SIZE 0x1000 /**< Kernel pagetable size. */
#define ORDOS_INIT_STACKSIZE 0x1000   /**< Init stack size when boot. */
#define ORDOS_INIT_MMAP_LENGTH 64     /**< Init memory map size. */
#define ORDOS_INIT_ARGS_BUFSIZE 0x200 /**< Init args size, */
#define ORDOS_IO_BUFSIZE 0x200        /**< IO buffer size. */
