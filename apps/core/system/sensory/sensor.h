// #ifndef __SENSOR_H
// #define __SENSOR_H

// #include "core.h"
// #include "log.h"
// #include <stdint.h>

// typedef struct sensor_t sensor_t;

// typedef enum
// {
//     SENEOR_NONE,
//     SENSOR_TOUCH,
//     SENSOR_MAX
// }sensor_type_t;

// typedef struct
// {
//     int (*connect_nerve)(sensor_t *sensor, nerve_t *child);
//     int (*feel)(sensor_t *sensor, uint8_t *data, size_t data_len);
// }sensor_ops_t;

// struct sensor_t
// {
//     sensor_type_t type;
//     sensor_ops_t ops;
// };

// sensor_t *create_sensor(sensor_type_t type);

// #endif // __SENSOR_H