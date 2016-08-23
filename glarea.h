#ifndef MYGLAREA_H
#define MYGLAREA_H

#include "log.h"
#include <iostream>
#include <string>
#include <giomm/resource.h>
#include <gtkmm/box.h>
#include <gtkmm/glarea.h>
#include <epoxy/gl.h>

#include <gdkmm/seat.h>
#include <gdkmm/display.h>
#include <gdkmm/event.h>

namespace M
{

// We derive from Gtk::Box because deriving from Gtk::GLArea results in weird
// errors (g_value_take_object: assertion 'G_IS_OBJECT (v_object)' failed) and
// add a Gtk::GLArea manually.
// see https://mail.gnome.org/archives/gtkmm-list/2016-April/msg00023.html
// TODO: Maybe we can change this now? And derive from GLArea directly?
//       ... waiting for gtkmm 3.20.2 or 3.20.3
class GLArea : public Gtk::Box
{
    public:
        GLArea();
        virtual ~GLArea();

        int get_button_event_x() const;
        void set_button_event_x(const int &x);
        int get_button_event_y() const;
        void set_button_event_y(const int &y);

    protected:
        bool on_motion_notify_event(GdkEventMotion *event);
        bool on_button_press_event(GdkEventButton *event);
        bool on_button_release_event(GdkEventButton *event);
        bool on_key_press_event(GdkEventKey *event);
        bool on_key_release_event(GdkEventKey *event);

        void realize();
        void unrealize();
        void init_data();
        void delete_buffers_and_data();
        void init_buffers();
        void init_shaders();
        GLuint create_shader(GLenum shaderType, const std::string &source);
        bool render(const Glib::RefPtr<Gdk::GLContext> &context);
        void draw();
        static gboolean frame_callback(GtkWidget *widget, GdkFrameClock *frame_clock, gpointer data);

    private:
        Gtk::GLArea _glArea;

        GLfloat *_points;
        GLfloat *_colors;
        GLfloat *_matrix;

        GLuint _vao;
        GLuint _vboPoints;
        GLuint _vboColors;

        GLuint _shaderProgram;
        GLint _matrixLocation;

        float _previousX;
        float _previousY;

        bool _isButton3Pressed;
        bool _isKeyWPressed;
        bool _isKeyAPressed;
        bool _isKeySPressed;
        bool _isKeyDPressed;

        int _px;
        int _py;
};

} // namespace M

#endif // MYGLAREA_H
