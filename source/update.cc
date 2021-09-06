
#include "update.hh"

#include <curl/curl.h>
#include <3rd/log.hh>

#include <ui/confirm.hh>
#include <ui/core.hh>

#include "install.hh"
#include "queue.hh"
#include "i18n.hh"


bool update_app()
{
	if(envIsHomebrew())
	{
		linfo << "Used 3dsx version. Not checking for updates";
		return false;
	}

	std::string nver = get_latest_version_string();
	linfo << "Fetched new version " << nver << " from " << UP_VERSION;
	linfo << "Current app version is " << VERSION;

	if(nver == FULL_VERSION)
		return false;

	ui::Widgets wids; bool shouldUpdate = false;
	wids.push_back("confirmation", new ui::Confirm(PSTRING(update_to, nver), shouldUpdate), ui::Scr::bottom);
	generic_main_breaking_loop(wids);

	if(!shouldUpdate)
	{
		linfo << "User declined update";
		return false;
	}

	u64 tid = 0x0; Result res = 0;
	if(R_FAILED(res = APT_GetProgramID(&tid)))
	{
		lerror << "APT_GetProgramID(...): " << std::hex << res;
		return false;
	}

	lverbose << "Updating from url: " << UP_CIA(nver, DEVID) << ",tid=" << tid_to_str(tid);
	process_uri(UP_CIA(nver, DEVID), true, tid_to_str(tid));
	return true;
}

static size_t curl_write_std_string(char *ptr, size_t size, size_t nmemb, void *userdata)
{
	std::string *str = (std::string *) userdata;
	(*str) += std::string(ptr, size * nmemb);
	return size * nmemb;
}

// https://stackoverflow.com/questions/1798112/removing-leading-and-trailing-spaces-from-a-string#1798170
std::string trim(const std::string& str, const std::string& whitespace)
{
	const size_t strBegin = str.find_first_not_of(whitespace);
	if(strBegin == std::string::npos)
		return "";

	const size_t strEnd = str.find_last_not_of(whitespace);
	const size_t strRange = strEnd - strBegin + 1;

	return str.substr(strBegin, strRange);
}

std::string get_url_content(std::string from)
{
	CURL *curl = curl_easy_init();
	std::string ret;

	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write_std_string);
	curl_easy_setopt(curl, CURLOPT_URL, from.c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ret);

	curl_easy_perform(curl);
	curl_easy_cleanup(curl);

	return trim(ret);
}

std::string get_latest_version_string()
{
	return get_url_content(UP_VERSION);
}

