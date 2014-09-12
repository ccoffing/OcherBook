#ifndef LIBCLC_HASH_H
#define LIBCLC_HASH_H

/** @file
 *  MurmurHash2 is a fast public-domain hash
 *
 *  MurmurHash2
 *  @author Austin Appleby
 *  @sa http://murmurhash.googlepages.com
 *
 *  Requirements when selecting a hash:
 *   - Fast
 *     (murmur is 10% faster than jenkins lookup3; 5x faster than SHA1)
 *   - Small code size; prefer no lookup tables
 *     (murmur can be as small as 52 bytes)
 *   - Excellent distribution, since may be "hashing" a single 4-byte pointer
 *     (http://murmurhash.googlepages.com/avalanche)
 *  Non-requirements:
 *   - Deterministic results across different platforms
 */

#include <stdint.h>

//#include "clc/crypto/Digest.h"


namespace clc
{

//class Murmur2 : public Digest
//{
//public:
//    Murmur2();
//
//};

/**
 *  Hashes a chunk of data.
 *  @param key
 *  @param len
 *  @return A 32 bit hash value.
 *  @note This hash is not necessarily consistent across different endiannesses.
 */
uint32_t hash(const uint8_t* key, unsigned int len);

inline uint32_t hash(const char* key, unsigned int len) { return hash((const uint8_t*)key, len); }
inline uint32_t hash(const void* key, unsigned int len) { return hash((const uint8_t*)key, len); }

}

#endif
