
#ifndef inc_error_hh
#define inc_error_hh

#include <string>

#define APPERR_NOSUPPORT MAKERESULT(RL_PERMANENT, RS_NOTSUPPORTED, RM_APPLICATION, 0)
#define APPERR_CANCELLED MAKERESULT(RL_TEMPORARY, RS_CANCELED, RM_APPLICATION, 1)
#define APPERR_NOSPACE MAKERESULT(RL_TEMPORARY, RS_OUTOFRESOURCE, RM_APPLICATION, 2)
#define APPERR_NOREINSTALL MAKERESULT(RL_FATAL, RS_INVALIDSTATE, RM_APPLICATION, 3)
#define APPERR_TITLE_MISMATCH MAKERESULT(RL_TEMPORARY, RS_OUTOFRESOURCE, RM_APPLICATION, 4)
#define APPERR_NORANGE MAKERESULT(RL_PERMANENT, RS_NOTSUPPORTED, RM_APPLICATION, 5)
#define APPERR_NOSIZE MAKERESULT(RL_PERMANENT, RS_NOTSUPPORTED, RM_APPLICATION, 6)
#define APPERR_JSON_FAIL MAKERESULT(RL_TEMPORARY, RS_INVALIDSTATE, RM_APPLICATION, 7)

typedef struct error_container
{

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
std::string format_err(const std::string& msg, Result code);
error_container get_error(Result res);
std::string pad8code(Result code);

#endif

