
#ifndef __hs_hh__
#define __hs_hh__

#include <exception>
#include <string>
#include <vector>


#define __HS_SIZE_T unsigned long long
#define __HS_TITLES_T unsigned long
#define __HS_IVER_T unsigned int
#define __HS_ID_T unsigned long

#define index_failed(i) (i.size == hs::constants::BAD_SIZE)
#define index_error(i) (i.updated)


namespace hs
{
	namespace constants
	{
		constexpr char BASE_LOC[] = "https://hshop.erista.me/api/";
		constexpr char CA_LOC[] = "romfs:/cacert.pem";
		constexpr int BAD_SIZE = 0;
	}

	typedef struct BaseCategory
	{
		std::string displayName;
		std::string name;
		std::string desc;

		__HS_TITLES_T totalTitles;
		__HS_SIZE_T size;
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

		__HS_TITLES_T totalTitles;
		__HS_SIZE_T size;

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

		__HS_SIZE_T size; /* Filesize */
		__HS_ID_T id;
	} Title;

	typedef struct FullTitle : public Title
	{
		FullTitle() : Title() { };

		std::string vstring; /* Readable version string */
		std::string desc; /* "" if none */
		std::string prod; /* prod code */

		std::string updated; /* unix timestamp (as string) */
		std::string added; /* unix timestamp (as string) */

		__HS_IVER_T version;
	} FullTitle;


	void global_deinit();
	bool global_init();

	std::string base_req(std::string url, std::string *err = nullptr);
	std::string route(std::string path);

	std::vector<Title> titles_in(std::string cat, std::string subcat);
	std::string get_title_download(__HS_ID_T id);
	Title title_meta(__HS_ID_T id);

	typedef __HS_ID_T id_t;
}

#endif
