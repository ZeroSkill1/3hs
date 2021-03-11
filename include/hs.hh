
#ifndef __hs_hh__
#define __hs_hh__

#include <exception>
#include <string>
#include <vector>


#define __HS_SIZE_T unsigned long long
#define __HS_TITLES_T unsigned long

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

	void global_deinit();
	bool global_init();

	std::string base_req(std::string url, std::string *err = nullptr);
	std::string route(std::string path);


	typedef struct Subcategory
	{
		std::string displayName;
		std::string name;
		std::string desc;

		__HS_TITLES_T totalTitles;
		__HS_SIZE_T size;
	} Subcategory;

	typedef struct Category : public Subcategory
	{
		Category() : Subcategory() { };
		std::vector<Subcategory> subcategories;
	} Category;

	typedef struct Index
	{
		static Index get();
		std::vector<Category> categories;
		std::string updated;

		__HS_TITLES_T totalTitles;
		__HS_SIZE_T size;
	} Index;
}

#endif
