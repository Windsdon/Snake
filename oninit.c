#include "snake.h"
#include <iostream>

void Snake::onInit() {

    freopen("CON", "w", stdout);
    freopen("CON", "w", stderr);

    if( !SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) ){
        printf("Failed to init.\n");
    }
}
