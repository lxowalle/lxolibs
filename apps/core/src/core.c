#include "core.h"
#include "log.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define _DBG()  printf("%s,%d\n", __func__, __LINE__)

/**
 * @brief Create a tree
 */

tree_t *create_tree(void)
{
    tree_t *new_tree = (tree_t  *)calloc(1, sizeof(tree_t));
    if (!new_tree)
    {
        LOGE("Create new tree error!\n");
        return NULL;
    }

    return new_tree;
}

/**
 * @brief Destory a tree
 * 
 */
void destory_tree(tree_t **tree)
{
    if (*tree)
    {
        if ((*tree)->zero != NULL)
            (*tree)->zero->father = NULL;
        if ((*tree)->one)
            (*tree)->one->father = NULL;
        free(*tree);
        *tree = NULL;
    }
}

/**
 * @brief Connect tree
 */
tree_t *connect_tree(tree_t *father, int type, tree_t *child)
{
    if (!father || !child)  
    {
        LOGW("Father or child is NULL\n"); 
        return NULL;
    }

    if (type)
    {
        if (father->one)
        {
            LOGW("Can not connect tree\n");
            return father;
        }

        father->one = child;
        child->father = father;
        child->type = 1;
    }
    else
    {
        if (father->zero)
        {
            LOGW("Can not connect tree\n");
            return father;
        }

        father->zero = child;
        child->father = father;
        child->type = 0;
    }

    return father;
}

/**
 * @brief 
 * 
 */
tree_t *traverse_tree(tree_t *tree)
{
    if (tree)
    { 
        traverse_tree(tree->one);
        traverse_tree(tree->zero);
        LOGI("%s\n", tree->name);
    }
}


static void _print_tree(tree_t *tree, int type,  int level)
{
	int i;

	if (!tree)  return;

	_print_tree(tree->one, 2, level + 1);
	switch (type)
	{
	case 0:
		printf("%s\n", tree->name);
		break;
	case 1:
		for (i = 0; i < level; i ++)	printf("\t");
		printf("\\ %s\n", tree->name);
		break;
	case 2:
		for (i = 0; i < level; i ++)	printf("\t");
		printf("/ %s\n", tree->name);
		break;	
	}
	_print_tree(tree->zero, 1,  level + 1);
}

/**
 * @brief 
 */
tree_t *print_tree(tree_t *tree)
{
    if (!tree)  return NULL;

    _print_tree(tree, 0, 0);

    return tree;
}

/**
 * @brief
 * @details
 * 
 * _|
 *   \__   _|/              \|_
 *   |__|___|_______________/
 * _/                       \__
 *  |                       |  
 * 
 * 
 * _|           _|/
 *   \   __      |              \|_
 *      |__|________________    /
 * _/                           \__
 *  |                           |  
*/
nerve_t *nerve_create(void)
{
    nerve_t *new = (nerve_t *)malloc(sizeof(nerve_t));
    if (!new)   return NULL;

    new->type = 0xFF;

    return new;
}

int nerve_add(nerve_t *dst, nerve_t *new)
{
    if (!dst || !new)   return -1;
}

neurons_t *neurons_create(void)
{
    neurons_t *new = (neurons_t *)malloc(sizeof(neurons_t));
    if (!new)   return NULL;

    memset(new, 0, sizeof(neurons_t));

    // new->dendrites = nerve_create();
    // if (!new->dendrites)
    // {
    //     free(new);
    //     return NULL;
    // }

    new->cell = nerve_create();
    if (!new->cell)
    {
        free(new);
        // free(new->dendrites);
        return NULL;
    }

    // new->axon_rear = nerve_create();
    // if (!new->axon_rear)
    // {
    //     free(new);
    //     free(new->dendrites);
    //     free(new->cell);
    //     return NULL;
    // }

    new->axon = nerve_create();
    if (!new->axon)
    {
        free(new);
        // free(new->dendrites);
        free(new->cell);
        // free(new->axon_rear);
        return NULL;
    }

    // new->axon->next = new->axon_rear;
    new->axon->previous = new->cell;
    new->cell->next = new->axon;

    return new;
}

