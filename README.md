# glarea-animation
Example of Gtk::GLArea widget with a moving triangle in it.

## Building
To build this application just run
```
make
```
in the main directory.

## Usage
You can run this example with:
```
./virtual_camera
```
The triangle can be moved around with the `W`, `A`, `S`, `D` keys.

## Issues
Currently the movement is not smooth. Sometimes, like randomly every second or so, it moves back for about one frame and then continues in the correct direction. This is especially noticable when moving the triangle linearly in one direction.
