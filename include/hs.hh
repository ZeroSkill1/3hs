
#ifndef inc_hs_hh
#define inc_hs_hh

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
	typedef uint64_t hsize;
	typedef uint64_t hcount;
	typedef uint32_t hiver;
	typedef uint64_t hid;

	typedef struct BaseCategory
	{
		std::string displayName; /* Display */
		std::string name; /* Internal */
		std::string desc;

		hcount totalTitles;
		hsize size;
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

		hcount totalTitles;
		hsize size;

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

		hsize size; /* Filesize */
		hid id;
	} Title;

	typedef struct FullTitle : public Title
	{
		FullTitle() : Title() { };

		std::string vstring; /* Readable version string */
		std::string desc; /* "" if none */
		std::string prod; /* prod code */

		std::string updated; /* unix timestamp (as string) */
		std::string added; /* unix timestamp (as string) */

		hiver version;

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
	std::string get_title_download(hid id);
	FullTitle title_meta(hid id);

	void sort_subcategory(std::vector<hs::Title>& vec);
	void sort_category(std::vector<hs::Title>& vec);
}

#endif
