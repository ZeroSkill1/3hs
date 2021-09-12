
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
	TitleSMDHTitle *title = nullptr;
	u8 syslang;

	if(R_SUCCEEDED(CFGU_GetSystemLanguage(&syslang)))
	{
		switch(syslang)
		{
		case CFG_LANGUAGE_JP: title = &smdh->titles[0]; break;
		case CFG_LANGUAGE_EN: title = &smdh->titles[1]; break;
		case CFG_LANGUAGE_FR: title = &smdh->titles[2]; break;
		case CFG_LANGUAGE_DE: title = &smdh->titles[3]; break;
		case CFG_LANGUAGE_IT: title = &smdh->titles[4]; break;
		case CFG_LANGUAGE_ES: title = &smdh->titles[5]; break;
		case CFG_LANGUAGE_ZH: title = &smdh->titles[6]; break;
		case CFG_LANGUAGE_KO: title = &smdh->titles[7]; break;
		case CFG_LANGUAGE_NL: title = &smdh->titles[8]; break;
		case CFG_LANGUAGE_PT: title = &smdh->titles[9]; break;
		case CFG_LANGUAGE_RU: title = &smdh->titles[10]; break;
		case CFG_LANGUAGE_TW: title = &smdh->titles[11]; break;
		}
	}

	if(title != nullptr && title->descShort[0] != 0)
		return title;

	// EN, JP, FR, DE, IT, ES, ZH, KO, NL, PT, RU, TW
	static const u8 lookuporder[] = { 1, 0, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
	for(u8 i = 0; i < sizeof(lookuporder); ++i)
	{
		if(smdh->titles[i].descShort[0] != 0)
			return &smdh->titles[i];
	}

	return nullptr;
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

std::string smdh_u16conv(u16 *str, size_t size)
{
	std::string ret;
	ret.reserve(size);

	for(size_t i = 0; i < size; ++i)
	{
		const u8 lower = (str[i] >> 0) & 0xFF;
		const u8 upper = (str[i] >> 8) & 0xFF;

		// We're done
		if(lower == 0) break;

		ret.push_back(lower);
		if(upper != 0) ret.push_back(upper);
	}

	return ret;
}

