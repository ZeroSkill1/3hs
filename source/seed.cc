
#include "seed.hh"

#include <string.h>
#include <3ds.h>
#include <map>

static std::map<u64 /* tid */, _Seed /* seed */> g_seeddb;


// from FBI:
// https://github.com/Steveice10/FBI/blob/6e3a28e4b674e0d7a6f234b0419c530b358957db/source/core/http.c#L440-L453
Result FSUSER_AddSeed(u64 titleId, const void* seed)
{
	u32 *cmdbuf = getThreadCommandBuffer();

	cmdbuf[0] = 0x087A0180;
	cmdbuf[1] = (u32) (titleId & 0xFFFFFFFF);
	cmdbuf[2] = (u32) (titleId >> 32);
	memcpy(&cmdbuf[3], seed, 16);

	Result ret = 0;
	if(R_FAILED(ret = svcSendSyncRequest(*fsGetSessionHandle())))
		return ret;

	ret = cmdbuf[1];
	return ret;
}

void init_seeddb()
{
	std::FILE *f = std::fopen("romfs:/perfect_seeddb.bin", "r");
	SeedDBHeader head;

	std::fread(&head, sizeof(SeedDBHeader), 1, f);
	SeedDBEntry *entries = new SeedDBEntry[head.size];
	std::fread(entries, sizeof(SeedDBEntry), head.size, f);

	for(size_t i = 0x0; i < head.size; ++i)
		g_seeddb[entries[i].tid] = entries[i].seed;

	delete [] entries;
	std::fclose(f);
}

Result add_seed(u64 tid)
{
	if(g_seeddb.count(tid) == 0) return 0x0;
	return FSUSER_AddSeed(tid, g_seeddb[tid].seed);
}
