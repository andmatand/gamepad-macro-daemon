#define _XOPEN_SOURCE 700
#include "main.h"

int main(int argc, char* argv[]) {
    init();
    if (argc > 1) load_mappings(argv[1]);

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

    printf("No more controllers can be added\n");
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
    init_pid_file();

    if (SDL_Init(SDL_INIT_GAMECONTROLLER) != 0) {
        fprintf(stderr, "\nUnable to initialize SDL: %s\n", SDL_GetError());
        exit(1);
    }
    atexit(remove_pid_file);
    atexit(SDL_Quit);

    MAX_CONTROLLERS = 4;
    controllers = malloc(sizeof(SDL_GameController*) * MAX_CONTROLLERS);
    for (int i = 0; i < MAX_CONTROLLERS; i++) {
        controllers[i] = NULL;
    }
}

void init_pid_file() {
    PID_FILENAME = "/tmp/gamepad-macro-daemon.pid";

    // Open the PID file for writing (or create it if it does not exist)
    pidFileStream = fopen(PID_FILENAME, "w");

    // If the PID file was not opened successfully
    if (pidFileStream == NULL) {
        fprintf(stderr, "Cannot open PID file %s for writing\n", PID_FILENAME);
        exit(1);
    }

    // If the PID file is locked
    if (lockf(fileno(pidFileStream), F_TEST, 0) == -1) {
        fprintf(stderr, "Another instance is already running\n");
        exit(1);
    }
    else {
        // Lock the PID file
        lockf(fileno(pidFileStream), F_LOCK, 0);

        // Write the PID to the PID file
        fprintf(pidFileStream, "%d\n", getpid());
        fflush(pidFileStream);
    }
}

void load_mappings(const char* filename) {
    int result = SDL_GameControllerAddMappingsFromFile(filename);
    if (result == -1) {
        printf("Error loading controller mappings\n");
    }
    else {
        printf("Loaded %d controller mappings\n", result);
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
                    printf("Launching Kodi\n");
                    system("/usr/bin/kodi");
                    break;
                default:
                    break;
            }
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

void remove_pid_file() {
    // Close the PID file (and release the lock)
    fclose(pidFileStream);

    // Delete the PID file
    if (unlink(PID_FILENAME) != 0) {
        fprintf(stderr, "Cannot delete PID file %s\n", PID_FILENAME);
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
