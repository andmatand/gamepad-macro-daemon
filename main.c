#include "main.h"

int main(int argc, char* argv[]) {
    init();

    printf("Listening for gamepads...\n");
    SDL_GameController* controller = NULL;
    while (1) {
        SDL_WaitEvent(NULL);

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                exit(0);
            }
            else if (event.type == SDL_CONTROLLERDEVICEADDED) {
                printf("Gamepad connected (index: %d)\n", event.cdevice.which);

                if (controller) {
                    // Close the previous controller
                    SDL_GameControllerClose(controller);
                    controller = NULL;
                }

                // Open the controller
                controller = SDL_GameControllerOpen(event.cdevice.which);
            }
            else if (event.type == SDL_CONTROLLERDEVICEREMOVED) {
                printf("Gamepad disconnected (device id: %d)\n",
                       event.cdevice.which);

                if (controller) {
                    // Get the joystick underlying the controller
                    SDL_Joystick* joystick = NULL;
                    joystick = SDL_GameControllerGetJoystick(controller);

                    // Get the SDL_JoystickID of the joystick
                    SDL_JoystickID id = SDL_JoystickInstanceID(joystick);

                    // If the device that was just removed is the current
                    // joystick
                    if (event.cdevice.which == id) {
                        SDL_GameControllerClose(controller);
                        controller = NULL;
                    }
                }
            }
            else if (event.type == SDL_CONTROLLERBUTTONDOWN) {
                // If the "guide" button is down
                if (SDL_GameControllerGetButton(controller,
                                                SDL_CONTROLLER_BUTTON_GUIDE))
                {
                    if (event.cbutton.button ==
                        SDL_CONTROLLER_BUTTON_LEFTSHOULDER) {
                        printf("Launching Steam\n");
                        system("/usr/bin/steam");
                    }
                    else if (event.cbutton.button ==
                             SDL_CONTROLLER_BUTTON_RIGHTSHOULDER) {
                        printf("Launching XBMC\n");
                        system("/usr/bin/xbmc");
                    }
                }
            }
        }
    }

    return 0;
}

void init() {
    if (SDL_Init(SDL_INIT_GAMECONTROLLER) != 0) {
        fprintf(stderr, "\nUnable to initialize SDL: %s\n", SDL_GetError());
        exit(1);
    }
    atexit(SDL_Quit);
}
