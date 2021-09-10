
#include "find_missing.hh"
#include "install.hh"
#include "titles.hh"
#include "queue.hh"
#include "panic.hh"
#include "util.hh"
#include "hs.hh"

#include <ui/loading.hh>
#include <ui/core.hh>

#include <algorithm>


template <typename T>
static void vecappend(std::vector<T>& a, const std::vector<T>& b)
{ a.insert(a.end(), b.begin(), b.end()); }

void show_find_missing()
{
	toggle_focus();

	ui::loading([]() -> void {

		std::vector<u64> installed;
		panic_if_err_3ds(list_titles_on(MEDIATYPE_SD, installed));
		list_titles_on(MEDIATYPE_GAME_CARD, installed); // it might error if there is no cart inserted so we don't want to panic if it fails

		std::vector<u64> installedGames;
		std::copy_if(installed.begin(), installed.end(), std::back_inserter(installedGames), [](u64 tid) -> bool {
			u16 category = (tid >> 32) & 0xFFFF;
			return category == 0x0000 /* normal */ || category == 0x8000 /* DSiWare/TWL */;
		});

		hs::BatchRelated related = hs::batch_related(installedGames);
		std::vector<hs::Title> potentialInstalls;

		for(size_t i = 0; i < installedGames.size(); ++i)
		{
			vecappend(potentialInstalls, related[installedGames[i]].updates);
			vecappend(potentialInstalls, related[installedGames[i]].dlc);
		}

		std::vector<hs::Title> newInstalls;
		std::copy_if(potentialInstalls.begin(), potentialInstalls.end(), std::back_inserter(newInstalls), [installed](const hs::Title& title) -> bool {
			// TODO: also check the version int, wait for backend update to return version int for that
			return std::find(installed.begin(), installed.end(), str_to_tid(title.tid)) != installed.end();
		});

		for(const hs::Title& title : newInstalls)
			queue_add(title.id);

	});

	toggle_focus();
}

