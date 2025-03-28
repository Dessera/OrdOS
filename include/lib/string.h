#pragma once

#include "lib/types.h"

/**
 * @brief Copy a string
 *
 * @param dest Destination buffer
 * @param src Source string
 * @return char* Pointer to the destination buffer
 */
char*
strcpy(char* dest, const char* src);

/**
 * @brief Copy a string, but only copy up to n characters
 *
 * @param dest Destination buffer
 * @param src Source string
 * @param n Maximum number of characters to copy
 * @return char* Pointer to the destination buffer
 */
char*
strncpy(char* dest, const char* src, size_t n);

/**
 * @brief Get the length of a string
 *
 * @param str String to get the length of
 * @return size_t Length of the string
 */
size_t
strlen(const char* str);

/**
 * @brief Compare two strings
 *
 * @param str1 First string
 * @param str2 Second string
 * @return ssize_t The difference between the first non-matching character, -1
 * if str1 < str2, 1 if str1 > str2, 0 if they are equal
 */
ssize_t
strcmp(const char* str1, const char* str2);

/**
 * @brief Find the first occurrence of a character in a string
 *
 * @param str String to search
 * @param c Character to search for
 * @return char* Pointer to the first occurrence of the character, or NULL if
 * not
 */
char*
strchr(const char* str, const char c);

/**
 * @brief Find the last occurrence of a character in a string
 *
 * @param str String to search
 * @param c Character to search for
 * @return char* Pointer to the last occurrence of the character, or NULL if not
 * found
 */
char*
strrchr(const char* str, const char c);

/**
 * @brief Concatenate two strings
 *
 * @param dest Destination string
 * @param src Source string
 * @return char* Pointer to the destination string
 */
char*
strcat(char* dest, const char* src);

/**
 * @brief Count the number of occurrences of a character in a string
 *
 * @param str String to search
 * @param c Character to search for
 * @return size_t Number of occurrences
 */
size_t
strchrs(const char* str, const char c);

/**
 * @brief Copy memory from one location to another
 *
 * @param dest Destination
 * @param src Source
 * @param n Number of bytes to copy
 */
void
memcpy(void* dest, const void* src, size_t n);

/**
 * @brief Set memory to a value
 *
 * @param dest Destination
 * @param c Value to set
 * @param n Number of bytes to set
 */
void
memset(void* dest, u8 c, size_t n);

/**
 * @brief Compare memory
 *
 * @param s1 First memory location
 * @param s2 Second memory location
 * @param n Number of bytes to compare
 * @return ssize_t Return 0 if equal, -1 if s1 < s2, 1 if s1 > s2
 */
ssize_t
memcmp(const void* s1, const void* s2, size_t n);