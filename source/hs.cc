
#define JSON_NOEXCEPTION
#include <3rd/json.hh>
#include <curl/curl.h>
#include <malloc.h>
#include <3ds.h>

#include <hs.hh>
#include <iostream>

using namespace nlohmann;
typedef ordered_json ojson;


// Putting this here instead of making it public
// Is because nlohmann::json throws a fuckton
// of warnings and we don't like those
template <typename J = json>
static J j_req(std::string path, std::string *err = nullptr)
{
	const std::string body = hs::base_req(hs::route(path), err);
	return J::parse(body);
}

static size_t curl_write_std_string(char *ptr, size_t size, size_t nmemb, void *userdata)
{
	std::string *str = (std::string *) userdata;
	(*str) += std::string(ptr, size * nmemb);
	return size * nmemb;
}

std::string hs::base_req(std::string url, std::string *err)
{
	CURL *curl = curl_easy_init();
	std::string body;

	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write_std_string);
	curl_easy_setopt(curl, CURLOPT_CAINFO, hs::constants::CA_LOC);
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &body);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);

	CURLcode res = curl_easy_perform(curl);
	curl_easy_cleanup(curl);

	if(res != CURLE_OK && err != nullptr)
		(*err) = std::string("(network-") + std::to_string(res) + "): " + curl_easy_strerror(res);
	return body;
}

std::string hs::route(std::string path)
{
	return std::string(constants::BASE_LOC) + path;
}


/**
 * IF this fails `ret.size` will be `hs::constants::BAD_SIZE` and
 * `ret.updated` will be the error string
 **/
hs::Index hs::Index::get()
{
	std::string err;
	ojson resp = j_req<ojson>("index", &err);
	Index ret;

	if(err != "")
	{
		ret.size = constants::BAD_SIZE;
		ret.updated = err;
		return ret;
	}

	ret.totalTitles = resp["total_title_count"].get<__HS_TITLES_T>();
	ret.updated = resp["updated_date"].get<std::string>();
	ret.size = resp["total_size"].get<__HS_SIZE_T>();

	ojson& cats = resp["entries"];
	for(ojson::iterator jcat = cats.begin(); jcat != cats.end(); ++jcat)
	{
		ojson& vcat = jcat.value();
		hs::Category cat;

		cat.displayName = vcat["display_name"].get<std::string>();
		cat.desc = vcat["description"].get<std::string>();
		cat.name = jcat.key();

		cat.totalTitles = vcat["content_count"].get<__HS_TITLES_T>();
		cat.size = vcat["size"].get<__HS_SIZE_T>();

		ojson& scats = vcat["subcategories"];

		for(ojson::iterator jscat = scats.begin(); jscat != scats.end(); ++jscat)
		{
			ojson& vscat = jscat.value();
			hs::Subcategory scat;

			scat.displayName = vscat["display_name"].get<std::string>();
			scat.desc = vscat["description"].get<std::string>();
			scat.name = jscat.key();

			scat.totalTitles = vscat["content_count"].get<__HS_TITLES_T>();
			scat.size = vscat["size"].get<__HS_SIZE_T>();

			cat.subcategories.push_back(scat);
		}

		ret.categories.push_back(cat);
	}

	return ret;
}

std::vector<hs::Title> hs::titles_in(std::string cat, std::string subcat)
{
	std::vector<hs::Title> ret;
	json res = j_req<json>(std::string("category/") + cat + "/" + subcat);
	for(json::iterator it = res.begin(); it != res.end(); ++it)
	{
		json& curr = it.value();
		hs::Title pushable;

		pushable.size = curr["size"].get<__HS_SIZE_T>();
		pushable.id = curr["id"].get<__HS_ID_T>();

		pushable.subcat = curr["subcategory"].get<std::string>();
		pushable.tid = curr["title_id"].get<std::string>();
		pushable.cat = curr["category"].get<std::string>();
		pushable.name = curr["name"].get<std::string>();

		ret.push_back(pushable);
	}
	return ret;
}


#define SOC_ALIGN       0x100000
#define SOC_BUFFERSIZE  0x10000

static u32 *g_socbuf;


void hs::global_deinit()
{
	curl_global_cleanup();
	socExit();
	// This shouldn't happen
	if(g_socbuf != NULL)
		free(g_socbuf);
}

bool hs::global_init()
{
	if((g_socbuf = (u32 *) memalign(SOC_ALIGN, SOC_BUFFERSIZE)) == NULL)
		return false;
	if(R_FAILED(socInit(g_socbuf, SOC_BUFFERSIZE)))
	{
		free(g_socbuf);
		return false;
	}

	curl_global_init(CURL_GLOBAL_ALL);
	return true;
}

