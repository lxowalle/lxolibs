#ifndef __CORE_H
#define __CORE_H

typedef struct nerve nerve_t;
typedef struct neurons neurons_t;

struct neurons_ops{
    void (*init)(neurons_t *param);
    void (*exit)(neurons_t *param);
};

struct nerve{
    int type;
    struct nerve *next, *previous;
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

#endif // __CORE_H