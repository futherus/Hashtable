#ifndef HASH_H
#define HASH_H

#include <stddef.h>
#include <stdint.h>

uint32_t crc32(const void *buf, size_t size);

#endif // HASH_H