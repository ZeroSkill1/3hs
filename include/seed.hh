
#ifndef inc_seed_hh
#define inc_seed_hh

#include <3ds/types.h> // u8, u16, u32, u64
#include <string>

// Seeds are 0x20 bytes (u8[0x20])
// TIDs are 0x8 bytes (u64)

typedef struct Seed
{
	u8 seed[0x10];
} Seed;

typedef struct SeedDBHeader
{
	u32 size;
	u8 pad[0xC];
} SeedDBHeader;

typedef struct SeedDBEntry
{
	u64 tid;
	Seed seed; // = u8 seed[0x20];
	u8 pad[0x8];
} SeedDBEntry;


Result FSUSER_AddSeed(u64 titleId, const void* seed);
/* Returns 0x0 (success) if no seed was found */
Result add_seed(u64 tid);
void init_seeddb();

#endif
