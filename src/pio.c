/**
 * @file pio.c
 * @brief TODO.
 *
 * TODO:
 * - clean up the repeats
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <phidget22.h>

#include "pio.h"

#define OPEN_TIMEOUT_MS (5000)

#define AIN_CHANNEL_1143 (3)
#define AIN_CHANNEL_1127 (2)
#define AIN_CHANNEL_1125_HUMID (0)
#define AIN_CHANNEL_1125_TEMP (1)

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
    PhidgetReturnCode p_ret;

    if(pio == NULL)
    {
        ret = -1;
    }

    // create channels
    check_and_exec(
            PhidgetVoltageInput_create,
            ret,
            p_ret,
            &pio->h_1143);

    check_and_exec(
            PhidgetVoltageInput_create,
            ret,
            p_ret,
            &pio->h_1127);

    check_and_exec(
            PhidgetVoltageRatioInput_create,
            ret,
            p_ret,
            &pio->h_1125_humid);

    check_and_exec(
            PhidgetVoltageRatioInput_create,
            ret,
            p_ret,
            &pio->h_1125_temp);

    // set channel indices
    check_and_exec(
            Phidget_setChannel,
            ret,
            p_ret,
            (PhidgetHandle) pio->h_1143,
            AIN_CHANNEL_1143);

    check_and_exec(
            Phidget_setChannel,
            ret,
            p_ret,
            (PhidgetHandle) pio->h_1127,
            AIN_CHANNEL_1127);

    check_and_exec(
            Phidget_setChannel,
            ret,
            p_ret,
            (PhidgetHandle) pio->h_1125_humid,
            AIN_CHANNEL_1125_HUMID);

    check_and_exec(
            Phidget_setChannel,
            ret,
            p_ret,
            (PhidgetHandle) pio->h_1125_temp,
            AIN_CHANNEL_1125_TEMP);

    // set channel sensor types
    check_and_exec(
            PhidgetVoltageInput_setSensorType,
            ret,
            p_ret,
            pio->h_1143,
            SENSOR_TYPE_1143);

    check_and_exec(
            PhidgetVoltageInput_setSensorType,
            ret,
            p_ret,
            pio->h_1127,
            SENSOR_TYPE_1127);

    check_and_exec(
            PhidgetVoltageRatioInput_setSensorType,
            ret,
            p_ret,
            pio->h_1125_humid,
            SENSOR_TYPE_1125_HUMIDITY);

    check_and_exec(
            PhidgetVoltageRatioInput_setSensorType,
            ret,
            p_ret,
            pio->h_1125_temp,
            SENSOR_TYPE_1125_TEMPERATURE);

    if(sn > 0)
    {
        check_and_exec(
                Phidget_setDeviceSerialNumber,
                ret,
                p_ret,
                (PhidgetHandle) pio->h_1143,
                (int32_t) sn);

        check_and_exec(
                Phidget_setDeviceSerialNumber,
                ret,
                p_ret,
                (PhidgetHandle) pio->h_1127,
                (int32_t) sn);

        check_and_exec(
                Phidget_setDeviceSerialNumber,
                ret,
                p_ret,
                (PhidgetHandle) pio->h_1125_humid,
                (int32_t) sn);

        check_and_exec(
                Phidget_setDeviceSerialNumber,
                ret,
                p_ret,
                (PhidgetHandle) pio->h_1125_temp,
                (int32_t) sn);
    }

    check_and_exec(
            Phidget_openWaitForAttachment,
            ret,
            p_ret,
            (PhidgetHandle) pio->h_1143,
            OPEN_TIMEOUT_MS);

    check_and_exec(
            Phidget_openWaitForAttachment,
            ret,
            p_ret,
            (PhidgetHandle) pio->h_1127,
            OPEN_TIMEOUT_MS);

    check_and_exec(
            Phidget_openWaitForAttachment,
            ret,
            p_ret,
            (PhidgetHandle) pio->h_1125_humid,
            OPEN_TIMEOUT_MS);

    check_and_exec(
            Phidget_openWaitForAttachment,
            ret,
            p_ret,
            (PhidgetHandle) pio->h_1125_temp,
            OPEN_TIMEOUT_MS);

    return ret;
}

void pio_fini(
        pio_s * const pio)
{
    PhidgetReturnCode p_ret;

    if(pio != NULL)
    {
        p_ret = Phidget_close((PhidgetHandle) pio->h_1143);
        if(p_ret != EPHIDGET_OK)
        {
            (void) p_err("Phidget_close (1143)", p_ret);
        }

        p_ret = Phidget_close((PhidgetHandle) pio->h_1127);
        if(p_ret != EPHIDGET_OK)
        {
            (void) p_err("Phidget_close (1127)", p_ret);
        }

        p_ret = Phidget_close((PhidgetHandle) pio->h_1125_humid);
        if(p_ret != EPHIDGET_OK)
        {
            (void) p_err("Phidget_close (1125-humid)", p_ret);
        }

        p_ret = Phidget_close((PhidgetHandle) pio->h_1125_temp);
        if(p_ret != EPHIDGET_OK)
        {
            (void) p_err("Phidget_close (1125-temp)", p_ret);
        }

        p_ret = PhidgetVoltageInput_delete(&pio->h_1143);
        if(p_ret != EPHIDGET_OK)
        {
            (void) p_err("PhidgetVoltageInput_delete (1143)", p_ret);
        }

        p_ret = PhidgetVoltageInput_delete(&pio->h_1127);
        if(p_ret != EPHIDGET_OK)
        {
            (void) p_err("PhidgetVoltageInput_delete (1127)", p_ret);
        }

        p_ret = PhidgetVoltageRatioInput_delete(&pio->h_1125_humid);
        if(p_ret != EPHIDGET_OK)
        {
            (void) p_err("PhidgetVoltageRatioInput_delete (1125-humid)", p_ret);
        }

        p_ret = PhidgetVoltageRatioInput_delete(&pio->h_1125_temp);
        if(p_ret != EPHIDGET_OK)
        {
            (void) p_err("PhidgetVoltageRatioInput_delete (1125-temp)", p_ret);
        }
    }
}
