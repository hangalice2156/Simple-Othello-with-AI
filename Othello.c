#include <stdio.h>
#include <stdlib.h>

struct board {
    char place[8][8];
    int player;
    int ai_turn;
    int move;
};

struct move {
    int row;
    int col;
};

void show_board(struct board *game);
void ai_move(struct move *m, int enable_cells[8][8]);
void ai_better_move(struct move *m, int enable_cells[8][8], struct board *game);
void update_board(struct board *game ,struct move a);
void get_enable_cells(int player, struct board *game, int enable_cells[8][8]);
int has_enable_cells(int enable_cells[8][8]);
int simple_score(int enable_cells[8][8]);
int can_put_line(int player, int selected[2], int index, struct board *game, int flag);
void reverse(int player, struct move m, struct board *game);
void flip(int player, int iter[2], int index, struct board *game);

int main()
{
    //initialize a board
    struct board game;
    for(int i = 0; i < 8; i++) {
        for(int j = 0; j < 8; j++) {
            game.place[i][j] = 'e';
        }
    }

    game.place[3][3] = 'w';
    game.place[3][4] = 'b';
    game.place[4][3] = 'b';
    game.place[4][4] = 'w';

    game.player = 0;
    game.move = 4;

    //import setting
    printf("Input first or later:\n");
    scanf("%d",&game.ai_turn);

    //game start
    while(game.move != 64) {
        //initialize enabled cells
        int enable_cells[8][8];
        for(int i = 0; i < 8; i++) {
            for(int j = 0; j < 8; j++) {
                enable_cells[i][j] = 0;
            }
        }
        get_enable_cells(game.player, &game, enable_cells);
        //show enable place
        for(int i = 0; i < 8; i++) {
            for(int j = 0; j < 8; j++) {
                printf("%d ", enable_cells[i][j]);
            }
            printf("\n");
        }

        //display current state
        printf("current player:%d\n", game.player);
        struct move m = {-1, -1};
        
        //check if is ai's turn
        if((game.player == 0 && game.ai_turn == 0) || (game.player == 1 && game.ai_turn == 1)) {
            ai_better_move(&m, enable_cells, &game);
        }
        else {
            printf("\nplayer turn:\n");
            scanf("%d %d", &m.row, &m.col);
        }
        printf("move row:%d col:%d\n", m.row, m.col);

        reverse(game.player, m, &game);
        show_board(&game);
        //switch player
        game.player = 1 - game.player;
        game.move++;
        if(m.col == -1 && m.row == -1) game.move--;
    }

    printf("game over\n");
    int white_score = 0;
    int black_score = 0;
    for(int i = 0; i < 8; i++) {
        for(int j = 0; j < 8; j++) {
            if(game.place[i][j] == 'b') {
                black_score++;
            }
            if(game.place[i][j] == 'w') {
                white_score++;
            }
        }
    }
    printf("Black score: %d\nWhite score: %d\n", black_score, white_score);
    if(black_score > white_score) {
        printf("Black wins!\n");
    } else if(white_score > black_score) {
        printf("White wins!\n");
    } else if(white_score == black_score) {
        printf("Draw\n");
    }
}

void show_board(struct board *game)
{
    printf("   0 1 2 3 4 5 6 7\n");
    printf("   ---------------\n");
    for(int i = 0; i < 8; i++) {
        printf("%d |", i);
        for(int j = 0; j < 8; j++) {
            printf("%c ", game->place[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

void ai_move(struct move *m, int enable_cells[8][8])
{
    //just select the top left
    for(int i = 0; i < 8; i++) {
        for(int j = 0; j < 8; j++) {
            if(enable_cells[i][j]) {
                m->row = i;
                m->col = j;
            }
        }
    }
    return;
}

void ai_better_move(struct move *m, int enable_cells[8][8], struct board *game)
{
    //may select the better move, making opponent have less choices
    struct move trial;
    struct board copy;
    trial = *m;
    copy = *game;

    int try_enable[8][8];
    for(int i = 0; i < 8; i++) {
        for(int j = 0; j < 8; j++) {
            try_enable[i][j] = 0;
        }
    }
    int score = 0;
    int best_score = 64; //less is better;

    for(int i = 0; i < 8; i++) {
        for(int j = 0; j < 8; j++) {
            if(enable_cells[i][j]) {
                trial.row = i;
                trial.col = j;
                reverse(copy.player, trial, &copy); //try avaliable moves.
                
                copy.player = 1 - copy.player;
                get_enable_cells(copy.player, &copy, try_enable); //check opponent avaliable moves.

                score = simple_score(try_enable);
                if(score < best_score) {
                    best_score = score;
                    m->row = trial.row;
                    m->col = trial.col;
                }

                copy = *game; //recover the board for the next trial.
            }
        }
    }
    return;
}

int simple_score(int enable_cells[8][8])
{
    int score;
    for(int i = 0; i < 8; i++) {
        for(int j = 0; j < 8; j++) {
            if(enable_cells[i][j]) { //calculate all enable cells
                score++;
            }
        }
    }

    return score;
}

void update_board(struct board *game ,struct move a)
{
    return;
}

void get_enable_cells(int player, struct board *game, int enable_cells[8][8])
{
    for(int i = 0; i < 8; i++) {
        for(int j = 0; j < 8; j++) {
            int selected[2] = {i, j};
            for(int k = 0; k < 8; k++) {
                if(game->place[i][j] == 'e' && can_put_line(player, selected, k, game, 0)) {
                    enable_cells[i][j] = 1;
                }
            }
        }
    }
}

int has_enable_cells(int enable_cells[8][8])
{
    for(int i = 0; i < 8; i++) {
        for(int  j= 0; j < 8; j++) {
            if(enable_cells[i][j] == 1) {
                return 1;
            }
        }
    }
    return 0;
}

int can_put_line(int player, int selected[2], int index, struct board *game, int flag)
{
    int directions[8][2] = {
    {-1,-1}, {0,-1}, {1,-1},
    {-1, 0}, {1, 0},
    {-1,1}, {0,1}, {1,1}
    };

    int tmp[2] = {selected[0], selected[1]};
    tmp[0] = tmp[0] + directions[index][0];
    tmp[1] = tmp[1] + directions[index][1];

    char color = (player == 0)? 'b':'w';
    if (tmp[0] <= 0 || tmp[1] <= 0 || tmp[0] > 7 || tmp[1] > 7) {
        //col
        return 0;
    } else if (game->place[tmp[0]][tmp[1]] == 'e') {
        //row
        return 0;
    }

      if (game->place[tmp[0]][tmp[1]] != color && !flag) {
        // there are place to put
        return can_put_line(player, tmp, index, game, 1);
    } else if (game->place[tmp[0]][tmp[1]] == color && !flag) {
        // same color at the end
        return 0;
    } else if (game->place[tmp[0]][tmp[1]] == color && flag) {
        // there are press from both side
        return 1;
    }
    return can_put_line(player, tmp, index, game, 1);
}

void reverse(int player, struct move m, struct board *game)
{
    char color = (player == 0)? 'b':'w';

    int iter_flip[2] = {m.row, m.col};
    game->place[m.row][m.col] = color;
    for(int i = 0; i < 8; i++) {
        if(can_put_line(player, iter_flip, i, game, 0))
            flip(player, iter_flip, i, game);
    }
}

void flip(int player, int iter[2], int index, struct board *game)
{
    int directions[8][2] = {
    {-1,-1}, {0,-1}, {1,-1},
    {-1, 0}, {1, 0},
    {-1,1}, {0,1}, {1,1}
    };

    int tmp[2] = {iter[0], iter[1]};
    tmp[0] = tmp[0] + directions[index][0];
    tmp[1] = tmp[1] + directions[index][1];

    int x = tmp[0];
    int y = tmp[1];

    char color = (player == 0)? 'b':'w';

    if(game->place[x][y] == color || game->place[x][y] == 'e') {
        //fliped to end
        return;
    }

    game->place[x][y] = color;
    flip(player, tmp, index, game);
}