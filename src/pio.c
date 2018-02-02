/**
 * @file pio.c
 * @brief TODO.
 *
 * TODO:
 * - clean up the repeats, for-each macro or better indexing scheme
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <time.h>
#include <phidget22.h>

#include "default_config.h"
#include "pio.h"

#define OPEN_TIMEOUT_MS (5000)

#define check_and_exec(fx, r, pr, ...) \
{ \
    if(r == 0) \
    { \
        pr = fx(__VA_ARGS__); \
        if(pr != EPHIDGET_OK) \
        { \
            r = p_err(#fx, pr); \
        } \
    } \
}

static const int INDEX_TO_CHANNEL[] =
{
    [PIO_SENSOR_1143] = DEF_AIN_CHANNEL_1143,
    [PIO_SENSOR_1127] = DEF_AIN_CHANNEL_1127,
    [PIO_SENSOR_1125_HUMID] = DEF_AIN_CHANNEL_1125_HUMID,
    [PIO_SENSOR_1125_TEMP] = DEF_AIN_CHANNEL_1125_TEMP
};

static int p_err(
        const char * const label,
        const PhidgetReturnCode p_ret)
{
    const char *err = NULL;

    if(p_ret != EPHIDGET_OK)
    {
        (void) Phidget_getErrorDescription(p_ret, &err);

        (void) fprintf(
                stderr,
                "%s : %s\n", label, err);
    }
    else if(label != NULL)
    {
        (void) fprintf(
                stderr,
                "%s\n", label);
    }

    return -1;
}

int pio_init(
        const unsigned long sn,
        pio_s * const pio)
{
    int ret = 0;
    unsigned long idx;
    PhidgetReturnCode p_ret;

    if(pio == NULL)
    {
        ret = -1;
    }

    // set names
    (void) strncpy(
            pio->sensors[PIO_SENSOR_1143].sensor_info,
            "1143 WR_L ",
            sizeof(pio->sensors[PIO_SENSOR_1143].sensor_info));

    (void) strncpy(
            pio->sensors[PIO_SENSOR_1127].sensor_info,
            "1127 P_L     ",
            sizeof(pio->sensors[PIO_SENSOR_1127].sensor_info));

    (void) strncpy(
            pio->sensors[PIO_SENSOR_1125_HUMID].sensor_info,
            "1125_HUM ",
            sizeof(pio->sensors[PIO_SENSOR_1125_HUMID].sensor_info));

    (void) strncpy(
            pio->sensors[PIO_SENSOR_1125_TEMP].sensor_info,
            "1125_TEMP",
            sizeof(pio->sensors[PIO_SENSOR_1125_TEMP].sensor_info));

    // create channels
    for(idx = PIO_SENSOR_1143; idx <= PIO_SENSOR_1127; idx += 1)
    {
        check_and_exec(
                PhidgetVoltageInput_create,
                ret,
                p_ret,
                &pio->sensors[idx].h_vin);
    }

    for(idx = PIO_SENSOR_1125_HUMID; idx <= PIO_SENSOR_1125_TEMP; idx += 1)
    {
        check_and_exec(
                PhidgetVoltageRatioInput_create,
                ret,
                p_ret,
                &pio->sensors[idx].h_vrin);
    }

    // set channel indices
    for(idx = PIO_SENSOR_1143; idx <= PIO_SENSOR_1127; idx += 1)
    {
        check_and_exec(
                Phidget_setChannel,
                ret,
                p_ret,
                (PhidgetHandle) pio->sensors[idx].h_vin,
                INDEX_TO_CHANNEL[idx]);
    }

    for(idx = PIO_SENSOR_1125_HUMID; idx <= PIO_SENSOR_1125_TEMP; idx += 1)
    {
        check_and_exec(
                Phidget_setChannel,
                ret,
                p_ret,
                (PhidgetHandle) pio->sensors[idx].h_vrin,
                INDEX_TO_CHANNEL[idx]);
    }

    // enforce sn if specified
    if(sn > 0)
    {
        for(idx = PIO_SENSOR_1143; idx <= PIO_SENSOR_1127; idx += 1)
        {
            check_and_exec(
                    Phidget_setDeviceSerialNumber,
                    ret,
                    p_ret,
                    (PhidgetHandle) pio->sensors[idx].h_vin,
                    (int32_t) sn);
        }

        for(idx = PIO_SENSOR_1125_HUMID; idx <= PIO_SENSOR_1125_TEMP; idx += 1)
        {
            check_and_exec(
                    Phidget_setDeviceSerialNumber,
                    ret,
                    p_ret,
                    (PhidgetHandle) pio->sensors[idx].h_vrin,
                    (int32_t) sn);
        }
    }

    // open
    for(idx = PIO_SENSOR_1143; idx <= PIO_SENSOR_1127; idx += 1)
    {
        check_and_exec(
                Phidget_openWaitForAttachment,
                ret,
                p_ret,
                (PhidgetHandle) pio->sensors[idx].h_vin,
                OPEN_TIMEOUT_MS);
    }

    for(idx = PIO_SENSOR_1125_HUMID; idx <= PIO_SENSOR_1125_TEMP; idx += 1)
    {
        check_and_exec(
                Phidget_openWaitForAttachment,
                ret,
                p_ret,
                (PhidgetHandle) pio->sensors[idx].h_vrin,
                OPEN_TIMEOUT_MS);
    }

    // get serial number from any channel
    if(ret == 0)
    {
        int32_t dev_sn = 0;

        p_ret = Phidget_getDeviceSerialNumber(
                (PhidgetHandle) pio->sensors[0].h_vin,
                &dev_sn);

        if(p_ret != EPHIDGET_OK)
        {
            ret = p_err("Phidget_getDeviceSerialNumber", p_ret);
        }
        else
        {
            pio->serial_number = (unsigned long) dev_sn;

            (void) fprintf(
                    stdout,
                    "found device with serial number '%ld'\n",
                    (long) dev_sn);
        }
    }

    // set channel sensor types
    check_and_exec(
            PhidgetVoltageInput_setSensorType,
            ret,
            p_ret,
            pio->sensors[PIO_SENSOR_1143].h_vin,
            SENSOR_TYPE_1143);

    check_and_exec(
            PhidgetVoltageInput_setSensorType,
            ret,
            p_ret,
            pio->sensors[PIO_SENSOR_1127].h_vin,
            SENSOR_TYPE_1127);

    check_and_exec(
            PhidgetVoltageRatioInput_setSensorType,
            ret,
            p_ret,
            pio->sensors[PIO_SENSOR_1125_HUMID].h_vrin,
            SENSOR_TYPE_1125_HUMIDITY);

    check_and_exec(
            PhidgetVoltageRatioInput_setSensorType,
            ret,
            p_ret,
            pio->sensors[PIO_SENSOR_1125_TEMP].h_vrin,
            SENSOR_TYPE_1125_TEMPERATURE);

    // populate sensor info
    for(idx = PIO_SENSOR_1143; idx <= PIO_SENSOR_1127; idx += 1)
    {
        check_and_exec(
                PhidgetVoltageInput_getSensorUnit,
                ret,
                p_ret,
                pio->sensors[idx].h_vin,
                &pio->sensors[idx].unit_info);
    }

    for(idx = PIO_SENSOR_1125_HUMID; idx <= PIO_SENSOR_1125_TEMP; idx += 1)
    {
        check_and_exec(
                PhidgetVoltageRatioInput_getSensorUnit,
                ret,
                p_ret,
                pio->sensors[idx].h_vrin,
                &pio->sensors[idx].unit_info);
    }

    for(idx = 0; idx < PIO_SENSOR_KIND_COUNT; idx += 1)
    {
        (void) fprintf(
                stdout,
                "[%lu]\n  unit: 0x%lX\n  name: '%s'\n  symbol: '%s'\n",
                idx,
                (unsigned long) pio->sensors[idx].unit_info.unit,
                pio->sensors[idx].unit_info.name,
                pio->sensors[idx].unit_info.symbol);
    }

    // TODO - get rid of this after testing
    (void) fflush(stdout);
    (void) fflush(stderr);

    return ret;
}

void pio_fini(
        pio_s * const pio)
{
    unsigned long idx;
    PhidgetReturnCode p_ret;

    if(pio != NULL)
    {
        for(idx = PIO_SENSOR_1143; idx <= PIO_SENSOR_1127; idx += 1)
        {
            p_ret = Phidget_close((PhidgetHandle) pio->sensors[idx].h_vin);
            if(p_ret != EPHIDGET_OK)
            {
                (void) p_err("Phidget_close", p_ret);
            }

            p_ret = PhidgetVoltageInput_delete(&pio->sensors[idx].h_vin);
            if(p_ret != EPHIDGET_OK)
            {
                (void) p_err("PhidgetVoltageInput_delete", p_ret);
            }
        }

        for(idx = PIO_SENSOR_1125_HUMID; idx <= PIO_SENSOR_1125_TEMP; idx += 1)
        {
            p_ret = Phidget_close((PhidgetHandle) pio->sensors[idx].h_vrin);
            if(p_ret != EPHIDGET_OK)
            {
                (void) p_err("Phidget_close", p_ret);
            }

            p_ret = PhidgetVoltageRatioInput_delete(&pio->sensors[idx].h_vrin);
            if(p_ret != EPHIDGET_OK)
            {
                (void) p_err("PhidgetVoltageRatioInput_delete", p_ret);
            }
        }
    }
}

int pio_poll(
        pio_s * const pio,
        pio_measurement_s * const measurement)
{
    int ret = 0;
    unsigned long idx;
    unsigned long v_idx;
    PhidgetReturnCode p_ret;

    if(pio == NULL)
    {
        ret = -1;
    }

    v_idx = 0;
    for(idx = PIO_SENSOR_1143; idx <= PIO_SENSOR_1127; idx += 1, v_idx += 1)
    {
        check_and_exec(
                PhidgetVoltageInput_getSensorValue,
                ret,
                p_ret,
                pio->sensors[idx].h_vin,
                &measurement->values[v_idx]);

        // the 1127 seems to go below their APIs threshold
        if(ret != 0)
        {
            if(p_ret == EPHIDGET_UNKNOWNVAL)
            {
                if(idx == PIO_SENSOR_1127)
                {
                    ret = 0;
                    p_ret = EPHIDGET_OK;
                }
            }
        }
    }

    for(idx = PIO_SENSOR_1125_HUMID; idx <= PIO_SENSOR_1125_TEMP; idx += 1, v_idx += 1)
    {
        check_and_exec(
                PhidgetVoltageRatioInput_getSensorValue,
                ret,
                p_ret,
                pio->sensors[idx].h_vrin,
                &measurement->values[v_idx]);
    }

    if(ret == 0)
    {
        ret = clock_gettime(
                CLOCK_REALTIME,
                &measurement->timestamp);
    }

    return ret;
}
