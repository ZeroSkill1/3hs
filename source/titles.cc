
#include "titles.hh"
#include "error.hh"

#include <string.h>

#define makebin(data) makebin_(data, sizeof(data))
static FS_Path makebin_(const void *path, u32 size)
{
	return { PATH_BINARY, size, path };
}

TitleSMDH *smdh_get(u64 tid, FS_MediaType media)
{
	static const u32 smdhPath[5] = AEXEFS_SMDH_PATH;
	u32 exefsArchivePath[4] = MAKE_EXEFS_APATH(tid, media);

	Handle smdhFile;
	if(R_FAILED(FSUSER_OpenFileDirectly(&smdhFile, ARCHIVE_SAVEDATA_AND_CONTENT,
			makebin(exefsArchivePath), makebin(smdhPath), FS_OPEN_READ, 0)))
		return nullptr;

	TitleSMDH *ret = new TitleSMDH; u32 bread = 0;
	memset(ret, 0x0, sizeof(TitleSMDH));

	if(R_FAILED(FSFILE_Read(smdhFile, &bread, 0, ret, sizeof(TitleSMDH))))
	{ delete ret; return nullptr; }

	// Invalid smdh
	if(memcmp(ret, SMDH_MAGIC, SMDH_MAGIC_LEN) != 0)
	{ delete ret; return nullptr; }

	FSFILE_Close(smdhFile);
	return ret;
}

Result list_titles_on(FS_MediaType media, std::vector<u64>& ret)
{
	u32 tcount = 0;
	Result res = AM_GetTitleCount(media, &tcount);
	if(R_FAILED(res)) return res;

	u32 tread = 0;
	u64 *tids = new u64[tcount];

	res = AM_GetTitleList(&tread, media, tcount, tids);
	if(R_FAILED(res)) return res;

	if(tread != tcount)
	{
		delete [] tids;
		return APPERR_TITLE_MISMATCH;
	}

	ret.reserve(tcount);
	for(u32 i = 0; i < tcount; ++i)
		ret.push_back(tids[i]);

	delete [] tids;
	return res;
}

