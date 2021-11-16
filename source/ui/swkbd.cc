
#include <ui/swkbd.hh>
#include <panic.hh>


// AppletSwkbd

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


void ui::next::AppletSwkbd::setup(std::string *ret, int maxLen, SwkbdType type,
	int numBtns)
{
	swkbdInit(&this->state, type, numBtns, maxLen);
	this->len = maxLen;
	this->ret = ret;
}

float ui::next::AppletSwkbd::width()
{ return 0.0f; } /* fullscreen */

float ui::next::AppletSwkbd::height()
{ return 0.0f; } /* fullscreen */

void ui::next::AppletSwkbd::hint(const std::string& h)
{ swkbdSetHintText(&this->state, h.c_str()); }

void ui::next::AppletSwkbd::passmode(SwkbdPasswordMode mode)
{ swkbdSetPasswordMode(&this->state, mode); }

void ui::next::AppletSwkbd::init_text(const std::string& t)
{ swkbdSetInitialText(&this->state, t.c_str()); }

void ui::next::AppletSwkbd::valid(SwkbdValidInput mode, u32 filterFlags, u32 maxDigits)
{ swkbdSetValidation(&this->state, mode, filterFlags, maxDigits); }

bool ui::next::AppletSwkbd::render(const ui::Keys& keys)
{
	((void) keys);

	/* why is this cast necessairy? */
	ui::RenderQueue::global()->render_and_then((std::function<bool()>) [this]() -> bool {
		char *buf = new char[this->len + 1];
		SwkbdButton btn = swkbdInputText(&this->state, buf, this->len + 1);
		*this->ret = buf;
		delete [] buf;

		if(this->resPtr != nullptr) *this->resPtr = swkbdGetResult(&this->state);
		if(this->buttonPtr != nullptr) *this->buttonPtr = btn;

		return false;
	});

	return true;
}

/* connect */

void ui::next::AppletSwkbd::connect(ui::next::AppletSwkbd::connect_type t, SwkbdButton *b)
{
	panic_assert(t == ui::next::AppletSwkbd::button, "EINVAL");
	this->buttonPtr = b;
}

void ui::next::AppletSwkbd::connect(ui::next::AppletSwkbd::connect_type t, SwkbdResult *r)
{
	panic_assert(t == ui::next::AppletSwkbd::result, "EINVAL");
	this->resPtr = r;
}

/* keyboard */

std::string ui::keyboard(std::function<void(ui::next::AppletSwkbd *)> configure,
	SwkbdButton *btn, SwkbdResult *res)
{
	ui::RenderQueue queue;
	std::string ret;

	ui::next::AppletSwkbd *swkbd;
	ui::builder<ui::next::AppletSwkbd>(ui::Screen::top, &ret)
		.connect(ui::next::AppletSwkbd::button, btn)
		.connect(ui::next::AppletSwkbd::result, res)
		.add_to(&swkbd, queue);
	configure(swkbd);

	queue.render_finite();
	return ret;
}

/* numpad */

uint64_t ui::numpad(std::function<void(ui::next::AppletSwkbd *)> configure,
	size_t length, SwkbdButton *btn, SwkbdResult *res)
{
	ui::RenderQueue queue;
	std::string ret;

	ui::next::AppletSwkbd *swkbd;
	ui::builder<ui::next::AppletSwkbd>(ui::Screen::top, &ret, length, SWKBD_TYPE_NUMPAD)
		.connect(ui::next::AppletSwkbd::button, btn)
		.connect(ui::next::AppletSwkbd::result, res)
		.add_to(&swkbd, queue);
	configure(swkbd);

	queue.render_finite();
	return strtoull(ret.c_str(), nullptr, 10);
}

