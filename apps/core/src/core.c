#include "core.h"
#include "log.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define _DBG()  printf("%s,%d\n", __func__, __LINE__)

/**
 * @brief Create a nerve
 */

nerve_t *create_nerve(void)
{
    nerve_t *new_nerve = (nerve_t  *)calloc(1, sizeof(nerve_t));
    if (!new_nerve)
    {
        LOGE("Create new nerve error!\n");
        return NULL;
    }

    return new_nerve;
}

/**
 * @brief Destory a nerve
 * 
 */
void destory_nerve(nerve_t **nerve)
{
    if (*nerve)
    {
        if ((*nerve)->zero)
            (*nerve)->zero->father = NULL;
        if ((*nerve)->one)
            (*nerve)->one->father = NULL;
        free(*nerve);
        *nerve = NULL;
    }
}

/**
 * @brief Connect nerve
 */
nerve_t *connect_nerve(nerve_t *father, int type, nerve_t *child)
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
            LOGW("Can not connect nerve\n");
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
            LOGW("Can not connect nerve\n");
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
nerve_t *traverse_nerve(nerve_t *nerve)
{
    if (nerve)
    { 
        traverse_nerve(nerve->one);
        traverse_nerve(nerve->zero);
        LOGI("%s\n", nerve->name);
    }
}


static void _print_nerve(nerve_t *nerve, int type,  int level)
{
	int i;

	if (!nerve)  return;

	_print_nerve(nerve->one, 2, level + 1);
	switch (type)
	{
	case 0:
		printf("%s\n", nerve->name);
		break;
	case 1:
		for (i = 0; i < level; i ++)	printf("\t");
		printf("\\ %s\n", nerve->name);
		break;
	case 2:
		for (i = 0; i < level; i ++)	printf("\t");
		printf("/ %s\n", nerve->name);
		break;	
	}
	_print_nerve(nerve->zero, 1,  level + 1);
}

/**
 * @brief 
 */
nerve_t *print_nerve(nerve_t *nerve)
{
    if (!nerve)  return NULL;

    _print_nerve(nerve, 0, 0);

    return nerve;
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

    // new->cell = nerve_create();
    // if (!new->cell)
    // {
    //     free(new);
    //     // free(new->dendrites);
    //     return NULL;
    // }

    // new->axon_rear = nerve_create();
    // if (!new->axon_rear)
    // {
    //     free(new);
    //     free(new->dendrites);
    //     free(new->cell);
    //     return NULL;
    // }

    // new->axon = nerve_create();
    // if (!new->axon)
    // {
    //     free(new);
    //     // free(new->dendrites);
    //     free(new->cell);
    //     // free(new->axon_rear);
    //     return NULL;
    // }

    // new->axon->next = new->axon_rear;
    // new->axon->previous = new->cell;
    // new->cell->next = new->axon;

    return new;
}

