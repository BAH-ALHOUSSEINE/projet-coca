#include "RepartitionResolution.h"
#include <stdlib.h>
#include <stdio.h>


#include <stdbool.h>

// Fonction auxiliaire pour le parcours en profondeur (DFS) afin de trouver une composante connexe
void dfs(int node, int player, RepartitionGraph graph, bool *visited) {
    visited[node] = true;
    for (int neighbor = 0; neighbor < rg_get_num_nodes(graph); neighbor++) {
        if (!visited[neighbor] && rg_get_player_of_node_in_partition(graph, neighbor) == player &&
            rg_is_edge(graph, node, neighbor)) {
            dfs(neighbor, player, graph, visited);
        }
    }
}

// Fonction vérifiant si chaque joueur a une composante connexe
bool isconnexe(RepartitionGraph graph) {
    int num_nodes = rg_get_num_nodes(graph);
    int num_players = rg_get_num_players(graph);

    for (int player = 0; player < num_players; player++) {
        bool *visited = (bool *)malloc(num_nodes * sizeof(bool));
        for (int i = 0; i < num_nodes; i++) {
            visited[i] = false;
        }

        // Trouver le premier nœud associé au joueur
        int start_node = -1;
        for (int node = 0; node < num_nodes; node++) {
            if (rg_get_player_of_node_in_partition(graph, node) == player) {
                start_node = node;
                break;
            }
        }

        if (start_node == -1) {
            free(visited);
            return false; // Aucun nœud associé au joueur, la composante connexe est vide.
        }

        dfs(start_node, player, graph, visited);

        // Vérifier si tous les nœuds associés au joueur ont été visités
        for (int node = 0; node < num_nodes; node++) {
            if (rg_get_player_of_node_in_partition(graph, node) == player && !visited[node]) {
                free(visited);
                return false; // Il existe une composante connexe incomplète pour ce joueur.
            }
        }

        free(visited);
    }

    return true; // Chaque joueur a une composante connexe.
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
    
    return   repartition_brute_force_recursive(graph, 0);
   
}
