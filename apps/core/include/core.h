#ifndef __CORE_H
#define __CORE_H

typedef struct neurons neurons_t;

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

struct neurons_ops{
    void (*init)(neurons_t *param);
    void (*exit)(neurons_t *param);
};

struct neurons{
    /* data */
    struct neurons_ops *ops;
    struct nerve *cell;
    // struct nerve *dendrites;
    struct nerve *axon;
    // struct nerve *axon_rear;
};

neurons_t *neurons_create(void);

nerve_t *create_nerve(void);
void destory_nerve(nerve_t **nerve);
nerve_t *connect_nerve(nerve_t *father, int type, nerve_t *child);
nerve_t *traverse_nerve(nerve_t *nerve);
nerve_t *print_nerve(nerve_t *nerve);

#endif // __CORE_H