/**
 * @file pio.h
 * @brief Phidgets I/O interface.
 *
 */

#ifndef PIO_H
#define PIO_H

#include <time.h>
#include <phidget22.h>

#include "pio_defs.h"

typedef struct
{
    unsigned long serial_number;
    pio_sensor_s sensors[PIO_SENSOR_KIND_COUNT];
} pio_s;

int pio_init(
        const unsigned long sn,
        pio_s * const pio);

void pio_fini(
        pio_s * const pio);

int pio_poll(
        pio_s * const pio,
        pio_measurement_s * const measurement);

#endif  /* PIO_H */
