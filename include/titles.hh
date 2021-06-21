
#ifndef inc_titles_hh
#define inc_titles_hh

#include <3ds.h>

#define AEXEFS_SMDH_PATH              { 0x00000000, 0x00000000, 0x00000002, 0x6E6F6369, 0x00000000 }
#define MAKE_EXEFS_APATH(tid, media) { (u32) (tid & 0xFFFFFFFF), (u32) ((tid >> 32) & 0xFFFFFFFF), media, 0x00000000 }
#define SMDH_MAGIC "SMDH"
#define SMDH_MAGIC_LEN 4

enum class RegionLockout : u32
{
	JPN = 0x01, USA = 0x02, EUR = 0x04,
	AUS = 0x08, CHN = 0x10, KOR = 0x20,
	TWN = 0x40
};

enum class Ratings : u32
{
	// TODO: Fill this in
};


typedef struct TitleSMDHTitle
{
	u16 descShort[0x40]; // wchar
	u16 descLong[0x80]; // wchar
	u16 publisher[0x40]; // wchar
} TitleSMDHTitle;

// https://www.3dbrew.org/wiki/SMDH
typedef struct TitleSMDH
{
	// Header
	char magic[0x4]; // constant: 'SMDH'
	u16 version;

	u16 reserved_1;

	// Titles
	TitleSMDHTitle titles[0x10]; // TODO: Get title based on sys lang?

	// App settings
	u8 ratings[0x10]; // array, index with enum Ratings
	u32 region; // bitfield of enum RegionLockout
	u32 matchMaker;
	u64 matchMakerBit;
	u32 flags;
	u16 eulaVersion;
	u16 reserved_2;
	u32 optimalBnrFrame; // actually a float, but unused so it doesn't matter
	u32 CECID; // streetpass

	u64 reserved_3;

	// GFX
	u8 iconSmall[0x0480]; // 24x24
	u8 iconLarge[0x1200]; // 48x48
} TitleSMDH;


TitleSMDH *smdh_get(u64 tid, FS_MediaType media);

#endif

