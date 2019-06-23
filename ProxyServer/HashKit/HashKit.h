#ifndef HASH_KIT
#define HASH_KIT

#include <stdint.h> // for uint32_t
#include <stddef.h> // for size_t

#ifdef __cplusplus
extern "C" {
#endif

typedef uint32_t (*hasher)(const char *key, size_t key_length);

uint32_t hash_md5(const char *key, size_t key_length);
uint32_t hash_crc16(const char *key, size_t key_length);
uint32_t hash_crc32(const char *key, size_t key_length);
uint32_t hash_crc32a(const char *key, size_t key_length);
uint32_t hash_fnv1_64(const char *key, size_t key_length);
uint32_t hash_fnv1a_64(const char *key, size_t key_length);
uint32_t hash_fnv1_32(const char *key, size_t key_length);
uint32_t hash_fnv1a_32(const char *key, size_t key_length);
uint32_t hash_hsieh(const char *key, size_t key_length);
uint32_t hash_jenkins(const char *key, size_t length);
uint32_t hash_murmur(const char *key, size_t length);

#ifdef __cplusplus
}
#endif


#endif // HASH_KIT
