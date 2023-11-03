#include "RepartitionGraph.h"
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>


bool repartition_brute_force(RepartitionGraph graph)
{
    int numNodes = rg_get_num_nodes(graph);
    int numPlayers = rg_get_num_players(graph);

    // Tableau pour stocker la meilleure allocation trouvée
    int *bestAllocation = (int *)malloc(numNodes * sizeof(int));

    // Initialisation de l'allocation avec une allocation invalide
    for (int i = 0; i < numNodes; i++)
    {
        bestAllocation[i] = -1;
    }

    // Appel récursif pour explorer toutes les allocations possibles
    bool foundAllocation = findValidEquitableAllocation(graph, 0, bestAllocation);

    // Affichage de l'allocation trouvée (s'il y en a une)
    if (foundAllocation)
    {
        printf("Valid and Equitable Allocation: ");
        for (int i = 0; i < numNodes; i++)
        {
            printf("%d ", bestAllocation[i]);
        }
        printf("\n");
    }
    else
    {
        printf("No valid and equitable allocation found.\n");
    }

    free(bestAllocation);
    return foundAllocation;
}

bool findValidEquitableAllocation(RepartitionGraph graph, int nodeIndex, int *currentAllocation)
{
    int numNodes = rg_get_num_nodes(graph);
    int numPlayers = rg_get_num_players(graph);

    // Si nous avons atteint le dernier nœud, vérifions si l'allocation actuelle est valide et équitable
    if (nodeIndex == numNodes)
    {
        bool valid = isValidAllocation(graph, currentAllocation);
        bool equitable = isEquitableAllocation(graph, currentAllocation);
        return valid && equitable;
    }

    // Essayer toutes les affectations possibles pour le nœud actuel
    for (int player = 0; player < numPlayers; player++)
    {
        currentAllocation[nodeIndex] = player;
        if (findValidEquitableAllocation(graph, nodeIndex + 1, currentAllocation))
        {
            return true; // Une allocation valide et équitable a été trouvée
        }
    }

    return false; // Aucune allocation valide et équitable n'a été trouvée pour ce nœud
}

bool isValidAllocation(RepartitionGraph graph, int *allocation)
{
    // Vérifier la validité de l'allocation (chaque sous-graphe doit être connexe)
    // Vous devez implémenter cette vérification en fonction de votre représentation du graphe.

    return true; // Retourne true si l'allocation est valide, sinon false.
}

#include <stdio.h>
#include <stdbool.h>

// Structure de données pour la répartition (vous devrez remplir avec vos données)
typedef struct RepartitionGraph_s {
    // ...
} RepartitionGraph_s;

// Fonction pour vérifier l'équité d'une allocation
bool is_equitable(RepartitionGraph_s *graph) {
    int num_nodes = graph_num_nodes(graph->graph);
    int num_players = graph->num_players;
    
    int total_weight = rg_get_total_weights(graph);  // Obtenez le poids total

    // Parcourez chaque joueur et vérifiez si sa part est équitable
    for (int player = 0; player < num_players; player++) {
        int player_weight = 0;

        // Parcourez chaque nœud attribué au joueur
        for (int node = 0; node < num_nodes; node++) {
            if (graph->partition[node] == player) {
                // Ajoutez le poids du nœud attribué au joueur
                player_weight += rg_get_weight(graph, node, player);
            }
        }

        // Vérifiez si la part du joueur est équitable
        if (player_weight < total_weight / num_players) {
            return false;  // La part n'est pas équitable pour ce joueur
        }
    }

    return true;  // Toutes les parts sont équitables
}


#include <stdio.h>
#include <stdbool.h>

// Structure de données pour la répartition (vous devrez remplir avec vos données)
typedef struct RepartitionGraph_s {
    // ...
} RepartitionGraph_s;

// Fonction pour vérifier si une répartition est connexe
bool isconnexe(RepartitionGraph_s *graph) {
    int num_nodes = rg_get_num_nodes(graph);
    int num_players = rg_get_num_players(graph);

    for (int player = 0; player < num_players; player++) {
        bool is_player_connexe = true;  // Supposons d'abord que le sous-graphe est connexe

        for (int node = 0; node < num_nodes; node++) {
            if (rg_get_player_of_node_in_partition(graph, node) == player) {
                for (int neighbor = 0; neighbor < num_nodes; neighbor++) {
                    if (rg_get_player_of_node_in_partition(graph, neighbor) == player &&
                        rg_is_edge(graph, node, neighbor)) {
                        // Le nœud et son voisin sont attribués au même joueur et sont connectés
                        break;
                    }

                    if (neighbor == num_nodes - 1) {
                        // Le nœud n'a pas de voisin attribué au même joueur
                        is_player_connexe = false;
                    }
                }

                if (!is_player_connexe) {
                    // Le sous-graphe n'est pas connexe pour ce joueur
                    return false;
                }
            }
        }
    }

    return true;  // Tous les sous-graphes sont connexes
}

#include <stdio.h>
#include <stdbool.h>

// Structure de données pour la répartition (vous devrez remplir avec vos données)
typedef struct RepartitionGraph_s {
    // ...
} RepartitionGraph_s;

// Fonction pour vérifier si un graphe est connexe
bool isconnexe(RepartitionGraph_s *graph);

// Fonction pour vérifier si une allocation est équitable
bool is_equitable(RepartitionGraph_s *graph);

// Fonction récursive pour l'algorithme de force brute
bool repartition_brute_force_recursive(RepartitionGraph_s *graph, int node) {
    int num_players = graph->num_players;
    
    // Si nous avons attribué tous les nœuds, vérifions si l'allocation est valide et équitable
    if (node == rg_get_num_nodes(graph)) {
        return isconnexe(graph) && is_equitable(graph);
    }
    
    // Essayez d'attribuer le nœud actuel à chaque joueur
    for (int player = 0; player < num_players; player++) {
        // Attribuer le nœud au joueur
        graph->partition[node] = player;
        
        // Appeler récursivement pour le nœud suivant
        if (repartition_brute_force_recursive(graph, node + 1)) {
            return true;  // Si une solution est trouvée, retournez true
        }
        
        // Si une solution n'a pas été trouvée, réinitialisez l'attribution
        graph->partition[node] = -1;
    }
    
    return false;  // Aucune solution trouvée
}

// Fonction principale pour l'algorithme de force brute
bool repartition_brute_force(RepartitionGraph_s *graph) {
    // Initialisation de la partition à -1 (aucun joueur attribué)
    int num_nodes = rg_get_num_nodes(graph);
    for (int node = 0; node < num_nodes; node++) {
        graph->partition[node] = -1;
    }
    
    // Appel récursif pour trouver une solution
    return repartition_brute_force_recursive(graph, 0);
}

int main() {
    // Créez votre structure RepartitionGraph_s avec les données appropriées
    RepartitionGraph_s *graph = create_repartition_graph();
    
    // Appelez l'algorithme de force brute
    if (repartition_brute_force(graph)) {
        printf("Solution trouvée !\n");
        // Affichez la répartition trouvée ici
    } else {
        printf("Aucune solution trouvée.\n");
    }
    
    // Libérez la mémoire et effectuez d'autres opérations de nettoyage si nécessaire
    // Libérez la mémoire allouée pour la structure graph
    // ...

    return 0;
}
