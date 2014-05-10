NOTE
====
This project is pretty bad, I want to write a new one that makes more sense (and mayeb uses vim like keybindings)
README
======

So! GameBridge(That's a crappy name, ain't it?) is for mapping a gamepad to a keyboard so you can use the gamepad in a game that doesn't support your gamepad... for *FREE*!

Config Layout
=============

Config files are laid out as below:

    controller#
    |
    |-button# -> (OSX keycode)
    | |-right_mouse -> boolean, true to click the right mouse 
    | |-left_mouse -> boolean, true to click the left mouse
    | |-quit -> if I should exit the program
    |-axis#
    | |-mouse_x -> boolean, whether or not to move on the X axis
    | |-mouse_y -> boolean, whether or not to move on the Y axis
    | |-threshold -> at what percent should the axis start registering events
    | |-sensitivity-> how sensitive the axis will be



config files can have more than one controller, if you wish. A sample configuration file, which is set up for a Sony DualShock 3 on Amnesia is provided

Later I will add support for which joystick will control the mouse.
