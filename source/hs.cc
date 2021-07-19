
#define JSON_NOEXCEPTION
#include <3rd/json.hh>
#include <curl/curl.h>
#include <malloc.h>
#include <3ds.h>

#include <algorithm>
#include <iostream>
#include <hs.hh>

#include <3rd/log.hh>

#include <widgets/meta.hh> // TODO: Move index cache to this file

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
	if(body == "") return J();
	return J::parse(body);
}

template <typename J = json>
static J j_abs_req(std::string url, std::string *err = nullptr)
{
	const std::string body = hs::base_req(url, err);
	if(body == "") return J();
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
		lerror << "libcurl error(" << res << "): " << curl_easy_strerror(res);
		(*err) = curl_easy_strerror(res);
		if(res == 60) // stupid error
			(*err) += "\nTry setting time & date correctly.";
		return "";
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

	ret.totalTitles = resp["total_title_count"].get<hs::hcount>();
	ret.updated = resp["updated_date"].get<std::string>();
	ret.size = resp["total_size"].get<hs::hsize>();

	ojson& cats = resp["entries"];
	for(ojson::iterator jcat = cats.begin(); jcat != cats.end(); ++jcat)
	{
		ojson& vcat = jcat.value();
		hs::Category cat;

		cat.displayName = vcat["display_name"].get<std::string>();
		cat.desc = vcat["description"].get<std::string>();
		cat.name = jcat.key();

		cat.totalTitles = vcat["content_count"].get<hs::hcount>();
		cat.size = vcat["size"].get<hs::hsize>();

		ojson& scats = vcat["subcategories"];

		for(ojson::iterator jscat = scats.begin(); jscat != scats.end(); ++jscat)
		{
			ojson& vscat = jscat.value();
			hs::Subcategory scat;

			scat.displayName = vscat["display_name"].get<std::string>();
			scat.desc = vscat["description"].get<std::string>();
			scat.name = jscat.key();
			scat.cat = cat.name;

			scat.totalTitles = vscat["content_count"].get<hs::hcount>();
			scat.size = vscat["size"].get<hs::hsize>();

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

		pushable.size = curr["size"].get<hs::hsize>();
		pushable.id = curr["id"].get<hs::hid>();

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

		pushable.size = curr["size"].get<hs::hsize>();
		pushable.id = curr["id"].get<hs::hid>();

		pushable.subcat = curr["subcategory"].get<std::string>();
		pushable.tid = curr["title_id"].get<std::string>();
		pushable.cat = curr["category"].get<std::string>();
		pushable.name = curr["name"].get<std::string>();

		ret.push_back(pushable);
	}
	return ret;
}

hs::FullTitle hs::title_meta(hs::hid id)
{
	json res = j_req<json>(std::string("title/") + std::to_string(id));
	hs::FullTitle ret;

	JS_SET_NULLABLE_PROP(res, ret, "description", desc);

	JS_SET_PROP(res, ret, "version_string", vstring);
//	JS_SET_PROP(res, ret, "updated_date", updated);
	JS_SET_PROP(res, ret, "subcategory", subcat);
	JS_SET_PROP(res, ret, "product_code", prod);
//	JS_SET_PROP(res, ret, "added_date", added);
	JS_SET_PROP(res, ret, "title_id", tid);
	JS_SET_PROP(res, ret, "category", cat);
	JS_SET_PROP(res, ret, "name", name);

	JT_SET_PROP(res, ret, "version", version, hs::hiver);
	JT_SET_PROP(res, ret, "size", size, hs::hsize);
	JT_SET_PROP(res, ret, "id", id, hs::hid);

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

// utils

std::string hs::parse_vstring(hs::hiver version)
{
	// based on:
	//  "{(ver >> 10) & 0x3F}.{(ver >> 4) & 0x3F}.{ver & 0xF}"
	return "v"
		+ std::to_string(version >> 10 & 0x3F) + "."
		+ std::to_string(version >> 4  & 0x3F) + "."
		+ std::to_string(version       & 0xF );
}

// sort

void hs::sort_category(std::vector<hs::Title>& vec)
{
	std::map<std::string, size_t> catmap; size_t idx = 0;
	for(hs::Category& cat : ui::get_index()->categories)
		catmap[cat.displayName] = idx++;
	std::sort(vec.begin(), vec.end(), [&catmap](const hs::Title& a, const hs::Title& b) -> bool {
		return catmap[a.cat] < catmap[b.cat];
	});
}

void hs::sort_subcategory(std::vector<hs::Title>& vec)
{
	std::map<std::string, size_t> catmap; size_t idx = 0;
	for(hs::Category& cat : ui::get_index()->categories)
	{
		for(hs::Subcategory& scat : cat.subcategories)
		{
			if(catmap.count(scat.displayName) == 0)
				catmap[scat.displayName] = idx++;
		}
	}

	std::sort(vec.begin(), vec.end(), [&catmap](const hs::Title& a, const hs::Title& b) -> bool {
		return catmap[a.subcat] < catmap[b.subcat];
	});
}


#define SOC_ALIGN       0x100000
#define SOC_BUFFERSIZE  0x20000

static u32 *g_socbuf = NULL;


void hs::global_deinit()
{
	curl_global_cleanup();
	socExit();

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


