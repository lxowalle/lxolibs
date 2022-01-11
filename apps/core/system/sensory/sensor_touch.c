#include "sensor.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

typedef struct sensor_touch_t sensor_touch_t;

typedef struct 
{
    char data;
}sensor_touch_data_t;

typedef struct sensor_touch_ops_t
{
    int (*init)(sensor_touch_t *sensor);
    int (*deinit)(sensor_touch_t *sensor);
    nerve_t *(*transport)(nerve_t *node, void *data, size_t data_len);
}sensor_touch_ops_t;

struct sensor_touch_t
{
    sensor_t ext;
    sensor_touch_ops_t ops; 
    nerve_t *child;
};

/** Public */
static int _public_connect(sensor_t *me, nerve_t *child)
{
    if (!me || !child)  return -1;

    sensor_touch_t *st = (sensor_touch_t *)me;
    LOGI("connect????\n");
    connect_nerve(child, 0, st->child);
    return 0;
}

static int _public_feel(sensor_t *me, uint8_t *data, size_t data_len)
{
    sensor_touch_t *st = (sensor_touch_t *)me;
    if (!st)    return -1;
    LOGI("data\n");
    
    nerve_t* top = st->ops.transport(st->child, data, data_len);
    if (top)
    {
        if (top->name)
            printf("%s\n", top->name);
    }
    return 0;
}

/** Private */
static int _private_init(sensor_touch_t *me)
{
    if (!me)    return -1;
    LOGI("init\n");
    me->ext.type = SENSOR_TOUCH;
    me->ext.ops.connect_nerve = _public_connect;
    me->ext.ops.feel = _public_feel;

    return 0;
}

static int _private_deinit(sensor_touch_t *me)
{
    LOGI("deinit\n");
    return 0;
}

static nerve_t *_private_transport(nerve_t *node, void *data, size_t data_len)
{
    nerve_t *top = node;
    while (top->father)
    {
        top = top->father;
        if (top->ops.recv_handle)
            top->ops.recv_handle(top, data, data_len);
    };
    return top;
}

sensor_t *sensor_touch_create(void)
{
    sensor_touch_t *new = (sensor_touch_t *)calloc(1, sizeof(sensor_touch_t));
    if (!new) return NULL;

    new->ops.init = _private_init;
    new->ops.deinit = _private_deinit;
    new->ops.transport = _private_transport;
    new->child = create_nerve();
    if (!new->child)
    {
        free(new);
        LOGW("Overflow!\n");
        return NULL;
    }
    strcpy(new->child->name, "sensor");
    if (new->ops.init)  _private_init(new);

    return (sensor_t *)new;
}

void sensor_touch_destory(sensor_t **me)
{
    sensor_touch_t *st = (sensor_touch_t *)*me;
    if (st->ops.deinit) st->ops.deinit(st);

    if (*me)
    {
        free(*me);
        *me = NULL;
    }
}