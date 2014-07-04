#include "main.h"

int main(int argc, char* argv[]) {
    init();

    printf("Listening for gamepads...\n");
    while (1) {
        SDL_WaitEvent(NULL);

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit();
            }
            else if (event.type == SDL_CONTROLLERDEVICEADDED) {
                printf("Gamepad connected (index: %d)\n", event.cdevice.which);
                add_controller(event.cdevice.which);
            }
            else if (event.type == SDL_CONTROLLERDEVICEREMOVED) {
                printf("Gamepad disconnected (instance id: %d)\n",
                       event.cdevice.which);

                // Close the controller and free its slot in our array (if it
                // is currently in our array)
                remove_controller(event.cdevice.which);
            }
            else if (event.type == SDL_CONTROLLERBUTTONDOWN) {
                process_button(event.cbutton.which, event.cbutton.button);
            }
        }
    }

    return 0;
}

void add_controller(int32_t deviceIndex) {
    // Find an empty slot in the controller array
    for (int i = 0; i < MAX_CONTROLLERS; i++) {
        if (controllers[i] == NULL) {
            // Open the controller
            controllers[i] = SDL_GameControllerOpen(deviceIndex);

            return;
        }
    }

    printf("No more controllers can be added.\n");
}

SDL_GameController* get_controller(SDL_JoystickID id) {
    // Search the controller array for the controller that has the specified
    // SDL_JoystickID
    for (int i = 0; i < MAX_CONTROLLERS; i++) {
        if (controllers[i]) {
            if (get_joystick_id(controllers[i]) == id) {
                return controllers[i];
            }
        }
    }

    return NULL;
}

SDL_JoystickID get_joystick_id(SDL_GameController* controller) {
    // Get the joystick underlying the controller
    SDL_Joystick* joystick = SDL_GameControllerGetJoystick(controller);

    // Return the instance ID of the joystick
    return SDL_JoystickInstanceID(joystick);
}

uint8_t guide_is_down(SDL_GameController* controller) {
    return SDL_GameControllerGetButton(
            controller,
            SDL_CONTROLLER_BUTTON_GUIDE);
}

void init() {
    if (SDL_Init(SDL_INIT_GAMECONTROLLER) != 0) {
        fprintf(stderr, "\nUnable to initialize SDL: %s\n", SDL_GetError());
        exit(1);
    }
    atexit(SDL_Quit);

    MAX_CONTROLLERS = 4;
    controllers = malloc(sizeof(SDL_GameController*) * MAX_CONTROLLERS);
    for (int i = 0; i < MAX_CONTROLLERS; i++) {
        controllers[i] = NULL;
    }
}

void process_button(SDL_JoystickID jsId, SDL_GameControllerButton button) {
    SDL_GameController* controller = get_controller(jsId);

    // If a controller with the specified joystick ID was found in our array
    if (controller) {
        // If the "guide" button is down on this controller
        if (guide_is_down(controller)) {
            switch (button) {
                case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
                    printf("Launching Steam\n");
                    system("/usr/bin/steam");
                    break;
                case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
                    printf("Launching XBMC\n");
                    system("/usr/bin/xbmc");
                    break;
                default:
                    break;
            }
        }
    }
}

void vacate_controller_array_slot(SDL_GameController* controller) {
    for (int i = 0; i < MAX_CONTROLLERS; i++) {
        if (controllers[i] == controller) {
            controllers[i] = NULL;
            return;
        }
    }
}

void quit() {
    free(controllers);
    exit(0);
}

void remove_controller(SDL_JoystickID id) {
    SDL_GameController* controller = get_controller(id);

    // If the device that was just removed is an existing controller in our
    // array
    if (controller) {
        // Close the controller
        SDL_GameControllerClose(controller);
        vacate_controller_array_slot(controller);
    }
}
