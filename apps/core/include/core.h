#ifndef __CORE_H
#define __CORE_H

typedef struct neuron neuron_t;

/**
 * @brief 
 */
typedef enum nerve_type
{
    NERVE_TYPE_NONE = 0,
    NERVE_TYPE_ZERO,
    NERVE_TYPE_ONE,
}nerve_type_t;

/**
 * @brief nerve
 * 
 */
typedef struct nerve
{
    nerve_type_t type;
    struct nerve *father, *zero, *one;
    char name[15];
    void *param;
}nerve_t;

/**
 * @brief 
 */
typedef enum neuron_type
{
    NEURONS_TYPE_NONE = 0,
    NEURONS_TYPE_PSEUDOUNIPOLAR,
    NEURONS_TYPE_BIPOLAR,
    NEURONS_TYPE_MULTIPOLAR,
}neuron_type_t;

/**
 * @brief control enum
 */
typedef enum neuron_control_cmd
{
    NEURONS_CTLSET_CONNECTAXON = 0,
    NEURONS_CTLSET_CONNECTSURROUND,
    NEURONS_CTLSET_CONNECTDENDRITES
}neuron_control_cmd_t;

typedef struct neuron_ops
{
    void (*init)(neuron_t *param);
    void (*exit)(neuron_t *param);
}neuron_ops_t;

typedef struct neuron_pseudounipolar
{
    neuron_t *n;
    nerve_t *cell;
    nerve_t *axon;
    nerve_t *surround;
}neuron_pseudounipolar_t;

typedef struct neuron_bipolar
{
    neuron_t *n;
    nerve_t *cell;
    nerve_t *axon;
    nerve_t *dendrites;
}neuron_bipolar_t;

typedef struct neuron_multipolar
{
    neuron_t *n;
    nerve_t *cell;
    nerve_t *axon;
    nerve_t *dendrites;
    int dendrite_num;
}neuron_multipolar_t;

struct neuron{
    neuron_type_t type;
    neuron_ops_t ops;
};

neuron_t *neuron_create(neuron_type_t type, ...);
void neuron_destory(neuron_t **neuron);
neuron_t *print_neuron(neuron_t *neuron);
int neuron_control(neuron_t *neuron, neuron_control_cmd_t cmd, ...);

nerve_t *create_nerve(void);
void destory_nerve(nerve_t **nerve);
nerve_t *connect_nerve(nerve_t *father, int type, nerve_t *child);
nerve_t *traverse_nerve(nerve_t *nerve);
nerve_t *print_nerve(nerve_t *nerve);

#endif // __CORE_H