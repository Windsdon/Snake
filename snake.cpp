#include "snake.h"

#ifdef __cplusplus
#include <cstdlib>
#else
#include <stdlib.h>
#endif

#include <iostream>

void Snake::OnInit() {

    freopen("CON", "w", stdout);
    freopen("CON", "w", stderr);

    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
        printf("Failed to init!\n");
        printf(SDL_GetError());
        exit(1);
    }

    screen = SDL_SetVideoMode(screenw, screenh, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);

    gs = SDL_CreateRGBSurface(SDL_HWSURFACE | SDL_DOUBLEBUF, screenw, screenh, 32, 0, 0, 0, 0);

    OnExecute();
}

void Snake::OnExecute() {
    gameState = GAMESTATE_NEWGAME;

    lastLoopTime = SDL_GetTicks();

    while(gameState) {
        if(gameState != GAMESTATE_NEWGAME) {
            OnEvent();
        }

        Uint32 timeNow = SDL_GetTicks();
        Uint32 delta = timeNow - lastLoopTime;
        while(delta > timePerLoop) {
            OnLoop();
            delta -= timePerLoop;
        }

        OnRender();
    }

    OnExit();
}

void Snake::OnEvent() {
    SDL_Event event;

    while(SDL_PollEvent(&event)) {
        switch(event.type) {
        case SDL_KEYDOWN:
            OnKey(&event);
            break;
        case SDL_MOUSEMOTION:
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
            OnMouse(&event);
            break;
        case SDL_QUIT:
            gameState = 0;
            break;
        }
    }
}

void Snake::OnKey(SDL_Event *event) {
    if(notChangeKeyFlag) {
        return;
    }
    printf("Key!");
    if(event->key.keysym.sym == SDLK_LEFT || event->key.keysym.sym == SDLK_a) {
        ChangeDirection(DIRECTION_LEFT);
        notChangeKeyFlag = 1;
    }
    if(event->key.keysym.sym == SDLK_RIGHT || event->key.keysym.sym == SDLK_d) {
        ChangeDirection(DIRECTION_RIGHT);
        notChangeKeyFlag = 1;
    }
    if(event->key.keysym.sym == SDLK_UP || event->key.keysym.sym == SDLK_w) {
        ChangeDirection(DIRECTION_UP);
        notChangeKeyFlag = 1;
    }
    if(event->key.keysym.sym == SDLK_DOWN || event->key.keysym.sym == SDLK_s) {
        ChangeDirection(DIRECTION_DOWN);
        notChangeKeyFlag = 1;
    }
}

void Snake::OnMouse(SDL_Event *event) {
    if(event->type == SDL_MOUSEBUTTONDOWN){
        gameState = GAMESTATE_NEWGAME;
    }
}

int Snake::ChangeDirection(int dir) {
    SnakeBodyPart *head = snakeParts[0];
    if((dir == DIRECTION_DOWN && head->direction != DIRECTION_UP)
            || (dir == DIRECTION_UP && head->direction != DIRECTION_DOWN)
            || (dir == DIRECTION_LEFT && head->direction != DIRECTION_RIGHT)
            || (dir == DIRECTION_RIGHT && head->direction != DIRECTION_LEFT)) {
        head->direction = dir;
        return 1;
    } else {
        return 0;
    }
}

void Snake::OnLoop() {
    notChangeKeyFlag = 0;
    printf("Loop\n");
    lastLoopTime = SDL_GetTicks();
    if(gameState == GAMESTATE_NEWGAME) {
        StartNewGame();
        return;
    }
    if(gameState != GAMESTATE_PLAYING) {
        return;
    }

    int i, newfood = 0;
    for(i = 0; i < snakeBodyPartsCount; i++) {
        SnakeBodyPart *part = snakeParts[i];
        int oldx = part->x,
                   oldy = part->y,
                          newx = oldx + GetMovementAdd(part->direction, DIRECTION_X),
                                 newy = oldy + GetMovementAdd(part->direction, DIRECTION_Y);
        //printf("At (%i, %i) to (%i, %i)\n", oldx, oldy, newx, newy);
        if(i == 0) {
            if(!getPartAt(newx, newy) && !(field[newy][newx]&WALL_FLAG)) {
                if(food[0] == newx && food[1] == newy) {
                    field[newy][newx] |= NEWBODY_FLAG;
                    newfood = 1;
                }
                part->x = newx;
                part->y = newy;
                field[oldy][oldx] = part->direction|(field[oldy][oldx]&NEWBODY_FLAG);
            } else {
                //startOutro();
                printf("You lose!\n");
                return;
            }
        } else {
            part->x = newx;
            part->y = newy;
            part->direction = field[newy][newx];
        }
        //printf("Body parts: %i\ni: %i\n", snakeBodyPartsCount, i);
        if(i == snakeBodyPartsCount - 1 && (field[oldy][oldx]&NEWBODY_FLAG)) {
            printf("New part created.\n");
            SnakeBodyPart *newpart = snakeParts[snakeBodyPartsCount++] = new SnakeBodyPart;
            newpart->x = oldx;
            newpart->y = oldy;
            newpart->direction = field[oldy][oldx]&~NEWBODY_FLAG;
            field[oldy][oldx] = newpart->direction;
            break;
        }
    }

    if(newfood) {
        makeNewFood();
    }
}

void Snake::StartNewGame() {
    int i,j;

    /* set all the direction changers to 0*/
    for(i = 0; i < MAP_HEIGHT; i++) {
        for(j = 0; j < MAP_WIDTH; j++) {
            field[i][j] = (i == 0 || j == 0 || j == MAP_WIDTH - 1 || i == MAP_HEIGHT - 1)?WALL_FLAG:0;
            snakeParts[i*MAP_HEIGHT + j] = 0;
        }
    }

    for(i = 0; i < MAP_HEIGHT; i++){
        for(j = 0; j < MAP_WIDTH; j++){
            if(!(rand()%20) && (i != MAP_WIDTH/2 && j != MAP_HEIGHT/2) && searchForWalls(i, j) < 2){
                field[i][j] = WALL_FLAG;
            }
        }
    }

    SnakeBodyPart *head = snakeParts[0] = new SnakeBodyPart;
    head->direction = DIRECTION_RIGHT;
    head->x = MAP_WIDTH/2;
    head->y = MAP_HEIGHT/2;

    gameState = GAMESTATE_PLAYING;
    snakeBodyPartsCount = 1;
    makeNewFood();
}

void Snake::OnRender() {
    int i, j;
    double xsize = ((double) screenw)/MAP_WIDTH;
    double ysize = ((double) screenh)/MAP_HEIGHT;

    SDL_FillRect(screen, 0, SDL_MapRGB(screen->format, 0, 0, 0));
    for(i = 0; i < MAP_WIDTH; i++) {
        for(j = 0; j < MAP_HEIGHT; j++) {
            SDL_Rect ds;
            ds.x = i*xsize + 1;
            ds.y = j*ysize + 1;
            ds.w = xsize - 2;
            ds.h = ysize - 2;
            if(field[j][i]&WALL_FLAG) {
                SDL_FillRect(screen, &ds, SDL_MapRGB(screen->format, 0, 0, 0));
            } else {
                SDL_FillRect(screen, &ds, SDL_MapRGB(screen->format, 0, 0x20, 0x20));
            }
        }
    }

    for(i = 0; i < snakeBodyPartsCount; i++) {
        SDL_Rect ds;
        SnakeBodyPart *part = snakeParts[i];
        ds.x = part->x*xsize + 1;
        ds.y = part->y*ysize + 1;
        ds.w = xsize - 2;
        ds.h = ysize - 2;

        if(!i) {
            SDL_FillRect(screen, &ds, SDL_MapRGB(screen->format, 0xcc, 0xcc, 0xcc));
        } else {
            SDL_FillRect(screen, &ds, SDL_MapRGB(screen->format, 0x88, 0x88, 0x88));
        }

    }

    SDL_Rect f;
    f.h = xsize/2;
    f.w = ysize/2;
    f.x = food[0]*xsize + xsize/4;
    f.y = food[1]*ysize + ysize/4;

    SDL_FillRect(screen, &f, SDL_MapRGB(screen->format, 0, 0xcc, 0));

    SDL_Flip(screen);
}

void Snake::OnExit() {
    SDL_Quit();
}

int Snake::GetMovementAdd(int dir, int mask) {
    return !!(dir&mask&DIRECTION_RIGHT)+ !!(dir&mask&DIRECTION_DOWN) - !!(dir&mask&DIRECTION_UP) - !!(dir&mask&DIRECTION_LEFT);
}

SnakeBodyPart *Snake::getPartAt(int x, int y) {
    int i;
    for(i = 0; i < snakeBodyPartsCount; i++) {
        SnakeBodyPart *p = snakeParts[i];
        if(p->x == x && p->y == y) {
            return p;
        }
    }
    return 0;
}

int Snake::searchForWalls(int x, int y){
    int i, j, c = 0;

    for(i = x - 1; i <= x + 1; i++){
        if(i >= MAP_WIDTH || i < 0){
            continue;
        }
        for(j = y - 1; j <= y + 1; j++){
            if(j >= MAP_HEIGHT || j < 0){
                continue;
            }
            if(field[j][i]&WALL_FLAG){
                c++;
            }
        }
    }

    return c;
}

void Snake::makeNewFood() {
    int foodx, foody;
    do {
        foodx = rand()%MAP_WIDTH;
        foody = rand()%MAP_HEIGHT;
    } while(getPartAt(foodx, foody) || field[foody][foodx]&WALL_FLAG);
    food[0] = foodx;
    food[1] = foody;
}
