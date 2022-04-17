#ifndef HASH_H
#define HASH_H

#include <stddef.h>
#include <stdint.h>

/**
 * Get 64-bit FNV1 hash integer.
 *
 * \param data      text source data
 * \param nbytes    size of data
 *
 * \return 64-bit unsigned hash value.
 *
**/
uint64_t qhashfnv1_64(const void *data, size_t nbytes);

#endif // HASH_H