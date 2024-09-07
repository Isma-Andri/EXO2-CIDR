#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ncurses.h>
#include "utils.h"

char* to_binary(int a) {
    char* binary_str = (char*)malloc(9);
    binary_str[8] = '\0';
    for (int i = 7; i >= 0; i--) {
		if (a % 2 == 1) {
			binary_str[i] ='1';
		}
		else {
			binary_str[i] = '0';
		}
        a /= 2;
    }
    return binary_str;
}

int binary_to_decimal(const char* binary_str) {
    int decimal = 0;
    int length = strlen(binary_str);
    for (int i = 0; i < length; i++) {
        if (binary_str[i] == '1') {
            decimal += pow(2, length - i - 1);
        }
    }
    return decimal;
}

void calculer_netmask(int cidr, int netmask[4]) {
    for (int i = 0; i < 4; i++) {
        if (cidr >= 8) {
            netmask[i] = 255;
            cidr -= 8;
        } else if (cidr > 0) {
            netmask[i] = 256 - pow(2, (8 - cidr));
            cidr = 0;
        } else {
            netmask[i] = 0;
        }
    }
}

void calculer_reseau(int IP[4], int netmask[4], int reseau[4]) {
    for (int i = 0; i < 4; i++) {
        reseau[i] = IP[i] & netmask[i];
    }
}

void calculer_diffusion(int reseau[4], int netmask[4], int diffusion[4]) {
    for (int i = 0; i < 4; i++) {
        diffusion[i] = reseau[i] + (255 - netmask[i]);
    }
}

int calculer_nb_hotes(int cidr) {
    if (cidr >= 31) {
        return 0;
    }
    return (int)pow(2, (32 - cidr)) - 2;
}

void afficher_resultats(int IP[4], int netmask[4], int nb_hote, int reseau[4], int diffusion[4]) {
    clear();
    mvprintw(0, 0, "Resultats du calcul :");
    mvprintw(2, 0, "Adresse IP: %d.%d.%d.%d", IP[0], IP[1], IP[2], IP[3]);
    mvprintw(3, 0, "Masque de sous-reseau: %d.%d.%d.%d", netmask[0], netmask[1], netmask[2], netmask[3]);
    mvprintw(4, 0, "Adresse du reseau: %d.%d.%d.%d", reseau[0], reseau[1], reseau[2], reseau[3]);
    mvprintw(5, 0, "Adresse de diffusion: %d.%d.%d.%d", diffusion[0], diffusion[1], diffusion[2], diffusion[3]);
    mvprintw(6, 0, "Nombre d'hotes disponibles: %d", nb_hote);
    mvprintw(7, 0, "Appuyez sur une touche pour continuer...");
}

void subdiviser_reseaux(int reseau[4], int netmask[4], int cidr, int sous_cidr) {
    clear();
    int nb_sous_reseaux = pow(2, sous_cidr - cidr);
    int incr = 256 / pow(2, sous_cidr - cidr);

    mvprintw(0, 0, "Sous-reseaux crees avec /%d :", sous_cidr);
    for (int i = 0; i < nb_sous_reseaux; i++) {
        int sous_reseau[4] = {reseau[0], reseau[1], reseau[2], reseau[3] + i * incr};
        int diffusion[4];
        calculer_diffusion(sous_reseau, netmask, diffusion);

        mvprintw(i + 2, 0, "Sous-reseau %d : %d.%d.%d.%d - Diffusion: %d.%d.%d.%d", 
                 i + 1, sous_reseau[0], sous_reseau[1], sous_reseau[2], sous_reseau[3],
                 diffusion[0], diffusion[1], diffusion[2], diffusion[3]);
    }
    mvprintw(nb_sous_reseaux + 3, 0, "Appuyez sur une touche pour continuer...");
    getch();
}

int valider_ip(int IP[4]) {
    for (int i = 0; i < 4; i++) {
        if (IP[i] < 0 || IP[i] > 255) {
            return 0;
        }
    }
    return 1;
}

int valider_cidr(int cidr) {
    return (cidr >= 0 && cidr <= 32);
}

void execution() {
    clear();

    char ip_str[19];
    int IP[4], cidr, sous_cidr;

    echo();
    printw("Entrez l'adresse en format CIDR (Exemple: 192.168.43.10/26): ");
    getnstr(ip_str, sizeof(ip_str) - 1);

    if (sscanf(ip_str, "%d.%d.%d.%d/%d", &IP[0], &IP[1], &IP[2], &IP[3], &cidr) != 5 || !valider_ip(IP)) {
        mvprintw(2, 0, "Adresse IP invalide ou erreur de saisie.");
        getch();
        return;
    }
    if (!valider_cidr(cidr)) {
        mvprintw(3, 0, "CIDR invalide.");
        getch();
        return;
    }

    int netmask[4], reseau[4], diffusion[4];
    calculer_netmask(cidr, netmask);
    calculer_reseau(IP, netmask, reseau);
    calculer_diffusion(reseau, netmask, diffusion);
    int nb_hote = calculer_nb_hotes(cidr);
    afficher_resultats(IP, netmask, nb_hote, reseau, diffusion);

    mvprintw(8, 0, "Entrez le CIDR des sous-reseaux (plus grand que %d): ", cidr);
    scanw("%d", &sous_cidr);

    if (!valider_cidr(sous_cidr) || sous_cidr <= cidr) {
        mvprintw(9, 0, "CIDR des sous-reseaux invalide.");
        getch();
        return;
    }

    subdiviser_reseaux(reseau, netmask, cidr, sous_cidr);
    getch();
}
