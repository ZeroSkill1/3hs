
#include "update.hh"

#include <curl/curl.h>
#include <3rd/log.hh>

#include <ui/confirm.hh>
#include <ui/core.hh>

#include "install.hh"


bool update_app()
{
	std::string nver = get_latest_version_string();
	linfo << "Fetched new version " << nver << " from " << UP_VERSION;
	linfo << "Current app version is " << VERSION;

	ui::Widgets wids; bool shouldUpdate = false;
	wids.push_back("confirmation", new ui::Confirm(std::string("Update to version ")
		+ nver + "?", shouldUpdate), ui::Scr::bottom);
	generic_main_breaking_loop(wids);

	if(nver == VERSION)
		return false;

	if(!shouldUpdate)
	{
		linfo << "User declined update";
		return false;
	}

	Handle cia; AM_StartCiaInstall(MEDIATYPE_SD, &cia);
	game::start_mutex();

	linfo << "Updating from url: " << UP_CIA(nver) << " ...";
	game::single_install_thread(UP_CIA(nver), cia);
	game::clean_mutex();
	AM_FinishCiaInstall(cia);
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
