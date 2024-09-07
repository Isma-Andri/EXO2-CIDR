#include <ncurses.h>
#include "utils.h"

void choix_execution();

int main() {
    initscr();
    cbreak();

    choix_execution();

    endwin();
    return 0;
}

void choix_execution() {
    int choice;
    char *options[] = {
        "[1] - Entrer une adresse IP avec CIDR",
        "[2] - Quitter"
    };

    while (1) {
        clear();
        printw("Bienvenue !\n");
        for (int i = 0; i < 2; i++) {
            printw("%s\n", options[i]);
        }
        printw("Choisissez une option: ");
        refresh();
        echo();
        scanw("%d", &choice);
        noecho();
        switch (choice) {
            case 1:
                execution();
                break;
            case 2:
                return;
            default:
                printw("Option invalide. Appuyez sur une touche pour continuer...\n");
                getch();
                break;
        }
    }
}

