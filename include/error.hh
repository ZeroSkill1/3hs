
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


void report_error(error_container& container, std::string note = "");
std::string format_err(std::string msg, Result code);
error_container get_error(Result res);
std::string pad8code(Result code);

#endif

