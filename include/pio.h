/**
 * @file pio.h
 * @brief Phidgets I/O interface.
 *
 */

#ifndef PIO_H
#define PIO_H

#include <phidget22.h>

typedef struct
{
    PhidgetVoltageInputHandle h_1143;
    PhidgetVoltageInputHandle h_1127;
    PhidgetVoltageRatioInputHandle h_1125_humid;
    PhidgetVoltageRatioInputHandle h_1125_temp;
} pio_s;

int pio_init(
        const unsigned long sn,
        pio_s * const pio);

void pio_fini(
        pio_s * const pio);

#endif  /* PIO_H */
