#include "FreeRTOS.h"
#include "task.h"

#include "supporting_functions.h"

/* Used as a loop counter to create a very crude delay. */
#define mainDELAY_LOOP_COUNT 1000000	//( 0xffffff )

/* The task functions. */

void vTask2(void* pvParameters)
{
	/* Task 2 does nothing but delete itself. To do this it could call vTaskDelete()
	using NULL as the parameter, but instead, and purely for demonstration purposes,
	it calls vTaskDelete() passing its own task handle. */
	vPrintString("Task 2 is running and about to delete itself\r\n");
	vTaskDelete(NULL);
}

void vTask1(void* pvParameters)
{
	const TickType_t xDelay100ms = pdMS_TO_TICKS(100UL);
	for (;; )
	{
		/* Print out the name of this task. */
		vPrintString("Task 1 is running\r\n");
		/* Create task 2 at a higher priority. */
		xTaskCreate(vTask2, "Task 2", 1000, NULL, 2, NULL);
		
		/* Task 2 has/had the higher priority, so for Task 1 to reach here Task 2
		must have already executed and deleted itself. Delay for 100
		milliseconds. */
		vTaskDelay(xDelay100ms);
	}
}




int main()
{
	/* Create the 1st task. */
	xTaskCreate(vTask1, "Task 1", 1000, NULL, 1, NULL);

	
	/* Start the scheduler to start the tasks executing. */
	vTaskStartScheduler();

	/* The following line should never be reached because vTaskStartScheduler()
	will only return if there was not enough FreeRTOS heap memory available to
	create the Idle and (if configured) Timer tasks.*/
	for (;; );
	return 0;
}
/*-----------------------------------------------------------*/


