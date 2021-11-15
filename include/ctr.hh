
#ifndef inc_ctr_hh
#define inc_ctr_hh

#include <vector>
#include <string>

#include "install.hh"


namespace ctr {
	enum class RegionLockout : u32
	{
		JPN = 0x01, USA = 0x02, EUR = 0x04,
		AUS = 0x08, CHN = 0x10, KOR = 0x20,
		TWN = 0x40, WORLD = 0x7FFFFFFF,
	};

	enum class Region
	{
		JPN, USA, EUR, // EUR includes AUS
		CHN, KOR, TWN,
		WORLD,
	};

	enum class Ratings : u32
	{
		CERO       = 1 << 0,  // Japan
		ESRB       = 1 << 1,  // USA
		_reserved0 = 1 << 2,
		USK        = 1 << 3,  // Germany
		PEGI_GEN   = 1 << 4,  // Europe
		_reserved1 = 1 << 5,
		PEGI_PRT   = 1 << 6,  // Portugal
		PEGI_BBFC  = 1 << 7,  // England
		COB        = 1 << 8,  // Australia
		GRB        = 1 << 9,  // South Korea
		CGSRR      = 1 << 10, // Taiwan
		_reserved2 = 1 << 11,
		_reserved3 = 1 << 12,
		_reserved5 = 1 << 13,
		_reserved6 = 1 << 14,
		_reserved7 = 1 << 15,
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
		TitleSMDHTitle titles[0x10];

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

	u64 str_to_tid(const std::string& str);
	std::string tid_to_str(u64 tid);

	Result list_titles_on(FS_MediaType media, std::vector<u64>& ret);
	Result get_free_space(Destination media, u64 *size);

	namespace smdh
	{
		TitleSMDHTitle *get_native_title(TitleSMDH *smdh);
		std::string u16conv(u16 *str, size_t size);
		TitleSMDH *get(u64 tid);
	}
}

#endif

