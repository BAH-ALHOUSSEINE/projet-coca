#include "RepartitionResolution.h"
#include <stdlib.h>
#include <stdio.h>


bool isconnexe(RepartitionGraph graph) {
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
    /*À remplacer par votre implémentation du brute-force.
    Cette fonction doit probablement simplement appeler une fonction récursive construisant pas à pas une partition, puis appelant un vérificateur sur chaque partition complète.
    Votre vérificateur devrait être séparé de cette fonction récursive (sauf si vous faites des vérifications sur des partitions partielles, mais cela me semble peu adéquat ici). Votre vérificateur devant vérifier deux propriétés différentes, il vous est conseillé de faire une fonction pour chacune de ces propriétés.
    Les fonctions de RepartitionGraph.h utilisées dans notre solution de cette résolution sont les suivantes :
    rg_get_num_nodes, rg_get_num_players, rg_is_edge, rg_get_player_of_node_in_partition, rg_set_player_of_node_partition.
    Si vous sentez le besoin de fonctions qui ne sont pas dans cette liste, vous êtes potentiellement en train de vous planter (rg_reset_partition pouvant cependant être utile, mais on peut faire sans).
    Notre implémentation (sans ce bloc de commentaire), porte ce fichier à 94 lignes. Cette valeur n’est évidemment qu’une indication, mais si vous dépassez les 300 lignes, il est probable que vous vous compliquiez la vie.
    */
}
