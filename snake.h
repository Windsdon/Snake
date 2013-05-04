#ifndef SNAKE_H
#define SNAKE_H

#include <SDL/SDL.h>

#define MAP_WIDTH 20
#define MAP_HEIGHT 20

#define DIRECTION_UP 1
#define DIRECTION_DOWN 2
#define DIRECTION_LEFT 4
#define DIRECTION_RIGHT 8
#define DIRECTION_X DIRECTION_LEFT | DIRECTION_RIGHT
#define DIRECTION_Y DIRECTION_UP | DIRECTION_DOWN

#define NEWBODY_FLAG 64
#define WALL_FLAG 128


typedef struct SnakeBodyPart {
    int x;
    int y;
    int direction;
};

enum GAMESTATE {
    GAMESTATE_STOPPED,
    GAMESTATE_NEWGAME,
    GAMESTATE_MAINMENU,
    GAMESTATE_ANIMINTRO,
    GAMESTATE_ANIMOUTRO,
    GAMESTATE_PLAYING,
    GAMESTATE_PAUSED,
    GAMESTATE_LOST,
    GAMESTATE_WON
};


class Snake {
    public:
        void OnInit();
        void OnEvent();
        void OnLoop();
        void OnRender();
        void OnExit();
        void OnExecute();
        void OnKey(SDL_Event *event);
        void OnMouse(SDL_Event *event);
        void StartNewGame();
    protected:
    private:
        int ChangeDirection(int dir);
        const static int screenw = 600;
        const static int screenh = 600;
        int gameState;
        int field[MAP_HEIGHT][MAP_WIDTH];
        int snakeBodyPartsCount;
        SDL_Surface *screen;
        SDL_Surface *gs;
        int food[2];
        Uint32 lastLoopTime;
        const static Uint32 timePerLoop = 100;
        int notChangeKeyFlag;

        SnakeBodyPart *snakeParts[MAP_WIDTH*MAP_HEIGHT];
        int GetMovementAdd(int dir, int mask);
        SnakeBodyPart *getPartAt(int x, int y);
        void makeNewFood();
        int searchForWalls(int i, int j);
};

#endif // SNAKE_H
