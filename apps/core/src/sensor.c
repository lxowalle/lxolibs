#include "sensor.h"
#include "core.h"
#include "log.h"

#include <stdlib.h>

static int _sensor_connect_nerve(sensor_t *sensor, nerve_t *child)
{
    return connect_nerve(child, 0, sensor->child);
}

sensor_t *create_sensor(sensor_type_t type)
{
    sensor_t *new_sensor = NULL;

    switch (type)
    {
        case SENSOR_TOUCH:
        {
            sensor_touch_t *new = (sensor_touch_t *)calloc(1, sizeof(sensor_touch_t));
            if (new)
            {
                new->ext.type = SENSOR_TOUCH;
                new->child = create_nerve();
                if (!new->child) 
                {
                    LOGW("Overflow!\n");
                    return NULL;
                }

                new_sensor = (sensor_t *)new;
                new_sensor->ops.connect_nerve = _sensor_connect_nerve;
                new_sensor->child = create_nerve();
                if (!new_sensor->child)
                {
                    free(new->child);
                    LOGW("Overflow!\n");
                    return NULL;
                }
                strcpy(new_sensor->child->name, "sensor_ext");
            }
            break;
        }
        default:
        {
            new_sensor = NULL;
            break;
        }
    }

    return new_sensor;
}
