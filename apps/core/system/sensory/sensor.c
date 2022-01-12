// #include "sensor.h"
// #include "core.h"
// #include "log.h"

// #include <stdlib.h>
// #include <stdarg.h>
// #include <string.h>

// extern sensor_t *sensor_touch_create(void);
// extern void sensor_touch_destory(sensor_t **me);

// sensor_t *create_sensor(sensor_type_t type)
// {
//     sensor_t *new_sensor = NULL;

//     switch (type)
//     {
//         case SENSOR_TOUCH:
//         {
//             new_sensor = (sensor_t *)sensor_touch_create();
//             if (!new_sensor) return NULL;
            
//             LOGI("sensor %p\n", new_sensor);
//             return new_sensor;
//             break;
//         }
//         default:
//         {
//             new_sensor = NULL;
//             break;
//         }
//     }
// LOGI("sensor %p\n", new_sensor);
//     return new_sensor;
// }
