#include <SDL2/SDL.h>
#include <stdint.h>
#include <stdio.h> // for printf()
#include <stdlib.h> // for atexit()
#include <unistd.h> // for PID stuff

const char* PID_FILENAME;
FILE* pidFileStream;

int MAX_CONTROLLERS;
SDL_GameController** controllers;

void add_controller(int32_t);
SDL_GameController* get_controller(SDL_JoystickID);
SDL_JoystickID get_joystick_id(SDL_GameController*);
void init();
void init_pid_file();
void load_mappings(const char* filename);
void process_button(SDL_JoystickID, SDL_GameControllerButton);
void quit();
void remove_pid_file();
void remove_controller(SDL_JoystickID);
void vacate_controller_array_slot(SDL_GameController*);
