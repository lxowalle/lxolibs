#include "main.h"
#include "common.h"
#include "core.h"
#include "sensor.h"
#include <string.h>

__attribute__((constructor)) static void _start_handler(void)
{
    LOGI("start handler!\n");
}

__attribute__((destructor)) static void _exit_handler(void)
{
    LOGI("exit handler!\n");
}

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

    connect_nerve(a, 1, b);
    connect_nerve(a, 0, c);
    connect_nerve(b, 1, d);
    connect_nerve(b, 0, e);
    connect_nerve(c, 1, f);
    connect_nerve(c, 0, g);

    // connect_nerve(f, 1, h);
    connect_nerve(g, 1, i);
    connect_nerve(h, 1, j);
    connect_nerve(h, 0, k);

    connect_nerve(k, 1, l);
    connect_nerve(k, 0, m);
    connect_nerve(m, 0, n);
    // traverse_nerve(a);
    print_nerve(a);
    LOGI("===============================================\n");
    print_nerve(h);

    LOGI("===============================================\n");

    neuron_t *new_psedounipolar = neuron_create(NEURONS_TYPE_PSEUDOUNIPOLAR);
    if (!new_psedounipolar)
    {
        LOGE("Error,create neuron\n");
    }

    neuron_t *neuron_bipolar = neuron_create(NEURONS_TYPE_BIPOLAR);
    if (!neuron_bipolar)
    {
        LOGE("Error,create neuron\n");
    }

    neuron_t *neuron_multipolar = neuron_create(NEURONS_TYPE_MULTIPOLAR);
    if (!neuron_multipolar)
    {
        LOGE("Error,create neuron\n");
    }

    neuron_control(new_psedounipolar, NEURONS_CTLSET_CONNECTAXON, a);
    neuron_control(new_psedounipolar, NEURONS_CTLSET_CONNECTSURROUND, h);

    // print_neuron(new_psedounipolar);

    sensor_t *sensor = (sensor_t *)create_sensor(SENSOR_TOUCH);
    if (sensor)
    {
        LOGI("Create sensor successfully\n");
        sensor->ops.connect_nerve(sensor, i);
        print_neuron(new_psedounipolar);
        sensor->ops.feel(sensor, "hello", sizeof("hello"));
    }
    else
    {
        LOGE("Create failed\n");
    }

    destory_nerve(&a);
    destory_nerve(&b);
    destory_nerve(&c);
    destory_nerve(&d);
    destory_nerve(&e);

    neuron_destory(&new_psedounipolar);
    neuron_destory(&neuron_bipolar);
    neuron_destory(&neuron_multipolar);



    /** 日常测试C代码使用 */
    extern int normal_test(void);
    normal_test();

    return 0;
}