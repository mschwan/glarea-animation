#include "glarea.h"

namespace M
{

GLArea::GLArea()
    : _points(0),
      _colors(0),
      _matrix(0),
      _isButton3Pressed(false),
      _isKeyWPressed(false),
      _isKeyAPressed(false),
      _isKeySPressed(false),
      _isKeyDPressed(false)
{
    set_has_window(false);

    // It is important to explicitely allow events on the GLArea!
    _glArea.add_events(Gdk::POINTER_MOTION_MASK
            | Gdk::BUTTON_PRESS_MASK
            | Gdk::BUTTON_RELEASE_MASK
            | Gdk::KEY_PRESS_MASK
            | Gdk::KEY_RELEASE_MASK);

    pack_end(_glArea, Gtk::PACK_EXPAND_WIDGET);

    _glArea.signal_realize().connect(
            sigc::mem_fun(*this, &GLArea::realize));
    // It is important that the unrealize signal calls our handler to clean up
    // GL resources *before* the default unrealize handler is called, hence the
    // "false".
    _glArea.signal_unrealize().connect(
            sigc::mem_fun(*this, &GLArea::unrealize), false);
    _glArea.signal_render().connect(
            sigc::mem_fun(*this, &GLArea::render));

    _glArea.signal_key_press_event().connect(
            sigc::mem_fun(*this, &GLArea::on_key_press_event), false);
    _glArea.signal_key_release_event().connect(
            sigc::mem_fun(*this, &GLArea::on_key_release_event), false);

    // Grab the focus to get all key events no matter what!
    _glArea.set_can_focus(true);
    _glArea.grab_focus();

    show_all();
}

GLArea::~GLArea()
{
}

bool GLArea::on_motion_notify_event(GdkEventMotion *event)
{
    if(_isButton3Pressed) {
        float dx = event->x - _previousX;
        float dy = event->y - _previousY;

        _matrix[12] += 2 * dx / (float) _glArea.get_allocation().get_width();
        _matrix[13] -= 2 * dy / (float) _glArea.get_allocation().get_height();
    }

    _previousX = event->x;
    _previousY = event->y;
}

bool GLArea::on_button_press_event(GdkEventButton *event)
{
    if(event->button == 3) {
        _isButton3Pressed = true;
    }
}

bool GLArea::on_button_release_event(GdkEventButton *event)
{
    if(event->button == 3) {
        _isButton3Pressed = false;
    }
}

bool GLArea::on_key_press_event(GdkEventKey *event)
{
    if(!_isKeyWPressed && event->keyval == GDK_KEY_w) {
        _isKeyWPressed = true;
    }

    if(!_isKeyAPressed && event->keyval == GDK_KEY_a) {
        _isKeyAPressed = true;
    }

    if(!_isKeySPressed && event->keyval == GDK_KEY_s) {
        _isKeySPressed = true;
    }

    if(!_isKeyDPressed && event->keyval == GDK_KEY_d) {
        _isKeyDPressed = true;
    }
}

bool GLArea::on_key_release_event(GdkEventKey *event)
{
    if(_isKeyWPressed && event->keyval == GDK_KEY_w) {
        _isKeyWPressed = false;
    }

    if(_isKeyAPressed && event->keyval == GDK_KEY_a) {
        _isKeyAPressed = false;
    }

    if(_isKeySPressed && event->keyval == GDK_KEY_s) {
        _isKeySPressed = false;
    }

    if(_isKeyDPressed && event->keyval == GDK_KEY_d) {
        _isKeyDPressed = false;
    }
}

void GLArea::realize()
{
    _glArea.make_current();

    try {
        _glArea.throw_if_error();

        const GLubyte *renderer = glGetString(GL_RENDERER);
        const GLubyte *version = glGetString(GL_VERSION);

        if(renderer && version) {
            std::cout
                << info_string()
                << "Realizing GL context"
                << "\n\t" << renderer
                << "\n\t" << version
                << std::endl;
        }

        init_data();
        init_buffers();
        init_shaders();

        gtk_widget_add_tick_callback(
                (GtkWidget *) this->gobj(),
                GLArea::frame_callback,
                this,
                NULL);
    } catch(const Gdk::GLError &error) {
        std::cerr
            << error_string()
            << "Failed to make GL context during realize"
            << "\n\tDomain: " << error.domain()
            << "\n\tCode: " << error.code()
            << "\n\tWhat: " << error.what()
            << std::endl;
    }
}

void GLArea::unrealize()
{
    _glArea.make_current();

    try {
        _glArea.throw_if_error();

        std::cout
            << info_string()
            << "Unrealizing GL context"
            << std::endl;

        delete_buffers_and_data();
    } catch(const Gdk::GLError &error) {
        std::cerr
            << error_string()
            << "Failed to make GL context during unrealize"
            << "\n\tDomain: " << error.domain()
            << "\n\tCode: " << error.code()
            << "\n\tWhat: " << error.what()
            << std::endl;
    }
}

void GLArea::init_data()
{
    _points = new GLfloat[9]{
        0.0,  0.5, 0.0,
        0.5, -0.5, 0.0,
       -0.5, -0.5, 0.0
    };
    _colors = new GLfloat[9]{
        1.0, 0.0, 0.5,
        0.5, 1.0, 0.0,
        0.0, 0.5, 1.0
    };
    _matrix = new GLfloat[16]{
        1.0, 0.0, 0.0, 0.0,
        0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        0.0, 0.0, 0.0, 1.0
    };
}

void GLArea::delete_buffers_and_data()
{
    glDeleteBuffers(1, &_vboPoints);
    glDeleteBuffers(1, &_vboColors);
    glDeleteProgram(_shaderProgram);

    delete[] _points;
    delete[] _colors;
    delete[] _matrix;
}

void GLArea::init_buffers()
{
    glClearColor(0.2, 0.2, 0.2, 1.0);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);

    glGenBuffers(1, &_vboPoints);
    glBindBuffer(GL_ARRAY_BUFFER, _vboPoints);
    glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), _points, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &_vboColors);
    glBindBuffer(GL_ARRAY_BUFFER, _vboColors);
    glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), _colors, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(1);
}

void GLArea::init_shaders()
{
    // Vertex shader
    auto vertexShaderBytes =
        Gio::Resource::lookup_data_global("/shaders/shader.vert");

    if(!vertexShaderBytes) {
        std::cerr
            << error_string()
            << "Failed fetching vertex shader resource!"
            << std::endl;
        _shaderProgram = 0;
        return;
    }

    gsize vertexShaderSize = vertexShaderBytes->get_size();
    auto vertexShader = create_shader(GL_VERTEX_SHADER,
            (const char *) vertexShaderBytes->get_data(vertexShaderSize));

    if(!vertexShader) {
        _shaderProgram = 0;
        return;
    }

    // Fragment shader
    auto fragmentShaderBytes =
        Gio::Resource::lookup_data_global("/shaders/shader.frag");

    if(!fragmentShaderBytes) {
        std::cerr
            << error_string()
            << "Failed fetching fragment shader resource!"
            << std::endl;
        _shaderProgram = 0;
        return;
    }

    gsize fragmentShaderSize = fragmentShaderBytes->get_size();
    auto fragmentShader = create_shader(GL_FRAGMENT_SHADER,
            (const char *) fragmentShaderBytes->get_data(fragmentShaderSize));

    if(!fragmentShader) {
        _shaderProgram = 0;
        return;
    }

    // Shader program
    _shaderProgram = glCreateProgram();
    glAttachShader(_shaderProgram, vertexShader);
    glAttachShader(_shaderProgram, fragmentShader);
    glLinkProgram(_shaderProgram);

    GLint status;
    glGetProgramiv(_shaderProgram, GL_LINK_STATUS, &status);

    if(status == GL_FALSE) {
        GLint logLength;
        glGetProgramiv(_shaderProgram, GL_INFO_LOG_LENGTH, &logLength);

        std::string logSpace(logLength + 1, ' ');
        glGetProgramInfoLog(_shaderProgram, logLength, nullptr,
                (GLchar *) logSpace.c_str());

        std::cerr
            << error_string()
            << "Failed to link program:"
            << "\n\t" << logSpace
            << std::endl;

        glDeleteProgram(_shaderProgram);
        _shaderProgram = 0;
    } else {
        _matrixLocation = glGetUniformLocation(_shaderProgram, "matrix");
        glUniformMatrix4fv(_matrixLocation, 1, GL_FALSE, _matrix);

        if(_matrixLocation == -1) {
            std::cerr
                << error_string()
                << "Could not find location of matrix!"
                << std::endl;
        }

        glUseProgram(_shaderProgram);

        glDetachShader(_shaderProgram, vertexShader);
        glDetachShader(_shaderProgram, fragmentShader);
    }

    // Mark shaders for deletion, so memory can be freed.
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

GLuint GLArea::create_shader(GLenum shaderType, const std::string &source)
{
    auto shader = glCreateShader(shaderType);

    const GLchar *cSource;
    cSource = source.c_str();
    glShaderSource(shader, 1, &cSource, nullptr);
    glCompileShader(shader);

    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

    if(status == GL_FALSE) {
        GLint logLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);

        std::string logSpace(logLength + 1, ' ');
        glGetShaderInfoLog(shader, logLength, nullptr,
                (GLchar *) logSpace.c_str());

        switch(shaderType) {
            case GL_VERTEX_SHADER:
                std::cerr
                    << error_string()
                    << "Compile failure in vertex shader:\n\t"
                    << logSpace
                    << std::endl;
                break;
            case GL_FRAGMENT_SHADER:
                std::cerr
                    << error_string()
                    << "Compile failure in fragment shader:\n\t"
                    << logSpace
                    << std::endl;
                break;
            default:
                std::cerr
                    << error_string()
                    << "Unhandled exception raised while compiling shader!"
                    << "\n\tThis should not happen. Please report this issue!"
                    << "\n\tThe source file is: glarea.cc"
                    << std::endl;
                break;
        }

        glDeleteShader(shader);

        return 0;
    } else {
        return shader;
    }
}

bool GLArea::render(const Glib::RefPtr<Gdk::GLContext> &)
{
    try {
        _glArea.throw_if_error();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //draw();
        if(_isKeyWPressed) {
            _matrix[13] += 0.01;
        }
        if(_isKeyAPressed) {
            _matrix[12] -= 0.01;
        }
        if(_isKeySPressed) {
            _matrix[13] -= 0.01;
        }
        if(_isKeyDPressed) {
            _matrix[12] += 0.01;
        }

        glUniformMatrix4fv(_matrixLocation, 1, GL_FALSE, _matrix);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        return true;
    } catch(const Gdk::GLError &error) {
        std::cerr
            << error_string()
            << "Something went wrong in the render callback\n\t"
            << "\n\tDomain: " << error.domain()
            << "\n\tCode: " << error.code()
            << "\n\tWhat: " << error.what()
            << std::endl;
        return false;
    }
}

void GLArea::draw()
{
}

gboolean GLArea::frame_callback(GtkWidget *widget, GdkFrameClock *frame_clock, gpointer data)
{
    GLArea *self = (GLArea *) data;
    self->_glArea.queue_draw();

    return G_SOURCE_CONTINUE;
}

} // namespace M
