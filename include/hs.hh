
#ifndef __hs_hh__
#define __hs_hh__

#include <string>
#include <vector>

#define index_failed(i) (i.size == HS_BAD_SIZE)
#define index_error(i) (i.updated)

#define HS_CDN_BASE_API "https://download4.erista.me/" // This one has https :/
#define HS_CDN_BASE     "http://download4.erista.me/"
#define HS_BASE_LOC     "https://hshop.erista.me/api/"
#define HS_CA_LOC       "romfs:/cacert.pem"
#define HS_BAD_SIZE     0


namespace hs
{
	typedef unsigned long long size_t;
	typedef unsigned long count_t;
	typedef unsigned int iver_t;
	typedef unsigned long id_t;

	typedef struct BaseCategory
	{
		std::string displayName; /* Display */
		std::string name; /* Internal */
		std::string desc;

		count_t totalTitles;
		size_t size;
	} BaseCategory;

	typedef struct Subcategory : BaseCategory
	{
		Subcategory() : BaseCategory() { }
		std::string cat;
	} Subcategory;

	typedef struct Category : public BaseCategory
	{
		Category() : BaseCategory() { }
		std::vector<Subcategory> subcategories;

		Subcategory *operator [] (std::string name)
		{
			for(size_t i = 0; i < this->subcategories.size(); ++i)
			{
				if(this->subcategories[i].name == name || this->subcategories[i].displayName == name)
					return &this->subcategories[i];
			}
			return nullptr;
		}
	} Category;

	typedef struct Index
	{
		static Index get();
		std::vector<Category> categories;
		std::string updated;

		count_t totalTitles;
		size_t size;

		Category *operator [] (std::string name)
		{
			for(size_t i = 0; i < this->categories.size(); ++i)
			{
				if(this->categories[i].name == name || this->categories[i].displayName == name)
					return &this->categories[i];
			}
			return nullptr;
		}
	} Index;

	typedef struct Title
	{
		std::string subcat;
		std::string name;
		std::string tid;
		std::string cat;

		size_t size; /* Filesize */
		id_t id;
	} Title;

	typedef struct FullTitle : public Title
	{
		FullTitle() : Title() { };

		std::string vstring; /* Readable version string */
		std::string desc; /* "" if none */
		std::string prod; /* prod code */

		std::string updated; /* unix timestamp (as string) */
		std::string added; /* unix timestamp (as string) */

		iver_t version;

		// We could probably add download count, don't feel like it
	} FullTitle;


	void global_deinit();
	bool global_init();

	std::string base_req(std::string url, std::string *err = nullptr);
	std::string get_download_link(hs::Title *title);
	std::string get_token(hs::Title *title);
	std::string route(std::string path);

	std::vector<Title> titles_in(std::string cat, std::string subcat);
	std::vector<hs::Title> search(std::string query);
	std::string get_title_download(id_t id);
	FullTitle title_meta(id_t id);
}

#endif
