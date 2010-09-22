#ifndef COMMAND_OPTIONS_H_
#define COMMAND_OPTIONS_H_

#define PLAYER_NAME_LEN  32

typedef struct {
    int nrounds;
    int vsCPU;
    int exit_game;
    char playername1[PLAYER_NAME_LEN];
    char playername2[PLAYER_NAME_LEN];
} Options;

Options *getMenuOptions();
void clear_screen();
void waitForUserInput();

#endif /* COMMAND_OPTIONS_H_ */
