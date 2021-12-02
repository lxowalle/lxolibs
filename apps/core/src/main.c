#include "main.h"
#include "core.h"
#include "log.h"

#include <stdio.h>
#include <string.h>

static void _printf_tree(tree_t *tree, int type,  int level)
{
	int i;

	if (!tree)  return;

	_printf_tree(tree->one, 2, level + 1);
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
	_printf_tree(tree->zero, 1,  level + 1);
}

int main(int argc, char** argv)
{
    tree_t *a = create_tree();
    tree_t *b = create_tree();
    tree_t *c = create_tree();
    tree_t *d = create_tree();
    tree_t *e = create_tree();
    tree_t *f = create_tree();
    tree_t *g = create_tree();
    tree_t *h = create_tree();
    tree_t *i = create_tree();
    tree_t *j = create_tree();
    tree_t *k = create_tree();
    tree_t *l = create_tree();
    tree_t *m = create_tree();
    tree_t *n = create_tree();

    strcpy(a->name, "a");
    strcpy(b->name, "b");
    strcpy(c->name, "c");
    strcpy(d->name, "d");
    strcpy(e->name, "e");
    strcpy(f->name, "f");
    strcpy(g->name, "g");
    strcpy(h->name, "h");
    strcpy(i->name, "i");
    strcpy(j->name, "j");
    strcpy(k->name, "k");
    strcpy(l->name, "l");
    strcpy(m->name, "m");
    strcpy(n->name, "n");

    /**     
     *    a___c
     *    |       
     *    b___c
     *    |   | 
     *    | \
     *    d e_
     *      
     *      |-
     *     / 
     *    b
     *    |_
     * a_/
     * 
     */
    /*
   

    d e f g
    \ / \ /
      b c
      \ / 
       a
    */ 
    connect_tree(a, 1, b);
    connect_tree(a, 0, c);
    connect_tree(b, 1, d);
    connect_tree(b, 0, e);
    connect_tree(c, 1, f);
    connect_tree(c, 0, g);
    connect_tree(f, 1, h);
    connect_tree(g, 1, i);
    connect_tree(h, 1, j);
    connect_tree(h, 0, k);

    connect_tree(k, 1, l);
    connect_tree(k, 0, m);
    connect_tree(m, 0, n);
    traverse_tree(a);
    print_tree(a);

    destory_tree(&a);
    destory_tree(&b);
    destory_tree(&c);
    destory_tree(&d);
    destory_tree(&e);
    return 0;
}