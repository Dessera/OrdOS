/**
 * @file compiler.h
 * @author Dessera (dessera@qq.com)
 * @brief Compiler and linker info.
 * @version 0.1.0
 * @date 2025-08-13
 *
 * @copyright Copyright (c) 2025 Dessera
 *
 */

#pragma once

#include "ordos/lib/common.h" // IWYU pragma: keep
#include "ordos/lib/types.h"  // IWYU pragma: keep

/**
 * @brief Linker variable to point kernel end point.
 *
 */
extern const uintptr_t __ld_kernel_end_vaddr[];

/**
 * @brief Linker variable to point kernel end point.
 *
 */
extern const uintptr_t __ld_kernel_end_paddr[];

/**
 * @brief Linker variable to point kernel module start point.
 *
 */
extern const uintptr_t __ld_kernel_modules_start_vaddr[];

/**
 * @brief Linker variable to point kernel module end point.
 *
 */
extern const uintptr_t __ld_kernel_modules_end_vaddr[];

/**
 * @brief Linker variable to point kernel start point.
 *
 */
extern const uintptr_t __ld_kernel_start_vaddr[];

/**
 * @brief Linker variable to point kernel start point.
 *
 */
extern const uintptr_t __ld_kernel_start_paddr[];

/**
 * @brief Get kernel end virtual address.
 *
 * @return uintptr_t Kernel end address.
 */
__inline static uintptr_t
compiler_kernel_end_vaddr(void)
{
  return (uintptr_t)__ld_kernel_end_vaddr;
}

/**
 * @brief Get kernel end physical address.
 *
 * @return uintptr_t Kernel end address.
 */
__inline static uintptr_t
compiler_kernel_end_paddr(void)
{
  return (uintptr_t)__ld_kernel_end_paddr;
}

/**
 * @brief Get kernel start virtual address.
 *
 * @return uintptr_t Kernel start address.
 */
__inline static uintptr_t
compiler_kernel_start_vaddr(void)
{
  return (uintptr_t)__ld_kernel_start_vaddr;
}

/**
 * @brief Get kernel start physical address.
 *
 * @return uintptr_t Kernel start address.
 */
__inline static uintptr_t
compiler_kernel_start_paddr(void)
{
  return (uintptr_t)__ld_kernel_start_paddr;
}

/**
 * @brief Get kernel module start virtual address.
 *
 * @return uintptr_t Kernel module start address.
 */
__inline static uintptr_t
compiler_kernel_module_start_vaddr(void)
{
  return (uintptr_t)__ld_kernel_modules_start_vaddr;
}

/**
 * @brief Get kernel module end virtual address.
 *
 * @return uintptr_t Kernel module end address.
 */
__inline static uintptr_t
compiler_kernel_module_end_vaddr(void)
{
  return (uintptr_t)__ld_kernel_modules_end_vaddr;
}
