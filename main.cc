#include "window.h"
#include <gtkmm/application.h>

int main(int argc, char *argv[])
{
    auto app = Gtk::Application::create(argc, argv,
            "com.github.mschwan.glareaexample");
    M::Window window;

    return app->run(window);
}
