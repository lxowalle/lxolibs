#include "sensor.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

typedef struct sensor_touch_t sensor_touch_t;

typedef struct sensor_touch_ops_t
{
    int (*init)(sensor_touch_t *sensor);
    int (*deinit)(sensor_touch_t *sensor);
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

static int _public_feel(sensor_t *me, ...)
{
    LOGI("data\n");
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

    if (me->ext.ops.feel)   me->ext.ops.feel(me);
    sensor_t *ss = (sensor_t *)me;
    ss->ops.feel(ss);
    return 0;
}

static int _private_deinit(sensor_touch_t *me)
{
    LOGI("deinit\n");
    return 0;
}

sensor_t *sensor_touch_create(void)
{
    sensor_touch_t *new = (sensor_touch_t *)calloc(1, sizeof(sensor_touch_t));
    if (!new) return NULL;

    new->ops.init = _private_init;
    new->ops.deinit = _private_deinit;
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