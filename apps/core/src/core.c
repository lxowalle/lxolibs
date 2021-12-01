#include "core.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define _DBG()  printf("%s,%d\n", __func__, __LINE__)
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

    memset(new, NULL, sizeof(neurons_t));

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

