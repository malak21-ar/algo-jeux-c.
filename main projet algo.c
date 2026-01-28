#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// STRUCTURES
typedef struct {
    int num;
    char nom[30];
    int age;

    int score_partie;
    int total_gagnes;
    int total_perdus;

    int victoires_consecutives;
    int defaites_total;
} Joueur;

typedef struct Cellule {
    Joueur info;
    struct Cellule* suiv;
} Cellule;

typedef struct {
    Cellule* tete;
    Cellule* queue;
} File;

typedef struct CellListe {
    Joueur info;
    struct CellListe* suiv;
} CellListe;

typedef struct {
    CellListe* tete;
} Liste;

// STRATÉGIE 2
int PGCD(int A, int B) {
    if (A % B == 0) return B;
    else return PGCD(B, A % B);
}

int chiffreDansNombre(int chiffre, int nombre) {
    if (chiffre < 0) chiffre = -chiffre;
    if (nombre < 0) nombre = -nombre;

    if (chiffre == 0 && nombre == 0) return 1;

    while (nombre > 0) {
        if (nombre % 10 == chiffre) {
            return 1;
        }
        nombre = nombre / 10;
    }
    return 0;
}

int jouerTourStrategie2() {
    int A = rand() % 100 + 1;
    int B = rand() % 100 + 1;
    int p = PGCD(A, B);

    int temp = p;
    while (temp != 0) {
        int chiffre = temp % 10;
        if (chiffreDansNombre(chiffre, A) || chiffreDansNombre(chiffre, B)) {
            return 1;
        }
        temp = temp / 10;
    }
    return 0;
}

//FILES
void initFile(File* F) {
    F->tete = NULL;
}

int fileVide(File *F) {
    return (F->tete == NULL);
}

void enfiler(File* F, Joueur j) {
    Cellule* nouv = (Cellule*)malloc(sizeof(Cellule));
    nouv->info = j;
    nouv->suiv = NULL;

    if (F->tete == NULL) {
        F->tete = F->queue = nouv;
    } else {
        F->queue->suiv = nouv;
        F->queue = nouv;
    }
}

int defiler(File* F, Joueur* j) {
    if (F->tete == NULL) return 0;

    Cellule* tmp = F->tete;
    *j = tmp->info;
    F->tete = tmp->suiv;

    if (F->tete == NULL) F->queue = NULL;

    free(tmp);
    return 1;
}

void afficherFile(File F, char nom[]) {
    Cellule* p = F.tete;
    printf("%s: [", nom);
    while (p != NULL) {
        printf("%s", p->info.nom);
        if (p->suiv != NULL) printf(", ");
        p = p->suiv;
    }
    printf("]\n");
}

// LISTES
void initListe(Liste* L) {
    L->tete = NULL;
}

void insererListe(Liste* L, Joueur j) {
    CellListe* c = (CellListe*)malloc(sizeof(CellListe));
    c->info = j;
    c->suiv = L->tete;
    L->tete = c;
}

void afficherListe(Liste L, char nom[]) {
    CellListe* p = L.tete;
    printf("%s: [", nom);
    while (p != NULL) {
        printf("%s(%d vict)", p->info.nom, p->info.total_gagnes);
        if (p->suiv != NULL) printf(", ");
        p = p->suiv;
    }
    printf("]\n");
}

// STRATÉGIE 2
int sommeChiffres(int x) {
    int s = 0;
    if (x < 0) x = -x;
    while (x != 0) {
        s += x % 10;
        x /= 10;
    }
    return s;
}

int jouerTour() {
    int val = rand() % 1000000;
    int s = sommeChiffres(val);
    return (s % 5 == 0) ? 1 : 0;
}

void jouerPartie(Joueur* j1, Joueur* j2, int strategie, time_t debutPartie) {
    int tours = 0;
    int maxTours = (strategie == 1) ? 12 : 16;

    j1->score_partie = 0;
    j2->score_partie = 0;

    printf("\n<><><><><><><><><><><><><><><><><><><><><>\n");
    printf("PARTIE (Stratégie %d)\n", strategie);
    printf("Heure début: %s", ctime(&debutPartie));
    printf("%s VS %s\n", j1->nom, j2->nom);
    printf("<><><><><><><><><><><><><><><><><><><><><>\n");

    while (abs(j1->score_partie - j2->score_partie) < 3 && tours < maxTours) {
        if (strategie == 1) {
            j1->score_partie += jouerTour();
            j2->score_partie += jouerTour();
        } else {
            j1->score_partie += jouerTourStrategie2();
            j2->score_partie += jouerTourStrategie2();
        }
        tours++;
        printf("Tour %d: %s=%d, %s=%d\n", tours, j1->nom, j1->score_partie, j2->nom, j2->score_partie);
    }

    printf("Score final: %s = %d, %s = %d\n", j1->nom, j1->score_partie, j2->nom, j2->score_partie);
}

void majResultats(Joueur* gagnant, Joueur* perdant) {
    gagnant->total_gagnes++;
    gagnant->victoires_consecutives++;

    perdant->total_perdus++;
    perdant->victoires_consecutives = 0;
    perdant->defaites_total++;
}

void placerJoueur(File* F, File* F1, File* F3, Joueur j, int strategie) {
    if (strategie == 1) {
        if (j.victoires_consecutives >= 3) {
            enfiler(F3, j);
            printf("%s → F3 (3 victoires consécutives)\n", j.nom);
        } else if (j.victoires_consecutives == 1) {
            enfiler(F1, j);
            printf("%s → F1 (gagnant)\n", j.nom);
        } else {
            enfiler(F, j);
            printf("%s → F\n", j.nom);
        }
    }
}

// SÉLECTION JOUEURS - CORRIGÉ
int selectionnerJoueurs(File* F, File* F1, File* F3, Joueur* j1, Joueur* j2) {
    // Priorité: F1 → F → F3
    if (!fileVide(F1)) {
        if (!defiler(F1, j1)) return 0;
        if (!fileVide(F1)) {
            if (!defiler(F1, j2)) return 0;
        } else if (!fileVide(F)) {
            if (!defiler(F, j2)) return 0;
        } else if (!fileVide(F3)) {
            if (!defiler(F3, j2)) return 0;
        } else {
            return 0;
        }
        return 1;
    }

    if (!fileVide(F)) {
        if (!defiler(F, j1)) return 0;
        if (!fileVide(F)) {
            if (!defiler(F, j2)) return 0;
        } else if (!fileVide(F3)) {
            if (!defiler(F3, j2)) return 0;
        } else {
            return 0;
        }
        return 1;
    }

    if (!fileVide(F3)) {
        if (!defiler(F3, j1)) return 0;
        if (!fileVide(F3)) {
            if (!defiler(F3, j2)) return 0;
        } else {
            return 0;
        }
        return 1;
    }

    return 0;
}


void afficherTop3(Liste LG) {
    printf("\n TOP 3 GAGNANTS :\n");
    CellListe* p = LG.tete;
    for (int i = 1; i <= 3 && p != NULL; i++) {
        printf("%d. %s - %d victoires (total parties: %d)\n",
               i, p->info.nom, p->info.total_gagnes,
               p->info.total_gagnes + p->info.total_perdus);
        p = p->suiv;
    }
    if (LG.tete == NULL) {
        printf("Aucun gagnant\n");
    }
}

void afficherStatsStrategie1(Liste LG, Liste LP) {

    printf("STATISTIQUES STRATÉGIE 1\n");


    int victoires[4] = {0}; // indices 1-3
    int defaites[4] = {0};  // indices 1-3

    // Compter victoires
    CellListe* p = LG.tete;
    while (p != NULL) {
        if (p->info.total_gagnes >= 1 && p->info.total_gagnes <= 3) {
            victoires[p->info.total_gagnes]++;
        }
        p = p->suiv;
    }

    // Compter défaites
    p = LP.tete;
    while (p != NULL) {
        if (p->info.total_perdus >= 1 && p->info.total_perdus <= 3) {
            defaites[p->info.total_perdus]++;
        }
        p = p->suiv;
    }

    printf("\n Victoires :\n");
    for (int i = 1; i <= 3; i++) {
        printf("  %d victoire(s): ", i);
        if (victoires[i] > 0) {
            // Trouver les joueurs avec i victoires
            p = LG.tete;
            int first = 1;
            while (p != NULL) {
                if (p->info.total_gagnes == i) {
                    if (!first) printf(", ");
                    printf("%s", p->info.nom);
                    first = 0;
                }
                p = p->suiv;
            }
            printf("\n");
        } else {
            printf("aucun\n");
        }
    }

    printf("\n Défaites :\n");
    for (int i = 1; i <= 3; i++) {
        printf("  %d défaite(s): ", i);
        if (defaites[i] > 0) {
            p = LP.tete;
            int first = 1;
            while (p != NULL) {
                if (p->info.total_perdus == i) {
                    if (!first) printf(", ");
                    printf("%s", p->info.nom);
                    first = 0;
                }
                p = p->suiv;
            }
            printf("\n");
        } else {
            printf("aucun\n");
        }
    }
}

void afficherStatsStrategie2(Liste LG, Liste LP) {

    printf("STATISTIQUES STRATÉGIE 2\n");


    int victoires[3] = {0}; // indices 1-2
    int defaites[3] = {0};  // indices 1-2

    // Compter victoires stratégie 2
    CellListe* p = LG.tete;
    while (p != NULL) {
        if (p->info.total_gagnes >= 1 && p->info.total_gagnes <= 2) {
            victoires[p->info.total_gagnes]++;
        }
        p = p->suiv;
    }

    // Compter défaites stratégie 2
    p = LP.tete;
    while (p != NULL) {
        if (p->info.total_perdus >= 1 && p->info.total_perdus <= 2) {
            defaites[p->info.total_perdus]++;
        }
        p = p->suiv;
    }

    printf("\n Victoires (stratégie 2) :\n");
    for (int i = 1; i <= 2; i++) {
        printf("  %d victoire(s): ", i);
        if (victoires[i] > 0) {
            p = LG.tete;
            int first = 1;
            while (p != NULL) {
                if (p->info.total_gagnes == i) {
                    if (!first) printf(", ");
                    printf("%s", p->info.nom);
                    first = 0;
                }
                p = p->suiv;
            }
            printf("\n");
        } else {
            printf("aucun\n");
        }
    }

    printf("\n Défaites (stratégie 2) :\n");
    for (int i = 1; i <= 2; i++) {
        printf("  %d défaite(s): ", i);
        if (defaites[i] > 0) {
            p = LP.tete;
            int first = 1;
            while (p != NULL) {
                if (p->info.total_perdus == i) {
                    if (!first) printf(", ");
                    printf("%s", p->info.nom);
                    first = 0;
                }
                p = p->suiv;
            }
            printf("\n");
        } else {
            printf("aucun\n");
        }
    }
}

void afficherJoueursSansVictoire(Liste LP) {
    printf("\n Joueurs sans victoire :\n");
    CellListe* p = LP.tete;
    int aucun = 1;
    while (p != NULL) {
        if (p->info.total_gagnes == 0) {
            printf("- %s (défaites: %d)\n", p->info.nom, p->info.total_perdus);
            aucun = 0;
        }
        p = p->suiv;
    }
    if (aucun) printf("Aucun joueur sans victoire\n");
}

// MAIN
int main() {
    srand(time(NULL));

    // Variables pour horodatage
    time_t debutJeu, finJeu;
    time(&debutJeu);

    File F, F1, F3;
    Liste LG, LP;

    initFile(&F);
    initFile(&F1);
    initFile(&F3);
    initListe(&LG);
    initListe(&LP);

    // Création des joueurs
    Joueur joueurs[] = {
        {1, "Malak", 22, 0, 0, 0, 0, 0},
        {2, "Amina", 21, 0, 0, 0, 0, 0},
        {3, "Yacine", 23, 0, 0, 0, 0, 0},
        {4, "Lina", 20, 0, 0, 0, 0, 0},
        {5, "Lilyane", 24, 0, 0, 0, 0, 0},

    };

    int nbJoueurs = sizeof(joueurs) / sizeof(joueurs[0]);
    for (int i = 0; i < nbJoueurs; i++) {
        enfiler(&F, joueurs[i]);
    }

    printf("<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>\n");
    printf("                    DÉBUT DU JEU                            \n");
    printf("<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>\n");
    printf("Heure de début: %s", ctime(&debutJeu));
    printf("Nombre de joueurs initiaux: %d\n", nbJoueurs);
    printf("Joueurs: ");
    for (int i = 0; i < nbJoueurs; i++) {
        printf("%s", joueurs[i].nom);
        if (i < nbJoueurs - 1) printf(", ");
    }
    printf("\n<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>\n");

    int numPartie = 1;
    int strategie = 1;
    int maxPartiesStrategie1 = 3 * nbJoueurs;
    int maxPartiesTotal = 100;
    int partiesStrategie2 = 0;
    int maxPartiesStrategie2 = 2 * nbJoueurs;

    Joueur j1, j2;

    while (numPartie <= maxPartiesTotal) {
        time_t debutPartie;
        time(&debutPartie);

        // Vérifier changement de stratégie
        if (numPartie > maxPartiesStrategie1 && strategie == 1) {
            strategie = 2;
            printf("\n<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>\n");
            printf("                CHANGEMENT DE STRATÉGIE !                    \n");
            printf("<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>\n");
            printf("Partie %d → Passage à la Stratégie 2\n", numPartie);
            printf("Nouvelle règle: Gagnant → F1, Perdant → F3\n");
            printf("<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>\n");
        }

        // Sélectionner joueurs
        if (!selectionnerJoueurs(&F, &F1, &F3, &j1, &j2)) {
            // Plus assez de joueurs pour une partie
            if (!fileVide(&F3)) {
                defiler(&F3, &j1);
                insererListe(&LP, j1);
                printf("%s déclaré perdant (seul en F3) → LP\n", j1.nom);
            }

        }

        printf("\n<><><><><><><><><><><><><><><><><><><>\n");
        printf("PARTIE %d\n", numPartie);
        printf("<><><><><><><><><><><><><><><><><><><>\n");

        // Jouer la partie
        jouerPartie(&j1, &j2, strategie, debutPartie);

        time_t finPartie;
        time(&finPartie);
        printf("Heure fin: %s", ctime(&finPartie));
        printf("Durée partie: %.0f secondes\n", difftime(finPartie, debutPartie));


        if (strategie == 1) {  // STRATÉGIE 1

            if (j1.score_partie > j2.score_partie) {
                majResultats(&j1, &j2);
                placerJoueur(&F, &F1, &F3, j1, strategie);
                placerJoueur(&F, &F1, &F3, j2, strategie);

                // Vérifier règles spéciales
                if (j1.victoires_consecutives >= 5) {
                    insererListe(&LG, j1);
                    printf("%s → LG (5 victoires)\n", j1.nom);
                }
                if (j2.defaites_total >= 5) {
                    insererListe(&LP, j2);
                    printf("%s → LP (5 défaites)\n", j2.nom);
                }

            } else if (j2.score_partie > j1.score_partie) {
                majResultats(&j2, &j1);
                placerJoueur(&F, &F1, &F3, j2, strategie);
                placerJoueur(&F, &F1, &F3, j1, strategie);

                if (j2.victoires_consecutives >= 5) {
                    insererListe(&LG, j2);
                    printf("%s → LG (5 victoires)\n", j2.nom);
                }
                if (j1.defaites_total >= 5) {
                    insererListe(&LP, j1);
                    printf("%s → LP (5 défaites)\n", j1.nom);
                }

            } else {
                // Égalité
                enfiler(&F, j1);
                enfiler(&F, j2);
                printf("Égalité → %s et %s dans F\n", j1.nom, j2.nom);
            }

        } else { // STRATÉGIE 2

            partiesStrategie2++;

            // Vérifier limite stratégie 2
            if (partiesStrategie2 > maxPartiesStrategie2) {
                printf("\n<><><><><><><><><><><><><><><><><><>\n");
                printf("ARRÊT - Limite stratégie 2 atteinte\n");
                printf("%d parties jouées avec stratégie 2\n", maxPartiesStrategie2);
                printf("<><><><><><><><><><><><><><><><><><>\n");

                // Mettre F1 dans LG, F et F3 dans LP
                while (!fileVide(&F1)) {
                    defiler(&F1, &j1);
                    insererListe(&LG, j1);
                    printf("%s (F1) → LG\n", j1.nom);
                }
                while (!fileVide(&F)) {
                    defiler(&F, &j1);
                    insererListe(&LP, j1);
                    printf("%s (F) → LP\n", j1.nom);
                }
                while (!fileVide(&F3)) {
                    defiler(&F3, &j1);
                    insererListe(&LP, j1);
                    printf("%s (F3) → LP\n", j1.nom);
                }

            }

            if (j1.score_partie > j2.score_partie) {
                // j1 gagne
                j1.total_gagnes++;
                enfiler(&F1, j1);
                printf("%s → F1 (gagnant)\n", j1.nom);

                // j2 perd
                j2.total_perdus++;
                enfiler(&F3, j2);
                printf("%s → F3 (perdant)\n", j2.nom);

                // Vérifier 2 victoires
                if (j1.total_gagnes >= 2) {
                    insererListe(&LG, j1);
                    printf("%s → LG (2 victoires)\n", j1.nom);
                }
                if (j2.total_perdus >= 2) {
                    insererListe(&LP, j2);
                    printf("%s → LP (2 défaites)\n", j2.nom);
                }

            } else if (j2.score_partie > j1.score_partie) {
                // j2 gagne
                j2.total_gagnes++;
                enfiler(&F1, j2);
                printf("%s → F1 (gagnant)\n", j2.nom);

                // j1 perd
                j1.total_perdus++;
                enfiler(&F3, j1);
                printf("%s → F3 (perdant)\n", j1.nom);

                if (j2.total_gagnes >= 2) {
                    insererListe(&LG, j2);
                    printf("%s → LG (2 victoires)\n", j2.nom);
                }
                if (j1.total_perdus >= 2) {
                    insererListe(&LP, j1);
                    printf("%s → LP (2 défaites)\n", j1.nom);
                }

            } else {
                // Égalité
                enfiler(&F, j1);
                enfiler(&F, j2);
                printf("Égalité → %s et %s dans F\n", j1.nom, j2.nom);
            }
        }

        // Afficher état après partie
        printf("\n<><><><><><><><><><><><><><><><><><>\n");
        printf("ÉTAT APRÈS PARTIE %d\n", numPartie);
        printf("<><><><><><><><><><><><><><><><><><>\n");
        afficherFile(F, "File F (normale)");
        afficherFile(F1, "File F1 (priorité 1)");
        afficherFile(F3, "File F3 (priorité 3)");
        afficherListe(LG, "Liste LG (gagnants)");
        afficherListe(LP, "Liste LP (perdants)");
        printf("<><><><><><><><><><><><><><><><><><>\n");

        numPartie++;

        // Arrêt conditionnel
        if (fileVide(&F) && fileVide(&F1) && fileVide(&F3)) {
            printf("\n<><><><><><><><><><><><><><><><><><>\n");
            printf("FIN - Toutes les files sont vides\n");
            printf("<><><><><><><><><><><><><><><><><><>\n");
            break;
        }
    }

    time(&finJeu);

    // AFFICHAGE FINAL
    printf("\n<><><><><><><><><><><><><><><><><><><><><><><><><><><><><>\n");
    printf("                      FIN DU JEU                            \n");
    printf("<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>\n");
    printf("Heure de fin: %s", ctime(&finJeu));
    printf("Durée totale: %.0f secondes\n", difftime(finJeu, debutJeu));
    printf("Total parties jouées: %d\n", numPartie - 1);
    printf("Parties stratégie 1: %d\n", (numPartie <= maxPartiesStrategie1 + 1) ? numPartie - 1 : maxPartiesStrategie1);
    printf("Parties stratégie 2: %d\n", partiesStrategie2);
    printf("<><><><><><><><><><><><><><><><><><><><><><><><><><>\n");

    // Top 3 gagnants
    afficherTop3(LG);

    // Joueurs sans victoire
    afficherJoueursSansVictoire(LP);

    // Statistiques détaillées
    afficherStatsStrategie1(LG, LP);
    afficherStatsStrategie2(LG, LP);


    return 0;
}
