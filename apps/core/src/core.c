#include "core.h"
#include "log.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

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
        for (i = 0; i < level; i ++)	printf("\t");
		printf("@%s\n", nerve->name);
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

/**
 * @brief Create a neuron
 */
neuron_t *neuron_create(neuron_type_t type, ...)
{
    neuron_t *new_neuron;
    switch (type)
    {
    case NEURONS_TYPE_PSEUDOUNIPOLAR:
    {
        new_neuron = (neuron_t *)calloc(1, sizeof(neuron_pseudounipolar_t));
        if (!new_neuron)   return NULL;

        new_neuron->type = NEURONS_TYPE_PSEUDOUNIPOLAR;
        break;
    }
    case NEURONS_TYPE_BIPOLAR:
    {
        new_neuron = (neuron_t *)calloc(1, sizeof(neuron_bipolar_t));
        if (!new_neuron)   return NULL;

        new_neuron->type = NEURONS_TYPE_BIPOLAR;
        break;
    }
    case NEURONS_TYPE_MULTIPOLAR:
    {
        new_neuron = (neuron_t *)calloc(1, sizeof(neuron_multipolar_t));
        if (!new_neuron)   return NULL;

        new_neuron->type = NEURONS_TYPE_MULTIPOLAR;
        break;
    }
    default:
        LOGW("Can not create neuron, isn't a valid type\n");
        break;
    }

    return new_neuron;
}

/**
 * @brief Destory a neuron
 */
void neuron_destory(neuron_t **neuron)
{
    if (*neuron)
    {
        free(*neuron);
        *neuron = NULL;
    }
}

/**
 * @brief Connect nerve axon to neuron 
 */
static void _neuron_connect_axon(neuron_t *father, nerve_t *child)
{
    switch (father->type)
    {
    case NEURONS_TYPE_PSEUDOUNIPOLAR:
    {
        neuron_pseudounipolar_t *n = (neuron_pseudounipolar_t *)father;
        if (!n->axon)
        {
            n->axon = child;
        }
        break;
    }
    case NEURONS_TYPE_BIPOLAR:
    {
        neuron_bipolar_t *n = (neuron_bipolar_t *)father;
        if (!n->axon)
        {
            n->axon = child;
        }
        break;
    }
    case NEURONS_TYPE_MULTIPOLAR:
    {
        neuron_multipolar_t *n = (neuron_multipolar_t *)father;
        if (!n->axon)
        {
            n->axon = child;
        }
        break;
    }
    default:
        LOGW("Can not connect neuron, isn't a valid type\n");
        break;
    }
}

/**
 * @brief Connect nerve dendrites to neuron 
 */
static void _neuron_connect_dendrites(neuron_t *father, nerve_t *child)
{
    switch (father->type)
    {
    case NEURONS_TYPE_BIPOLAR:
    {
        neuron_bipolar_t *n = (neuron_bipolar_t *)father;
        if (!n->dendrites)
        {
            n->dendrites = child;
        }
        break;
    }
    case NEURONS_TYPE_MULTIPOLAR:
    {
        neuron_multipolar_t *n = (neuron_multipolar_t *)father;
        if (!n->dendrites)
        {
            n->dendrites = child;
        }
        break;
    }
    default:
        LOGW("Can not connect neuron, isn't a valid type\n");
        break;
    }
}

/**
 * @brief Connect surround axon to neuron 
 */
static void _neuron_connect_surround(neuron_t *father, nerve_t *child)
{
    switch (father->type)
    {
    case NEURONS_TYPE_PSEUDOUNIPOLAR:
    {
        neuron_pseudounipolar_t *n = (neuron_pseudounipolar_t *)father;
        if (!n->surround)
        {
            n->surround = child;
        }
        break;
    }
    default:
        LOGW("Can not connect neuron, isn't a valid type\n");
        break;
    }
}

int neuron_control(neuron_t *neuron, neuron_control_cmd_t cmd, ...)
{
    if (!neuron) return -1;

    va_list ap;
    va_start(ap, cmd);

    switch (cmd)
    {
    case NEURONS_CTLSET_CONNECTAXON:
    {
        neuron_t *father = (neuron_t *)neuron;
        nerve_t *child = va_arg(ap, void *);

        _neuron_connect_axon(father, child);
        break;
    }
    case NEURONS_CTLSET_CONNECTSURROUND:
    {
        neuron_t *father = (neuron_t *)neuron;
        nerve_t *child = va_arg(ap, void *);

        _neuron_connect_surround(father, child);
        break;
    }
    case NEURONS_CTLSET_CONNECTDENDRITES:
    {
        neuron_t *father = (neuron_t *)neuron;
        nerve_t *child = va_arg(ap, void *);

        _neuron_connect_dendrites(father, child);
        break;
    }
    default:break;
    }

    va_end(ap);
    return 0;
}

/**
 * @brief Print neuron 
 */
neuron_t *print_neuron(neuron_t *neuron)
{
    if (!neuron)    return NULL;

    switch (neuron->type)
    {
    case NEURONS_TYPE_PSEUDOUNIPOLAR:
    {
        neuron_pseudounipolar_t *n = (neuron_pseudounipolar_t *)neuron;
        
        if (n->axon)
        {
            _print_nerve(n->axon, 0, 1);
        }
        printf(" __   |\n");
        printf("|__|__|\n");
        printf("      |\n");
        if (n->surround)
        {
            _print_nerve(n->surround, 0, 1);
        }
        break;
    }
    case NEURONS_TYPE_BIPOLAR:
    {

        break;
    }
    case NEURONS_TYPE_MULTIPOLAR:
    {

        break;
    }
    default:
        LOGW("Can not print neuron, isn't a valid type\n");
        break;
    }


    return neuron;
}