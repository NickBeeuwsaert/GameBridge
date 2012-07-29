#include <SDL/SDL.h>
#include <stdio.h>
#include <ApplicationServices/ApplicationServices.h>
#include <libconfig.h++>
#include <iostream>
#include <stdarg.h>
#include <vector>
using namespace libconfig;
using namespace std;
std::string format(const std::string &fmt, ...) {
       int size=100;
       std::string str;
       va_list ap;
       while (1) {
       str.resize(size);
       va_start(ap, fmt);
       int n = vsnprintf((char *)str.c_str(), size, fmt.c_str(), ap);
       va_end(ap);
       if (n > -1 && n < size) {
           str.resize(n);
           return str;
       }
       if (n > -1)
           size=n+1;
       else
           size*=2;
       }
}


string button_str = "controller%i.button%i.%s"; // controller index, button index property
int main(int argc, char*argv[]){
	SDL_Init(SDL_INIT_EVERYTHING);
	
	SDL_Event  evt;
	if(SDL_NumJoysticks() == 0){
		printf("No joysticks detected!\n");
		return 0;
	}
	int numJoysticks = SDL_NumJoysticks();
	printf("Detected %d joysticks\n", numJoysticks);
	SDL_Joystick **sticks = new SDL_Joystick*[numJoysticks];
	for(int i = 0; i < SDL_NumJoysticks(); i++){
		 sticks[i] = SDL_JoystickOpen(i);
		 printf("SDL_JoystickNumAxes(sticks[%d]):->%d \n",i, SDL_JoystickNumAxes(sticks[i]));
	}
	//SDL_Joystick *stick = SDL_JoystickOpen(0);
	//SDL_Surface *screen = SDL_SetVideoMode(480,320, 32, SDL_HWSURFACE);	
	bool running = true;

	CGPoint point;

	Config cfg;
 	try
  	{
	  cfg.readFile("config.cfg");
	}
	catch(const FileIOException &fioex)
	{
	  std::cerr << "I/O error while reading file." << std::endl;
	  return(EXIT_FAILURE);
	}
	catch(const ParseException &pex)
	{
	  std::cerr << "Parse error at " << pex.getFile() << ":" << pex.getLine()
	            << " - " << pex.getError() << std::endl;
	  return(EXIT_FAILURE);
	}

    CGRect screenBounds = CGDisplayBounds(CGMainDisplayID());
	while(running){
		SDL_Delay(1000/60);
		for(int i = 0; i < numJoysticks; i++){
			if(!cfg.exists(format("controller%d",i))) continue;
			float sensitivity = 10;
			cfg.lookupValue(format("controller%d.sensitivity",i), sensitivity);
			// 2^16 / 2 == signed 16 bit integer
			CGEventRef eventPos= CGEventCreate(NULL);
			point = CGEventGetLocation(eventPos);
			CFRelease(eventPos);
			for(int axis = 0; axis < SDL_JoystickNumAxes(sticks[i]); axis ++){
				bool mouse_x = false, mouse_y = false;
				cfg.lookupValue(format("controller%d.axis%d.mouse_x",i, axis), mouse_x);
				cfg.lookupValue(format("controller%d.axis%d.mouse_y",i, axis), mouse_y);

				float pos = SDL_JoystickGetAxis(sticks[i],axis) / 32767.0;
				float delta = pos * sensitivity;



				if(fabs(pos) > 0.05){
					if(mouse_x)
						point.x += delta;
					if(mouse_y)
						point.y += delta;
					CGEventRef mouse_event = CGEventCreateMouseEvent(NULL, kCGEventMouseMoved, point, 0);
					if(mouse_x)
						CGEventSetIntegerValueField(mouse_event, kCGMouseEventDeltaX, delta);
					if(mouse_y)
						CGEventSetIntegerValueField(mouse_event, kCGMouseEventDeltaY, delta);
					CGEventSetType(mouse_event, kCGEventMouseMoved);
					CGEventPost(kCGHIDEventTap, mouse_event);
					CFRelease(mouse_event);

				}

				/*float xPos = SDL_JoystickGetAxis(sticks[i], 2) / 32767.0;
				float yPos = SDL_JoystickGetAxis(sticks[i], 3) / 32767.0;
				CGEventRef posEvent = CGEventCreate(NULL);
				point = CGEventGetLocation(posEvent); 
				float dX= (sensitivity * xPos), dY= (sensitivity * yPos);
				CFRelease(posEvent);

			
				if(fabs(xPos) > .05 || fabs(xPos) > .05){
					if(point.x + dX < screenBounds.size.width && point.x + dX > 0 && fabs(xPos) > .05)
						point.x += dX;
					//else dX = 0;
					if(point.y + dY < screenBounds.size.height && point.y + dY > 0 && fabs(yPos) > .05)
						point.y += dY;
					//else dY = 0;
					CGEventRef event = CGEventCreateMouseEvent(CGEventSourceCreate(kCGEventSourceStateHIDSystemState),kCGEventMouseMoved , point,  0);

					CGEventSetIntegerValueField(event, kCGMouseEventClickState, 1);

					if(fabs(xPos) > .05)
						CGEventSetIntegerValueField(event, kCGMouseEventDeltaX, dX);
					if(fabs(yPos) > .05)
						CGEventSetIntegerValueField(event, kCGMouseEventDeltaY, dY);

					point.x = max(min(float(screenBounds.size.width), float(point.x)), 0.0f);
					point.y = max(min(float(screenBounds.size.height), float(point.y)), 0.0f);
					CGEventSetType(event, kCGEventMouseMoved);// apparently there is a apple bug that requires this...
					CGEventPost(kCGHIDEventTap, event);
					CFRelease(event);*/
				//}
			}
		}
		while(SDL_PollEvent(&evt)){
			switch(evt.type){
				case SDL_JOYBUTTONDOWN:{
					bool left_mouse = false;
					bool right_mouse = false;
					int keycode = -1;
					if(cfg.lookupValue(format(button_str, evt.jbutton.which, evt.jbutton.button, "left_mouse"), left_mouse)){
						if(left_mouse){
							CGEventRef event = CGEventCreateMouseEvent(NULL, kCGEventLeftMouseDown, point, 0);
							CGEventPost(kCGHIDEventTap, event);
							CFRelease(event);
						}
					}else if(cfg.lookupValue(format(button_str, evt.jbutton.which, evt.jbutton.button, "right_mouse"), right_mouse)){
						if(right_mouse){
							CGEventRef event = CGEventCreateMouseEvent(NULL, kCGEventRightMouseDown, point, 0);
							CGEventPost(kCGHIDEventTap, event);
							CFRelease(event);
						}
					}else if(cfg.lookupValue(format(button_str, evt.jbutton.which, evt.jbutton.button, "button"), keycode)){
							CGEventRef event = CGEventCreateKeyboardEvent(NULL, keycode,  true);
							CGEventPost(kCGHIDEventTap, event);
							CFRelease(event);
					}else if(cfg.exists(format(button_str, evt.jbutton.which, evt.jbutton.button, "quit"))){
							running = false;
					}
				}break;
				case SDL_JOYBUTTONUP:{
					bool left_mouse = false;
					bool right_mouse = false;
					int keycode = -1;
					printf("Gamepad #%d pressed button #%d\n", evt.jbutton.which, evt.jbutton.button);
					if(cfg.lookupValue(format(button_str, evt.jbutton.which, evt.jbutton.button, "left_mouse"), left_mouse)){
						if(left_mouse){
							CGEventRef event = CGEventCreateMouseEvent(NULL, kCGEventLeftMouseUp, point, 0);
							CGEventPost(kCGHIDEventTap, event);
							CFRelease(event);
						}
					}else if(cfg.lookupValue(format(button_str, evt.jbutton.which, evt.jbutton.button, "right_mouse"), right_mouse)){
						if(right_mouse){
							CGEventRef event = CGEventCreateMouseEvent(NULL, kCGEventRightMouseUp, point, 0);
							CGEventPost(kCGHIDEventTap, event);
							CFRelease(event);
						}
					}else if(cfg.lookupValue(format(button_str, evt.jbutton.which, evt.jbutton.button, "button"), keycode)){
							CGEventRef event = CGEventCreateKeyboardEvent(NULL, keycode,  false);
							CGEventPost(kCGHIDEventTap, event);
							CFRelease(event);
					}
				}break;
			}
		}
	}
	//delete Cfg;
	for(int i = 0; i < numJoysticks; i++){
		SDL_JoystickClose(sticks[i]);
	}
	delete [] sticks;
	return 0;
}
