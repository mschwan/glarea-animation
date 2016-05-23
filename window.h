#ifndef WINDOW_H
#define WINDOW_H

#include "glarea.h"
#include <iostream>
#include <gtkmm/box.h>
#include <gtkmm/window.h>

namespace M
{

class Window : public Gtk::Window
{
public:
    Window();
    virtual ~Window();

protected:
    Gtk::Box _box;
    M::GLArea ngl;
};

} // namespace M

#endif // WINDOW_H
