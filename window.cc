#include "window.h"

namespace M
{

Window::Window()
{
    set_border_width(6);
    set_default_size(640, 480);

    _box.set_spacing(6);
    add(_box);

    _box.pack_end(ngl, Gtk::PACK_EXPAND_WIDGET);

    show_all();
}

Window::~Window()
{
}

} // namespace M
