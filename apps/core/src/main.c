#include "main.h"
#include "core.h"
#include "log.h"

#include <stdio.h>
#include <string.h>

static void _printf_nerve(nerve_t *nerve, int type,  int level)
{
	int i;

	if (!nerve)  return;

	_printf_nerve(nerve->one, 2, level + 1);
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
	_printf_nerve(nerve->zero, 1,  level + 1);
}

int main(int argc, char** argv)
{
    nerve_t *a = create_nerve();
    nerve_t *b = create_nerve();
    nerve_t *c = create_nerve();
    nerve_t *d = create_nerve();
    nerve_t *e = create_nerve();
    nerve_t *f = create_nerve();
    nerve_t *g = create_nerve();
    nerve_t *h = create_nerve();
    nerve_t *i = create_nerve();
    nerve_t *j = create_nerve();
    nerve_t *k = create_nerve();
    nerve_t *l = create_nerve();
    nerve_t *m = create_nerve();
    nerve_t *n = create_nerve();

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
    connect_nerve(a, 1, b);
    connect_nerve(a, 0, c);
    connect_nerve(b, 1, d);
    connect_nerve(b, 0, e);
    connect_nerve(c, 1, f);
    connect_nerve(c, 0, g);
    connect_nerve(f, 1, h);
    connect_nerve(g, 1, i);
    connect_nerve(h, 1, j);
    connect_nerve(h, 0, k);

    connect_nerve(k, 1, l);
    connect_nerve(k, 0, m);
    connect_nerve(m, 0, n);
    traverse_nerve(a);
    print_nerve(a);

    destory_nerve(&a);
    destory_nerve(&b);
    destory_nerve(&c);
    destory_nerve(&d);
    destory_nerve(&e);
    return 0;
}