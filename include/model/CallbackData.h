#ifndef _CALLBACK_DATA_H
#define _CALLBACK_DATA_H

#include <stdint.h>

/**
 * Callback struct used to store sigfox input data
 */
struct __attribute__((packed)) CallbackData
{
    uint32_t timestamp;
    uint8_t sprinkleStartHour;
    uint8_t sprinkleStartMinute;
    uint8_t sprinleDuration;
    uint8_t minimalSprinkleHumidity;
};

#endif