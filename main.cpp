#include <SDL/SDL.h>
#include <stdio.h>
#include <ApplicationServices/ApplicationServices.h>
#include <libconfig.h++>
#include <iostream>
#include <stdarg.h>
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
		printf("No joysticks detected!");
	}
	SDL_Joystick *stick = SDL_JoystickOpen(0);
	//SDL_Surface *screen = SDL_SetVideoMode(480,320, 32, SDL_HWSURFACE);	
	bool running = true;
	float sensitivity = 10;
	float coords[] = {0,0};
	SDL_WarpMouse(0,0);
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
	cfg.lookupValue("controller0.sensitivity", sensitivity);
	int width=800, height=600;
	cfg.lookupValue("width", width);
	cfg.lookupValue("height", height);
	while(running){
		SDL_Delay(1000/60);
		float xPos = SDL_JoystickGetAxis(stick, 0) / 32767.0;
		float yPos = SDL_JoystickGetAxis(stick, 1) / 32767.0;
		if(fabs(xPos) > .05)
			coords[0] += (sensitivity * xPos);
		if(fabs(yPos) > .05)
			coords[1] += (sensitivity * yPos);
		if(coords[0] < 0 || coords[0] > width)
			coords[0] = max(min(float(width), coords[0]), 0.0f);
		if(coords[1] < 0 || coords[1] > height)
			coords[1] = max(min(float(height), coords[1]), 0.0f);
		CGWarpMouseCursorPosition((CGPoint){coords[0], coords[1]});
		CGEventRef event = CGEventCreateMouseEvent(NULL,kCGEventMouseMoved , (CGPoint){coords[0], coords[1]}, 0);
		CGEventPost(kCGHIDEventTap, event);
		CFRelease(event);
		while(SDL_PollEvent(&evt)){
			switch(evt.type){
				case SDL_JOYAXISMOTION:{
					/*int axis = evt.jaxis.axis % 2;
					float num = evt.jaxis.value / 32767.0;
					coords[axis] += int(num * sensitivity);
					printf("%s: %f\n", axis==0?"X":"Y", num);
					// SDL_WarpMouse(coords[0], coords[1]); */
					//printf("AXis Event!\n");
				}break;
				case SDL_JOYBUTTONDOWN:{
					bool left_mouse = false;
					bool right_mouse = false;
					int keycode = -1;
					if(cfg.lookupValue(format(button_str, evt.jbutton.which, evt.jbutton.button, "left_mouse"), left_mouse)){
						if(left_mouse){
							CGEventRef event = CGEventCreateMouseEvent(NULL, kCGEventLeftMouseDown, (CGPoint){coords[0], coords[1]}, 0);
							CGEventPost(kCGHIDEventTap, event);
							CFRelease(event);
						}
					}else if(cfg.lookupValue(format(button_str, evt.jbutton.which, evt.jbutton.button, "right_mouse"), right_mouse)){
						if(right_mouse){
							CGEventRef event = CGEventCreateMouseEvent(NULL, kCGEventRightMouseDown, (CGPoint){coords[0], coords[1]}, 0);
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
					printf("%i\n", evt.jbutton.button);
					if(cfg.lookupValue(format(button_str, evt.jbutton.which, evt.jbutton.button, "left_mouse"), left_mouse)){
						if(left_mouse){
							CGEventRef event = CGEventCreateMouseEvent(NULL, kCGEventLeftMouseUp, (CGPoint){coords[0], coords[1]}, 0);
							CGEventPost(kCGHIDEventTap, event);
							CFRelease(event);
						}
					}else if(cfg.lookupValue(format(button_str, evt.jbutton.which, evt.jbutton.button, "right_mouse"), right_mouse)){
						if(right_mouse){
							CGEventRef event = CGEventCreateMouseEvent(NULL, kCGEventRightMouseUp, (CGPoint){coords[0], coords[1]}, 0);
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
	SDL_JoystickClose(stick);
	return 0;
}
