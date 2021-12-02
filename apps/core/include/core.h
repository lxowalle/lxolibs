#ifndef __CORE_H
#define __CORE_H

typedef struct nerve nerve_t;
typedef struct neurons neurons_t;

/**
 * @brief tree
 * 
 */
typedef struct tree
{
    int base;
    char name[15];
    int type;
    struct tree *father, *zero, *one;
}tree_t;

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

tree_t *create_tree(void);
void destory_tree(tree_t **tree);
tree_t *connect_tree(tree_t *father, int type, tree_t *child);
tree_t *traverse_tree(tree_t *tree);
tree_t *print_tree(tree_t *tree);

#endif // __CORE_H