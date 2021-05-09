
#ifndef __error_hh__
#define __error_hh__

#include <string>


enum ErrTypes
{
	ErrType_curl,
	ErrType_3ds,
};

typedef struct error_container
{
	ErrTypes type;

	std::string sDesc;
	Result      iDesc;

	std::string sLvl;
	Result      iLvl;

	std::string sSum;
	Result      iSum;

	std::string sMod;
	Result      iMod;

	Result full;
} error_container;

#endif

