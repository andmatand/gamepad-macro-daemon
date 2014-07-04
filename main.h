#include <SDL2/SDL.h>
#include <stdint.h>
#include <stdio.h> // for printf()
#include <stdlib.h> // for atexit()

int MAX_CONTROLLERS;
SDL_GameController** controllers;

void add_controller(int32_t);
SDL_GameController* get_controller(SDL_JoystickID);
SDL_JoystickID get_joystick_id(SDL_GameController*);
void init();
void process_button(SDL_JoystickID, SDL_GameControllerButton);
void quit();
void remove_controller(SDL_JoystickID);
void vacate_controller_array_slot(SDL_GameController*);
