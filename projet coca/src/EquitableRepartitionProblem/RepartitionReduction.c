#include "RepartitionReduction.h"
#include "Z3Tools.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

/**
 * @brief Returns the variable V_{@p node}^{@p player} from the subject that is true if @p node is attributed to @p player.
 *
 * @param ctx The solver context.
 * @param node A node.
 * @param player A player.
 * @return Z3_ast The variable.
 */
//vij
Z3_ast variable_node_associated_to_player(Z3_context ctx, int node, int player)
{
    char name[40];
    snprintf(name, 40, "V[%d:%d]", node, player);
    return mk_bool_var(ctx, name);
}

/**
 * @brief The varible C_{@p node1,@p node2,@p limit}^{@p player} true if there is a path from @p node1 to @p node2 with nodes between 0 and @p limit with nodes that are attributed to @p player.
 *
 * @param ctx The solver context.
 * @param node1 A node.
 * @param node2 A node.
 * @param limit A node.
 * @param player A player.
 * @return Z3_ast The variable.
 */
Z3_ast variable_floyd_warshall(Z3_context ctx, int node1, int node2, int limit, int player)
{

    char name[40];
    snprintf(name, 40, "C[%d-%d:%d;%d]", node1, node2, limit, player);
    return mk_bool_var(ctx, name);
}

/**
 * @brief The variable M_{@p node,@p position}^{@p player} from the subject, expressing that @p node is at @p position in the partition of @p player.
 *
 * @param ctx The solver context.
 * @param node A node.
 * @param position A position.
 * @param player A player.
 * @return Z3_ast The variable.
 */
Z3_ast variable_count(Z3_context ctx, int node, int position, int player)
{
    char name[40];
    snprintf(name, 40, "M[%d,%d - %d]", node, position, player);
    return mk_bool_var(ctx, name);
}
Z3_ast un_sommet_du_graphe_est_alloue_a_au_moins_u_participant (Z3_context ctx, int node , int num_players)
{
    Z3_ast tabvar[num_players];

    for (int player = 0; player < num_players; player++)
    {
        Z3_ast var = variable_node_associated_to_player(ctx, player, player);
        tabvar[player] = var;
    }

    Z3_ast result = Z3_mk_or(ctx, num_players, tabvar);

   // free(tabvar);

    return result;
}

Z3_ast chaque_sommet_a_et_alloue_a_au_moins_un_participant(Z3_context ctx, int num_nodes , int num_players){
  
    Z3_ast tabvar[num_nodes];
    
    

    for (int node = 0; node < num_nodes; node++)
    {
       
            tabvar[node]=un_sommet_du_graphe_est_alloue_a_au_moins_u_participant (ctx,node ,num_players);
    }

     Z3_ast result = Z3_mk_and(ctx, num_nodes, tabvar);
    return result;
}
Z3_ast sommet_est_alloue_a_au_plus_un_participant(Z3_context ctx, int node , int num_players)
{
    Z3_ast tabvar[num_players];

    
    for (int player = 0; player < num_players; player++)
    {
        Z3_ast var = variable_node_associated_to_player(ctx, node, player);
        Z3_ast negated_var = Z3_mk_not(ctx, var);
        tabvar[player] = negated_var;
    }

    Z3_ast result = Z3_mk_or(ctx, num_players, tabvar);

   // free(tabvar);

    return result;
}
Z3_ast is_partition(Z3_context ctx, int num_nodes, int num_players)
{
    Z3_ast condition1 = chaque_sommet_a_et_alloue_a_au_moins_un_participant(ctx, num_nodes, num_players);

    Z3_ast* conjunctions = (Z3_ast*)malloc(num_nodes * sizeof(Z3_ast));

    for (int node = 0; node < num_nodes; node++)
    {
        Z3_ast condition2 = sommet_est_alloue_a_au_plus_un_participant(ctx, node, num_players);
        conjunctions[node] = condition2;
    }

    Z3_ast condition2_combined = Z3_mk_and(ctx, num_nodes, conjunctions);

    Z3_ast result = Z3_mk_and(ctx, 2, (Z3_ast[]){condition1, condition2_combined});

    free(conjunctions);

    return result;
}

Z3_ast implication_connexe(Z3_context ctx, int node, int nodeprime, int players, int limite) {
    Z3_ast tabvar[2];
    // Assuming player is a variable of type int
    tabvar[0] = variable_node_associated_to_player(ctx, node, players);
    tabvar[1] = variable_node_associated_to_player(ctx, nodeprime, players);
    Z3_ast C_varforchal = variable_floyd_warshall(ctx, node, nodeprime, limite, players);
    Z3_ast tabvars = Z3_mk_and(ctx, 2, (Z3_ast[]){tabvar[0], tabvar[1]});
    return Z3_mk_implies(ctx, tabvars, C_varforchal);
}

Z3_ast  is_arrete_existe (Z3_context ctx, int node, int nodeprime, int players, int limite ,const RepartitionGraph graph){
  
   if(rg_is_edge(graph, node, nodeprime)==true){

       return implication_connexe(ctx, node ,nodeprime, players,limite); 
   }

       Z3_ast tabvar[2];
       tabvar[0] = variable_node_associated_to_player(ctx, node, players);
       tabvar[1] = variable_node_associated_to_player(ctx, nodeprime, players);
       Z3_ast tabvars = Z3_mk_and(ctx, 2, (Z3_ast[]){tabvar[0], tabvar[1]});
       Z3_ast negated_var = Z3_mk_not(ctx, tabvars);
       return  negated_var;
}

Z3_ast isconnexeR(Z3_context ctx,int num_nodes,int num_players,const RepartitionGraph graph){
     
    Z3_ast tabvard[num_players*(num_nodes*(num_nodes*num_nodes))];
    int i=0;
    Z3_ast tabvarcs;
    Z3_ast tabvarc[2];
    for (int player = 0; player < num_players; player++){
        for (int level = 0; level < num_nodes; level++)
        {
            for (int n1 = 0; n1 < num_nodes; n1++){
                for (int n2 = 0; n2 < num_nodes; n2++)
                {
                  if(n1!=level && n2!=level){
                       tabvard[i]= is_arrete_existe(ctx, n1 ,n2, player,level-1,graph);
                        i++;
                  }
                  else{
                      if(n1==level && n2==level){
                         tabvarc[0] = is_arrete_existe(ctx, n1 ,level, player,level-1,graph);
                         tabvarc[1] = is_arrete_existe(ctx, level ,n2, player,level-1,graph);
                         tabvarcs = Z3_mk_and(ctx, 2, (Z3_ast[]){tabvarc[0], tabvarc[1]});
                          tabvard[i]=tabvarcs;
                          i++;
                      } 
                  }
                 }   
            }
            }
    }

    return   Z3_mk_or(ctx, i, tabvard);
}






Z3_ast repartition_reduction(Z3_context ctx, const RepartitionGraph graph)
{
    int num_players = rg_get_num_players(graph);
    int num_nodes=  rg_get_num_nodes(graph);
     Z3_ast tabF[2];
     tabF[0]= is_partition(ctx,num_nodes, num_players);
     tabF[1] =  isconnexeR(ctx,num_nodes,num_players,graph);
   
    return   Z3_mk_and(ctx, 2, (Z3_ast[]){tabF[0], tabF[1]});;
   
}

void repartition_set_partition_from_model(Z3_context ctx, Z3_model model, RepartitionGraph graph)
{
    int num_nodes = rg_get_num_nodes(graph);
    int num_players = rg_get_num_players(graph);
    for (int node = 0; node < num_nodes; node++)
    {
        for (int player = 0; player < num_players; player++)
        {
            if (value_of_var_in_model(ctx, model, variable_node_associated_to_player(ctx, node, player)))
                rg_set_player_of_node_partition(graph, node, player);
        }
    }
}

void repartition_print_model(Z3_context ctx, Z3_model model, RepartitionGraph graph)
{
    int num_nodes = rg_get_num_nodes(graph);
    int num_players = rg_get_num_players(graph);
    int max_pos = (rg_get_total_weights(graph) - 1) / num_players + 1;
    for (int node = 0; node < num_nodes; node++)
        for (int player = 0; player < num_players; player++)
            for (int pos = 0; pos < max_pos; pos++)
            {
                printf("M[%d,%d - %d] = %d\n", node, pos, player, value_of_var_in_model(ctx, model, variable_count(ctx, node, pos, player)));
            }
    for (int player = 0; player < num_players; player++)
        for (int level = 0; level < num_nodes; level++)
        {
            for (int n1 = 0; n1 < num_nodes; n1++)
                for (int n2 = 0; n2 < num_nodes; n2++)
                {
                    printf("C[%d-%d:%d;%d] = %d\n", n1, n2, level, player, value_of_var_in_model(ctx, model, variable_floyd_warshall(ctx, n1, n2, level, player)));
                }
        }
    for (int node = 0; node < num_nodes; node++)
        for (int player = 0; player < num_players; player++)
            printf("V[%d:%d] = %d\n", node, player, value_of_var_in_model(ctx, model, variable_node_associated_to_player(ctx, node, player)));
}
