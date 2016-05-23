#include "os.h"

struct exec_stack_s{
	uint8_t stack_index;
	function_pointer_t function_pointer[OS_EXEC_STACK_SIZE];
};

struct timer_attribute{
	bool running;
	bool retriggable;
	uint8_t trig_value;
	uint8_t current_value;
	function_pointer_t callback;
};

struct os{
	struct exec_stack_s exec_stack;
	volatile uint8_t pending_tick;
	struct timer_attribute timer[OS_TIMERS_NUMBER];
};

struct os me;

/* interrupt routine that is called to update the OS Tick */
static void time_scheduler(void);

void os_init(void)
{  
	uint8_t i;

	/* init task stack */
	me.exec_stack.stack_index=0;

	/* init timers */
	for(i=0; i<OS_TIMERS_NUMBER; i++)
		me.timer[i].running = false;

	me.pending_tick = 0;
}

/* start OS - will never return */
void os_start(void)
{
	for(;;){
		/* exec functions on the stack */
	      	if(me.exec_stack.stack_index) /* task in the stack? */
			me.exec_stack.function_pointer[--me.exec_stack.stack_index](); /* run it and unstack */

		/* check pending ticks */
		//__disable_interrupt();
		if(me.pending_tick){
			time_scheduler();
			me.pending_tick--;
		}
		//__enable_interrupt();
      	}
/* never return */
}

/* interrupt routine that must be called to update the OS Tick */
void os_tick_update(void)
{  
	me.pending_tick++;
}

static void time_scheduler(void)
{  
	uint8_t i;
        
	/* decrease each running timer */
  	for(i=0; i<OS_TIMERS_NUMBER; i++)               
		if(me.timer[i].running){
			me.timer[i].current_value++;
			if(me.timer[i].current_value == me.timer[i].trig_value){
				if(me.timer[i].retriggable)
					me.timer[i].current_value = 0;
				else
					me.timer[i].running = false;

				me.timer[i].callback();
			}
		}
}

void os_set_timer(enum timers timer_id, uint16_t delay_ms, function_pointer_t callback)
{
	me.timer[timer_id].current_value = 0;
	me.timer[timer_id].trig_value = delay_ms;
	me.timer[timer_id].callback = callback;
	me.timer[timer_id].retriggable = false;
	me.timer[timer_id].running = true;
}

void os_set_cyclic_timer(enum timers timer_id, uint16_t delay_ms, function_pointer_t callback)
{
	me.timer[timer_id].current_value = 0;
	me.timer[timer_id].trig_value = delay_ms;
	me.timer[timer_id].callback = callback;
	me.timer[timer_id].retriggable = true;
	me.timer[timer_id].running = true;
}

void os_retrig_timer(enum timers timer_id)
{
	me.timer[timer_id].current_value = 0;
	me.timer[timer_id].running = true;
}

void os_stop_timer(enum timers timer_id)
{
	me.timer[timer_id].running = false;
}

bool os_timer_is_running(enum timers timer_id)
{
	return me.timer[timer_id].running;
}

void os_exec(function_pointer_t function)
{
	me.exec_stack.function_pointer[me.exec_stack.stack_index++] = function;
}
