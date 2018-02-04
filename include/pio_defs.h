/**
 * @file pio_defs.h
 * @brief TODO.
 *
 */

#ifndef PIO_DEFS_H
#define PIO_DEFS_H

#include <time.h>
#include <phidget22.h>

#define PIO_MEASUREMENT_VALUE_INVALID (0.0)

enum pio_sensor_e
{
    PIO_SENSOR_1143 = 0,
    PIO_SENSOR_1127,
    PIO_SENSOR_1125_HUMID,
    PIO_SENSOR_1125_TEMP,
    PIO_SENSOR_KIND_COUNT
};

typedef struct
{
    struct timespec timestamp;
    double values[PIO_SENSOR_KIND_COUNT];
} pio_measurement_s;

typedef struct
{
    char sensor_info[256];
    Phidget_UnitInfo unit_info;
    union
    {
        PhidgetVoltageInputHandle h_vin;
        PhidgetVoltageRatioInputHandle h_vrin;
    };
} pio_sensor_s;

#endif  /* PIO_DEFS_H */
