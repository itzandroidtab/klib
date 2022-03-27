#include <limits.h>
#include <stdlib.h>

/**
 * @brief Mem compare
 * 
 * @param ptr1 
 * @param ptr2 
 * @param num 
 * @return int 
 */
int memcmp(const void *ptr1, const void *ptr2, size_t num);

/**
 * @brief Memset
 * 
 * @param ptr 
 * @param value 
 * @param num 
 * @return void* 
 */
void *memset(void *ptr, int value, size_t num);

/**
 * @brief Mem copy
 * 
 * @param destination 
 * @param source 
 * @param num 
 * @return void* 
 */
void *memcpy(void *destination, const void *source, size_t num);
