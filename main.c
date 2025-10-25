#define SDL_MAIN_USE_CALLBACKS 1  /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <stdio.h>

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

typedef struct {
    float accX;
    float accY;
    float accZ;
    float rotX;
    float rotY;
    float rotZ;
} SensorReading;

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    SensorReading *reading;

    /* Create the window */
    if (!SDL_CreateWindowAndRenderer("Sensor readings", 800, 600, 0, &window, &renderer)) {
        SDL_Log("Couldn't create window and renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    /* Init sensor API */
    if (!SDL_Init(SDL_INIT_SENSOR)) {
        SDL_Log("Could not initialize sensor API: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!(reading = (SensorReading *)SDL_calloc(1, sizeof(*reading)))) {
        return SDL_APP_FAILURE;
    }
    *(SensorReading**)appstate = reading;

    reading->accX = 0.0;
    reading->accY = 0.0;
    reading->accZ = 0.0;
    reading->rotX = 0.0;
    reading->rotY = 0.0;
    reading->rotZ = 0.0;

    SDL_Log("Is enabled %d", SDL_EventEnabled(SDL_EVENT_SENSOR_UPDATE));

    return SDL_APP_CONTINUE;
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    
    if (event->type == SDL_EVENT_KEY_DOWN ||
        event->type == SDL_EVENT_QUIT) {

        if (((SDL_KeyboardEvent*)event)->key == SDLK_F10) {
            SDL_WindowFlags flag = SDL_GetWindowFlags(window);
	        bool is_fullscreen = flag & SDL_WINDOW_FULLSCREEN;
            SDL_SetWindowFullscreen(window, !is_fullscreen);
            return SDL_APP_CONTINUE;
        }

        return SDL_APP_SUCCESS;  /* end the program, reporting success to the OS. */
    }

    SensorReading* reading = (SensorReading*)appstate;

    if (event->type == SDL_EVENT_SENSOR_UPDATE) {
        SDL_SensorEvent *sensor_event = (SDL_SensorEvent*)event;
        SDL_Sensor *sensor = SDL_GetSensorFromID(sensor_event->which);
        SDL_SensorType sensor_type = SDL_GetSensorType(sensor);
        
        if (sensor_type == SDL_SENSOR_ACCEL) {
            reading->accX = sensor_event->data[0];
            reading->accY = sensor_event->data[1];
            reading->accZ = sensor_event->data[2];
        }

        if (sensor_type == SDL_SENSOR_GYRO) {
            reading->rotX = sensor_event->data[0];
            reading->rotY = sensor_event->data[1];
            reading->rotZ = sensor_event->data[2];
        }
    }
    return SDL_APP_CONTINUE;
}

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void *appstate)
{
    SensorReading* reading = (SensorReading*)appstate;
    const float SCALE = 4.0f;
    const int STR_SIZE = SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE * 9;
    const float CENTER_XOFFSET = SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE * 2;
    const float CENTER_YOFFSET = SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE * 2;
    const char *ACCL_MSG = "ACCL";
    const char *GYRO_MSG = "GYRO";
    char output[10];
    int w = 0, h = 0;
    float cx, cy;
    float x, y;

    /* Center the message and scale it up */
    SDL_GetRenderOutputSize(renderer, &w, &h);
    SDL_SetRenderScale(renderer, SCALE, SCALE);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    
    cx = (w / SCALE) / 2;
    cy = (h / SCALE) / 2;

    /* Draw the message */
    x = cx - CENTER_XOFFSET - STR_SIZE;
    y = cy - CENTER_YOFFSET;

    /* ACCELORMETER */
    SDL_RenderDebugText(renderer, x, y, ACCL_MSG);

    y += SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE;
    snprintf(output, 10, "X: %.2f", reading->accX);
    SDL_RenderDebugText(renderer, x, y, output);

    y += SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE;
    snprintf(output, 10, "Y: %.2f", reading->accY);
    SDL_RenderDebugText(renderer, x, y, output);

    y += SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE;
    snprintf(output, 10, "Z: %.2f", reading->accZ);
    SDL_RenderDebugText(renderer, x, y, output);

    /* GYROSCOPE */
    x = cx + CENTER_XOFFSET;
    y = cy - CENTER_YOFFSET;
    
    SDL_RenderDebugText(renderer, x, y, GYRO_MSG);

    y += SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE;
    snprintf(output, 10, "X: %.2f", reading->rotX);
    SDL_RenderDebugText(renderer, x, y, output);

    y += SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE;
    snprintf(output, 10, "Y: %.2f", reading->rotY);
    SDL_RenderDebugText(renderer, x, y, output);

    y += SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE;
    snprintf(output, 10, "Z: %.2f", reading->rotZ);
    SDL_RenderDebugText(renderer, x, y, output);

    /* Render to screen */
    SDL_RenderPresent(renderer);

    return SDL_APP_CONTINUE;
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
}

