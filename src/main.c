#include "KSP2D.h"
#include "SDL.h"
#undef main

/*
void on_center_button() {
	static bool pressed = false;
	pressed = !pressed;
	if (pressed) {
		lcd_set_text(2, "I was pressed!");
	} else {
		lcd_clear_line(2);
	}
}

void initialize() {
    lcd_initialize();
    lcd_set_text(1, "david is a nerd");

    lcd_register_btn1_cb(on_center_button);
}
*/

void anim()
{
    game_update();
    game_draw();
}

int main(int argc, char** argv) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) return 1;
    SDL_Window* window = NULL;
    SDL_Surface* screen = NULL;
    if ((window = SDL_CreateWindow("TomatoSoup", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 480, 240, SDL_WINDOW_SHOWN)) == 0) return 1;
    if ((screen = SDL_GetWindowSurface(window)) == -0) return 1;
	game_init();


    unsigned int frames = 0;
    unsigned int tNow = SDL_GetTicks();
    unsigned int tLast = tNow;

    int isRunning = 1;
    int makeUpTime = 0;
    while (isRunning)
    {
        frames++;

        SDL_Event evt;
        while (SDL_PollEvent(&evt)) {
            switch (evt.type)
            {
            case SDL_KEYDOWN: 
            switch (evt.key.keysym.sym)
            {
                default: 
                break;
            }
            break;
            case SDL_QUIT: isRunning = 0;
                break;
            default: 
                break;
            }
        }

        anim();

        ZB_copyFrameBuffer(frameBuffer, screen->pixels, screen->pitch);
        SDL_UpdateWindowSurface(window);

        tNow = SDL_GetTicks64();
        int dt = tNow - tLast;
        tLast = tNow;
        frames = frames % targetFPS;
        int targetDT = 1000 / targetFPS;
        int tFPS = targetDT - dt;//time to delay
        if (tFPS < 0) makeUpTime += tFPS;
        if (tFPS > 0 && makeUpTime < 10) {
            SDL_Delay(tFPS);
        }//if we need to delay, and we dont have make up time, then delay
    }

    return 1;
}