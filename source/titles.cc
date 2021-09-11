
#include "install.hh"
#include "titles.hh"
#include "error.hh"
#include "panic.hh"

#include <string.h>

#define makebin(data) makebin_(data, sizeof(data))
static FS_Path makebin_(const void *path, u32 size)
{
	return { PATH_BINARY, size, path };
}

TitleSMDH *smdh_get(u64 tid)
{
	static const u32 smdhPath[5] = AEXEFS_SMDH_PATH;
	u32 exefsArchivePath[4] = MAKE_EXEFS_APATH(tid, to_mediatype(detect_dest(tid)));

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

TitleSMDHTitle *smdh_get_native_title(TitleSMDH *smdh)
{
	u8 syslang = 0;
	if(R_FAILED(CFGU_GetSystemLanguage(&syslang)))
		return &smdh->titles[1]; // english

	switch(syslang)
	{
	case CFG_LANGUAGE_JP: return &smdh->titles[0];
	case CFG_LANGUAGE_EN: return &smdh->titles[1];
	case CFG_LANGUAGE_FR: return &smdh->titles[2];
	case CFG_LANGUAGE_DE: return &smdh->titles[3];
	case CFG_LANGUAGE_IT: return &smdh->titles[4];
	case CFG_LANGUAGE_ES: return &smdh->titles[5];
	case CFG_LANGUAGE_ZH: return &smdh->titles[6];
	case CFG_LANGUAGE_KO: return &smdh->titles[7];
	case CFG_LANGUAGE_NL: return &smdh->titles[8];
	case CFG_LANGUAGE_PT: return &smdh->titles[9];
	case CFG_LANGUAGE_RU: return &smdh->titles[10];
	case CFG_LANGUAGE_TW: return &smdh->titles[11];
	}

	return nullptr; // unreachable
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

