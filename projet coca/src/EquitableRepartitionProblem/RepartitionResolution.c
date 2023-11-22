#include "RepartitionResolution.h"
#include <stdlib.h>
#include <stdio.h>
void dfs(RepartitionGraph graph, int player, int node, bool *visited) {
    visited[node] = true;

    for (int neighbor = 0; neighbor < rg_get_num_nodes(graph); neighbor++) {
        if (!visited[neighbor] &&
            rg_get_player_of_node_in_partition(graph, neighbor) == player &&
            rg_is_edge(graph, node, neighbor)) {
            dfs(graph, player, neighbor, visited);
        }
    }
}

// Fonction principale avec parcours en profondeur (DFS)
bool isconnexe(RepartitionGraph graph) {
    int num_nodes = rg_get_num_nodes(graph);
    int num_players = rg_get_num_players(graph);
     bool all_nodes_visited = true;
    for (int player = 0; player < num_players; player++) {
        bool is_player_connexe = false;  // Supposons d'abord que le sous-graphe n'est pas connexe

        // Recherchez un nœud attribué au joueur
        for (int node = 0; node < num_nodes; node++) { 
            if (rg_get_player_of_node_in_partition(graph, node) == player) {
                // Initialisez un tableau de booléens pour suivre les nœuds visités pendant DFS
                bool *visited = malloc(num_nodes * sizeof(bool));
                for (int i = 0; i < num_nodes; i++) {
                    visited[i] = false;
                }

                // Effectuez DFS à partir du nœud attribué au joueur
                dfs(graph, player, node, visited);

                // Vérifiez si tous les nœuds attribués au joueur ont été visités (sous-graphe connexe)
                bool all_nodes_visited = true;
                for (int i = 0; i < num_nodes; i++) {
                    if (rg_get_player_of_node_in_partition(graph, i) == player && !visited[i]) {
                        all_nodes_visited = false;
                        break;
                    }
                }

                // Libérez la mémoire allouée pour le tableau visited
                free(visited);

                if (all_nodes_visited) {
                    is_player_connexe = true;
                    break;  // Pas besoin de vérifier d'autres nœuds pour ce joueur
                }
            }
        }

        // Si le sous-graphe est connexe pour au moins un joueur, retournez true
        if (all_nodes_visited==false) {
            return false;
        }
    }

    // Aucun sous-graphe connexe trouvé pour aucun joueur
    return true;
}

bool is_equitable(RepartitionGraph graph) {
    int num_nodes = rg_get_num_nodes(graph);
    int num_players = rg_get_num_players(graph);
    
    int total_weight = rg_get_total_weights(graph);  // Obtenez le poids total

    // Parcourez chaque joueur et vérifiez si sa part est équitable
    for (int player = 0; player < num_players; player++) {
        int player_weight = 0;

        // Parcourez chaque nœud attribué au joueur
        for (int node = 0; node < num_nodes; node++) {
            if (rg_get_player_of_node_in_partition(graph, node) == player) {
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

bool repartition_brute_force_recursive(RepartitionGraph graph, int node) {
    int num_players = rg_get_num_players(graph);
    
    // Si nous avons attribué tous les nœuds, vérifions si l'allocation est valide et équitable
    if (node == rg_get_num_nodes(graph)) {
        return isconnexe(graph) && is_equitable(graph);
    }
    
    // Essayez d'attribuer le nœud actuel à chaque joueur
    for (int player = 0; player < num_players; player++) {
        // Attribuer le nœud au joueur
        rg_set_player_of_node_partition(graph, node,player);
       
        
        // Appeler récursivement pour le nœud suivant
        if (repartition_brute_force_recursive(graph, node + 1)) {
            return true;  // Si une solution est trouvée, retournez true
        }
        
        // Si une solution n'a pas été trouvée, réinitialisez l'attribution
        rg_set_player_of_node_partition(graph, node,-1);
    }
    
    return false;  // Aucune solution trouvée
}

bool repartition_brute_force(RepartitionGraph graph)
{
    printf("Brute Force not implemented\n");
    return   repartition_brute_force_recursive(graph, 0);
   
}
