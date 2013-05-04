#include "snake.h"

int main ( int argc, char** argv ) {
    Snake game = Snake();

    game.OnInit();

    printf("Exited cleanly\n");
    return 0;
}
