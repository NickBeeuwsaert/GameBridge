README
======

So! GameBridge(That's a crappy name, ain't it?) is for mapping a gamepad to a keyboard so you can use the gamepad in a game that doesn't support you gamepad... for *FREE*!

Config Layout
=============

Config files are laid out as below:

    controller#
    |
    |-buttons
    | |-button# -> (OSX keycode)
    | |-right_mouse -> (boolean, can be true or false, it's not checked)
    | |-left_mouse -> (boolean, can be true or false, it's not checked)
    |-sensitivity -> integer, how sensitive the joystick will be



config files can have more than one controller, if you wish. A sample configuration file, which is set up for a Sony DualShock 3 on Amnesia is provided

Later I will add support for which joystick will control the mouse.