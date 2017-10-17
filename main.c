#include <stdio.h>
#include <stdlib.h>
#include "labworks.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

typedef struct {
    unsigned int num;    // led number (LIB_LEDS_GREEN, LIB_LEDS_RED)
    unsigned int status; // led status (LIB_LEDS_OFF, LIB_LEDS_ON, ...)
} LedInfo;

void taskLeds(void *pvParameters)
{
	QueueHandle_t ledQueue = (QueueHandle_t) pvParameters;
    LedInfo led;

    for (;;) {
        xQueueReceive(ledQueue, &led, portMAX_DELAY);
        libLedsSet(led.num, led.status);
    }
}

void taskButtons(void *pvParameters)
{
	QueueHandle_t ledQueue = (QueueHandle_t) pvParameters;
    LedInfo led;

    led.status = LIB_LEDS_OFF;
    led.num = LIB_LEDS_RED;

    for (;;) {

        if (libButtonsSW1Pressed() && led.status == LIB_LEDS_OFF) {
            led.status = LIB_LEDS_ON;
            xQueueSend(ledQueue, &led, portMAX_DELAY);
        }
        else if (libButtonsSW3Pressed() && led.status == LIB_LEDS_ON) {
            led.status = LIB_LEDS_OFF;
            xQueueSend(ledQueue, &led, portMAX_DELAY);
        }

        vTaskDelay(100/portTICK_RATE_MS);
    }
}

int main(void)
{
	QueueHandle_t ledQueue;

	libHwInit();

	/* create queue */
	ledQueue = xQueueCreate(1, sizeof(LedInfo));

	/* create leds task */
    xTaskCreate(taskLeds, (signed char *)"TaskLeds",
                configMINIMAL_STACK_SIZE, (void *)ledQueue, 1, NULL);

    /* create buttons task */
    xTaskCreate(taskButtons, (signed char *)"TaskButtons",
                configMINIMAL_STACK_SIZE, (void *)ledQueue, 1, NULL);

	/* start the scheduler */
	vTaskStartScheduler();

	for (;;);

	return 0;
}
