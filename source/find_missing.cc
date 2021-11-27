
#include "find_missing.hh"
#include "install.hh"
#include "hsapi.hh"
#include "queue.hh"
#include "panic.hh"
#include "ctr.hh"

#include <ui/loading.hh>

#include <algorithm>


template <typename T>
static void vecappend(std::vector<T>& a, const std::vector<T>& b)
{ a.insert(a.end(), b.begin(), b.end()); }

bool tid_can_have_missing(hsapi::htid tid)
{
	u16 category = (tid >> 32) & 0xFFFF;
	return category == 0x0000 /* normal */ || category == 0x8000 /* DSiWare/TWL */;
}

void show_find_missing(hsapi::htid tid)
{
	ui::loading([tid]() -> void {

		std::vector<hsapi::htid> installed;
		panic_if_err_3ds(ctr::list_titles_on(MEDIATYPE_SD, installed));
		ctr::list_titles_on(MEDIATYPE_GAME_CARD, installed); // it might error if there is no cart inserted so we don't want to panic if it fails

		std::vector<hsapi::htid> doCheckOn;
		if(tid == 0) doCheckOn = installed;
		else doCheckOn.push_back(tid);

		std::vector<hsapi::htid> installedGames;
		std::copy_if(doCheckOn.begin(), doCheckOn.end(), std::back_inserter(installedGames), tid_can_have_missing);

		hsapi::BatchRelated related;
		if(R_FAILED(hsapi::batch_related(related, installedGames)))
			return;

		std::vector<hsapi::Title> potentialInstalls;

		for(size_t i = 0; i < installedGames.size(); ++i)
		{
			vecappend(potentialInstalls, related[installedGames[i]].updates);
			vecappend(potentialInstalls, related[installedGames[i]].dlc);
		}

		std::vector<hsapi::Title> newInstalls;
		std::copy_if(potentialInstalls.begin(), potentialInstalls.end(), std::back_inserter(newInstalls), [installed](const hsapi::Title& title) -> bool {
			// TODO: also check the version int, wait for backend update to return version int for that
			// Is the title already installed?
			return std::find(installed.begin(), installed.end(), title.tid) == installed.end()
				// Is the title already in the queue?
				&& std::find(queue_get().begin(), queue_get().end(), title) == queue_get().end();
		});

		for(const hsapi::Title& title : newInstalls)
			queue_add(title.id, false);

	});
}
