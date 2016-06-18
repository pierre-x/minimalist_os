#ifndef __OS_H__
#define __OS_H__

#include "os_config.h"
#include <stdint.h>
#include <stdbool.h>

#define DEBUG_MODE
#define ARRAY_SIZE(array) (uint8_t)(sizeof(array)/sizeof(array[0]))

typedef void (*function_pointer_t)(void);	/* function pointer */

void os_init(void);				    /* internal initialization */
void os_start(void);				/* start OS, will never return */
void os_tick_update(void);			/* interrupt routine that must be called to update the OS Tick */
void os_set_timer(enum timers timer_id, uint16_t delay_ms, function_pointer_t callback);
void os_retrig_timer(enum timers timer_id);
void os_set_cyclic_timer(enum timers timer_id, uint16_t delay_ms, function_pointer_t callback);
bool os_timer_is_running(enum timers timer_id);
void os_stop_timer(enum timers timer_id);
void os_exec(function_pointer_t function);

#endif
