
#ifndef inc_hsapi_hh
#define inc_hsapi_hh

#include <unordered_map>
#include <string>
#include <vector>

#include <3ds.h>

#include <ui/confirm.hh>
#include <ui/loading.hh>
#include <ui/core.hh>

#include "panic.hh"
#include "error.hh"
#include "i18n.hh"


namespace hsapi
{
	using htimestamp = uint64_t; /* unix timestamp type. unsigned because there will be no negative timestamps on hshop */
	using hsize      = uint64_t; /* size type */
	using hiver      = uint16_t; /* integer version type */
	using htid       = uint64_t; /* title id type */
	using hid        = int64_t;  /* landing id type */

	namespace impl
	{
		typedef struct BaseCategory
		{
			std::string disp; /* display name; use this */
			std::string name; /* internal short name */
			std::string desc; /* category description */
			hsize titles; /* title count */
			hsize size; /* total size */
		} BaseCategory;
	}

	typedef struct Subcategory : public impl::BaseCategory
	{
		std::string cat; /* parent category */
	} Subcategory;

	typedef struct Category : public impl::BaseCategory
	{
		std::vector<Subcategory> subcategories; /* subcategories in this category */

		/* find a subcategory by name */
		Subcategory *find(const std::string& name);
	} Category;

	typedef struct Index
	{
		std::vector<Category> categories; /* categories on hShop */
		std::string updated; /* last updated date */
		hsize titles; /* total titles on hShop */
		hsize size; /* total size of hShop */

		/* find a category by name */
		Category *find(const std::string& name);
	} Index;

	typedef struct Title
	{
		std::string subcat; /* subcategory this title belongs to */
		std::string name; /* name of the title on hShop */
		std::string cat; /* category this title belongs to */
		hsize size; /* filesize */
		htid tid; /* title id of the title */
		hid id; /* hShop id */
	} Title;

	typedef struct FullTitle : public Title
	{
		std::string prod; /* product code */
		std::string desc; /* "" if none */
//		htimestamp updated; /* last updated timestamp */
//		htimestamp added; /* added on timestamp */
		hiver version; /* version int */
	} FullTitle;

	typedef struct Related
	{
		std::vector<Title> updates;
		std::vector<Title> dlc;
	} Related;
	using BatchRelated = std::unordered_map<htid, Related>;


	void global_deinit();
	bool global_init();

	Result titles_in(std::vector<Title>& ret, const std::string& cat, const std::string& scat);
	Result batch_related(BatchRelated& ret, const std::vector<htid>& tids);
	Result search(std::vector<Title>& ret, const std::string& query);
	Result get_download_link(std::string& ret, const Title& title);
	Result get_latest_version_string(std::string& ret);
	Result title_meta(FullTitle& ret, hid id);
	Result fetch_index();

	std::string update_location(const std::string& ver);
	std::string parse_vstring(hiver ver);
	Index *get_index();

	// NOTE: You have to std::move() primitives (hid, hiver, htid, ...)
	template <typename ... Ts>
	Result call(Result (*func)(Ts...), Ts&& ... args)
	{
		Result res = 0;
		do {
			ui::loading([&res, func, &args...]() -> void {
				res = (*func)(args...);
			});

			if(R_FAILED(res)) // Ask if we want to retry
			{
				error_container err = get_error(res);
				handle_error(err);

				ui::Widgets wids; bool cont = true;
				wids.push_back(new ui::Confirm(STRING(retry_req), cont), ui::Scr::bottom);
				generic_main_breaking_loop(wids);

				if(!cont) break;
			}
		} while(R_FAILED(res));
		return res;
	}
}

#endif

