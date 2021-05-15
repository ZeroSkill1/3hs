
#define JSON_NOEXCEPTION
#include <3rd/json.hh>
#include <curl/curl.h>
#include <malloc.h>
#include <3ds.h>

#include <iostream>
#include <hs.hh>

#include <3rd/log.hh>

#define JS_SET_NULLABLE_PROP(j,s,pj,ps) \
	JT_SET_NULLABLE_PROP(j,s,pj,ps,std::string)
#define JT_SET_NULLABLE_PROP(j,s,pj,ps,T) \
	if(!j[ pj ].is_null()) s. ps = j[ pj ].get< T >()
#define JT_SET_PROP(j,s,pj,ps,T) \
	s. ps = j[ pj ].get< T >()
#define JS_SET_PROP(j,s,pj,ps) \
	JT_SET_PROP(j,s,pj,ps,std::string)

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

template <typename J = json>
static J j_abs_req(std::string url, std::string *err = nullptr)
{
	const std::string body = hs::base_req(url, err);
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
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
	curl_easy_setopt(curl, CURLOPT_CAINFO, HS_CA_LOC);
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &body);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);

	CURLcode res = curl_easy_perform(curl);
	curl_easy_cleanup(curl);

	long code = 0; curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &code); lverbose << "Status: " << code;

	if(res != CURLE_OK && err != nullptr)
	{
		(*err) = std::string("(network-") + std::to_string(res) + "): " + curl_easy_strerror(res);
		lerror << "libcurl error(" << res << "): " << curl_easy_strerror(res);
	}

	lverbose << url << " says " << body;
	return body;
}

std::string hs::route(std::string path)
{
	return std::string(HS_BASE_LOC) + path;
}


/**
 * IF this fails `ret.size` will be `HS_BAD_SIZE` and
 * `ret.updated` will be the error string (use the macros
 * index_failed(...) and index_error(...)) */
hs::Index hs::Index::get()
{
	std::string err;
	ojson resp = j_req<ojson>("index", &err);
	Index ret;

	if(err != "")
	{
		ret.size = HS_BAD_SIZE;
		ret.updated = err;
		return ret;
	}

	ret.totalTitles = resp["total_title_count"].get<hs::count_t>();
	ret.updated = resp["updated_date"].get<std::string>();
	ret.size = resp["total_size"].get<hs::size_t>();

	ojson& cats = resp["entries"];
	for(ojson::iterator jcat = cats.begin(); jcat != cats.end(); ++jcat)
	{
		ojson& vcat = jcat.value();
		hs::Category cat;

		cat.displayName = vcat["display_name"].get<std::string>();
		cat.desc = vcat["description"].get<std::string>();
		cat.name = jcat.key();

		cat.totalTitles = vcat["content_count"].get<hs::count_t>();
		cat.size = vcat["size"].get<hs::size_t>();

		ojson& scats = vcat["subcategories"];

		for(ojson::iterator jscat = scats.begin(); jscat != scats.end(); ++jscat)
		{
			ojson& vscat = jscat.value();
			hs::Subcategory scat;

			scat.displayName = vscat["display_name"].get<std::string>();
			scat.desc = vscat["description"].get<std::string>();
			scat.name = jscat.key();
			scat.cat = cat.name;

			scat.totalTitles = vscat["content_count"].get<hs::count_t>();
			scat.size = vscat["size"].get<hs::size_t>();

			cat.subcategories.push_back(scat);
		}

		ret.categories.push_back(cat);
	}

	return ret;
}

std::vector<hs::Title> hs::search(std::string query)
{
	CURL *curl = curl_easy_init();
	char *qch = curl_easy_escape(curl, query.c_str(), query.size());
	query = std::string(qch);
	curl_easy_cleanup(curl);
	curl_free(qch);

	json res = j_req<json>("title/search?query=" + query);

	std::vector<hs::Title> ret;
	for(json::iterator it = res.begin(); it != res.end(); ++it)
	{
		json& curr = it.value();
		hs::Title pushable;

		pushable.size = curr["size"].get<hs::size_t>();
		pushable.id = curr["id"].get<hs::id_t>();

		pushable.subcat = curr["subcategory"].get<std::string>();
		pushable.tid = curr["title_id"].get<std::string>();
		pushable.cat = curr["category"].get<std::string>();
		pushable.name = curr["name"].get<std::string>();

		ret.push_back(pushable);
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

		pushable.size = curr["size"].get<hs::size_t>();
		pushable.id = curr["id"].get<hs::id_t>();

		pushable.subcat = curr["subcategory"].get<std::string>();
		pushable.tid = curr["title_id"].get<std::string>();
		pushable.cat = curr["category"].get<std::string>();
		pushable.name = curr["name"].get<std::string>();

		ret.push_back(pushable);
	}
	return ret;
}

hs::FullTitle hs::title_meta(hs::id_t id)
{
	json res = j_req<json>(std::string("title/") + std::to_string(id));
	hs::FullTitle ret;

	JS_SET_NULLABLE_PROP(res, ret, "description", desc);

	JS_SET_PROP(res, ret, "version_string", vstring);
	JS_SET_PROP(res, ret, "updated_date", updated);
	JS_SET_PROP(res, ret, "subcategory", subcat);
	JS_SET_PROP(res, ret, "product_code", prod);
	JS_SET_PROP(res, ret, "added_date", added);
	JS_SET_PROP(res, ret, "title_id", tid);
	JS_SET_PROP(res, ret, "category", cat);
	JS_SET_PROP(res, ret, "name", name);

	JT_SET_PROP(res, ret, "version", version, hs::iver_t);
	JT_SET_PROP(res, ret, "size", size, hs::size_t);
	JT_SET_PROP(res, ret, "id", id, hs::id_t);

	return ret;
}

std::string hs::get_token(hs::Title *title)
{
	return j_abs_req<json>(std::string(HS_CDN_BASE_API "content/request?id=")
		+ std::to_string(title->id))["token"].get<std::string>();
}

std::string hs::get_download_link(hs::Title *title)
{
	return std::string(HS_CDN_BASE "content/") + std::to_string(title->id)
		+ "?token=" + hs::get_token(title);
}

#define SOC_ALIGN       0x100000
#define SOC_BUFFERSIZE  0x10000

static u32 *g_socbuf;


void hs::global_deinit()
{
	curl_global_cleanup();
	socExit();
	// g_socbuf shouldn't be NULL
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

