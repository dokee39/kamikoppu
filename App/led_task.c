#include "led_task.h"
#include "projdefs.h"
#include "ws2812.h"
#include <math.h>
#include "FreeRTOS.h"
#include "task.h"

#define LED_FLOW_PERIOD pdMS_TO_TICKS(1200)
#define LED_BRIGHTNESS 8

void led_task(void *argument)
{
    TickType_t xLastWakeTime;
    uint32_t time = 0;
    uint8_t r, b, g;
    
    xLastWakeTime = xTaskGetTickCount();
    
    for (;;) {
        time = xTaskGetTickCount();
        time %= LED_FLOW_PERIOD;
        r = (sin(((float)time / LED_FLOW_PERIOD) * 2 * 3.1415f) + 1) * LED_BRIGHTNESS;
        b = (sin(((float)time / LED_FLOW_PERIOD + 0.3333f) * 2 * 3.1415f) + 1) * LED_BRIGHTNESS;
        g = (sin(((float)time / LED_FLOW_PERIOD + 0.6666f) * 2 * 3.1415f) + 1) * LED_BRIGHTNESS;
        WS2812_Ctrl(r, b, g);
        
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(10));
    }
}



