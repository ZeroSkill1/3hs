
#include "ui/swkbd.hh"


ui::AppletSwkbd::AppletSwkbd(std::string *ret, int maxLen, SwkbdType type, int numBtns)
	: Widget("applet_swkbd"), sret(ret), len(maxLen)
{
	swkbdInit(&this->state, type, numBtns, maxLen);
}

void ui::AppletSwkbd::passmode(SwkbdPasswordMode mode)
{ swkbdSetPasswordMode(&this->state, mode); }

void ui::AppletSwkbd::init_text(std::string txt)
{ swkbdSetInitialText(&this->state, txt.c_str()); }

void ui::AppletSwkbd::hint(std::string txt)
{ swkbdSetHintText(&this->state, txt.c_str()); }

void ui::AppletSwkbd::validinput(SwkbdValidInput valid, u32 filterFlags, u32 maxDigits)
{ swkbdSetValidation(&this->state, valid, filterFlags, maxDigits); }

SwkbdResult ui::AppletSwkbd::get_result()
{ return swkbdGetResult(&this->state); }

SwkbdButton ui::AppletSwkbd::get_button()
{ return this->button; }


ui::Results ui::AppletSwkbd::draw(ui::Keys&, ui::Scr)
{
	ui::end_frame();

	char *buf = new char[this->len];
	this->button = swkbdInputText(&this->state, buf, this->len);
	*this->sret = buf;
	delete [] buf;

	return ui::Results::quit_no_end;
}

std::string ui::AppletSwkbd::read(std::function<void(AppletSwkbd *)> setup,
	SwkbdResult *result, SwkbdButton *button, int maxlen, SwkbdType type, int numBtns)
{
	std::string ret;
	AppletSwkbd *keyboard = new AppletSwkbd(&ret, maxlen, type, numBtns);
	setup(keyboard);

	ui::Widgets wids;
	wids.push_back(keyboard);
	single_draw(wids);

	if(result != nullptr) *result = keyboard->get_result();
	if(button != nullptr) *button = keyboard->get_button();

	return ret;
}

