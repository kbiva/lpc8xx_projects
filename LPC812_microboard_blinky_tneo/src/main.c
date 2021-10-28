/*
 *  main.c
 *
 *  LPC812 microboard and eight LEDs.
 *  One LED is blinking every second.
 *  The other LEDs are blinking with following periods:
 *  50ms,100ms,200ms,400ms,800ms,1600ms and 3200ms.
 *  Using real-time kernel TNeo. Each LED is controlled by separate task (total 8 tasks).
 *
 *  Author: Kestutis Bivainis
 */

#include "chip.h"
#include "tn.h"

#define LED1_PIN 17
#define LED2_PIN 14
#define LED3_PIN 0
#define LED4_PIN 6
#define LED5_PIN 7
#define LED6_PIN 8
#define LED7_PIN 9
#define LED8_PIN 1

//-- system frequency
#define SYS_FREQ           12000000L

//-- kernel ticks (system timer) frequency
#define SYS_TMR_FREQ       1000

//-- system timer period (auto-calculated)
#define SYS_TMR_PERIOD              \
   (SYS_FREQ / SYS_TMR_FREQ)

//-- idle task stack size, in words
#define IDLE_TASK_STACK_SIZE      (TN_MIN_STACK_SIZE + 16)

//-- interrupt stack size, in words
#define INTERRUPT_STACK_SIZE      (TN_MIN_STACK_SIZE + 32)

//-- stack sizes of user tasks
#define TASK_LED1_STK_SIZE        (TN_MIN_STACK_SIZE + 32)
#define TASK_LED2_STK_SIZE        (TN_MIN_STACK_SIZE + 32)
#define TASK_LED3_STK_SIZE        (TN_MIN_STACK_SIZE + 32)
#define TASK_LED4_STK_SIZE        (TN_MIN_STACK_SIZE + 32)
#define TASK_LED5_STK_SIZE        (TN_MIN_STACK_SIZE + 32)
#define TASK_LED6_STK_SIZE        (TN_MIN_STACK_SIZE + 32)
#define TASK_LED7_STK_SIZE        (TN_MIN_STACK_SIZE + 32)
#define TASK_LED8_STK_SIZE        (TN_MIN_STACK_SIZE + 32)

//-- user task priorities
#define TASK_LED1_PRIORITY        7
#define TASK_LED2_PRIORITY        6
#define TASK_LED3_PRIORITY        6
#define TASK_LED4_PRIORITY        6
#define TASK_LED5_PRIORITY        6
#define TASK_LED6_PRIORITY        6
#define TASK_LED7_PRIORITY        6
#define TASK_LED8_PRIORITY        6

/*******************************************************************************
 *    DATA
 ******************************************************************************/

//-- Allocate arrays for stacks: stack for idle task
//   and for interrupts are the requirement of the kernel;
//   others are application-dependent.
//
//   We use convenience macro TN_STACK_ARR_DEF() for that.

TN_STACK_ARR_DEF(idle_task_stack, IDLE_TASK_STACK_SIZE);
TN_STACK_ARR_DEF(interrupt_stack, INTERRUPT_STACK_SIZE);

TN_STACK_ARR_DEF(task_LED1_stack, TASK_LED1_STK_SIZE);
TN_STACK_ARR_DEF(task_LED2_stack, TASK_LED2_STK_SIZE);
TN_STACK_ARR_DEF(task_LED3_stack, TASK_LED3_STK_SIZE);
TN_STACK_ARR_DEF(task_LED4_stack, TASK_LED4_STK_SIZE);
TN_STACK_ARR_DEF(task_LED5_stack, TASK_LED5_STK_SIZE);
TN_STACK_ARR_DEF(task_LED6_stack, TASK_LED6_STK_SIZE);
TN_STACK_ARR_DEF(task_LED7_stack, TASK_LED7_STK_SIZE);
TN_STACK_ARR_DEF(task_LED8_stack, TASK_LED8_STK_SIZE);

//-- task structures

struct TN_Task task_LED1;
struct TN_Task task_LED2;
struct TN_Task task_LED3;
struct TN_Task task_LED4;
struct TN_Task task_LED5;
struct TN_Task task_LED6;
struct TN_Task task_LED7;
struct TN_Task task_LED8;

/*******************************************************************************
 *    ISRs
 ******************************************************************************/

/**
 * system timer ISR
 */
void SysTick_Handler(void) {

  tn_tick_int_processing();
}

extern uint32_t SystemCoreClock;

void appl_init(void);

void task_LED1_body(void *par) {

  //-- this is a first created application task, so it needs to perform
  //   all the application initialization.
  appl_init();

  //-- and then, let's get to the primary job of the task
  //   (job for which task was created at all)
  for(;;) {
    Chip_GPIO_SetPinToggle(LPC_GPIO_PORT, 0, LED1_PIN);
    tn_task_sleep(1000);
  }
}

void task_LED2_body(void *par) {

  for(;;) {
    Chip_GPIO_SetPinToggle(LPC_GPIO_PORT, 0, LED2_PIN);
    tn_task_sleep(50);
  }
}

void task_LED3_body(void *par) {

  for(;;) {
    Chip_GPIO_SetPinToggle(LPC_GPIO_PORT, 0, LED3_PIN);
    tn_task_sleep(100);
  }
}

void task_LED4_body(void *par) {

  for(;;) {
    Chip_GPIO_SetPinToggle(LPC_GPIO_PORT, 0, LED4_PIN);
    tn_task_sleep(200);
  }
}

void task_LED5_body(void *par) {

  for(;;) {
    Chip_GPIO_SetPinToggle(LPC_GPIO_PORT, 0, LED5_PIN);
    tn_task_sleep(400);
  }
}

void task_LED6_body(void *par) {

  for(;;) {
    Chip_GPIO_SetPinToggle(LPC_GPIO_PORT, 0, LED6_PIN);
    tn_task_sleep(800);
  }
}

void task_LED7_body(void *par) {

  for(;;) {
    Chip_GPIO_SetPinToggle(LPC_GPIO_PORT, 0, LED7_PIN);
    tn_task_sleep(1600);
  }
}

void task_LED8_body(void *par) {

  for(;;) {
    Chip_GPIO_SetPinToggle(LPC_GPIO_PORT, 0, LED8_PIN);
    tn_task_sleep(3200);
  }
}

/**
 * Hardware init: called from main() with interrupts disabled
 */
void hw_init(void) {

  // Read clock settings and update SystemCoreClock variable
  SystemCoreClockUpdate();
  //-- init system timer
  SysTick_Config(SYS_TMR_PERIOD);
}

/**
 * Application init: called from the first created application task
 */
void appl_init(void) {

  //-- configure LED port pins
  Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_GPIO);
  Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, 0, LED1_PIN);
  Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, 0, LED2_PIN);
  Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, 0, LED3_PIN);
  Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, 0, LED4_PIN);
  Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, 0, LED5_PIN);
  Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, 0, LED6_PIN);
  Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, 0, LED7_PIN);
  Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, 0, LED8_PIN);

  //-- initialize various on-board peripherals, such as
  //   flash memory, displays, etc.
  //   (in this sample project there's nothing to init)

  //-- initialize various program modules
  //   (in this sample project there's nothing to init)

  //-- create all the rest application tasks

  tn_task_create(&task_LED2, task_LED2_body, TASK_LED2_PRIORITY, task_LED2_stack, TASK_LED2_STK_SIZE, NULL, (TN_TASK_CREATE_OPT_START));
  tn_task_create(&task_LED3, task_LED3_body, TASK_LED3_PRIORITY, task_LED3_stack, TASK_LED3_STK_SIZE, NULL, (TN_TASK_CREATE_OPT_START));
  tn_task_create(&task_LED4, task_LED4_body, TASK_LED4_PRIORITY, task_LED4_stack, TASK_LED4_STK_SIZE, NULL, (TN_TASK_CREATE_OPT_START));
  tn_task_create(&task_LED5, task_LED5_body, TASK_LED5_PRIORITY, task_LED5_stack, TASK_LED5_STK_SIZE, NULL, (TN_TASK_CREATE_OPT_START));
  tn_task_create(&task_LED6, task_LED6_body, TASK_LED6_PRIORITY, task_LED6_stack, TASK_LED6_STK_SIZE, NULL, (TN_TASK_CREATE_OPT_START));
  tn_task_create(&task_LED7, task_LED7_body, TASK_LED7_PRIORITY, task_LED7_stack, TASK_LED7_STK_SIZE, NULL, (TN_TASK_CREATE_OPT_START));
  tn_task_create(&task_LED8, task_LED8_body, TASK_LED8_PRIORITY, task_LED8_stack, TASK_LED8_STK_SIZE, NULL, (TN_TASK_CREATE_OPT_START));
}

//-- idle callback that is called periodically from idle task
void idle_task_callback (void) {
  // put MCU to sleep
   __WFI();
}

//-- create first application task(s)
void init_task_create(void) {

  //-- task A performs complete application initialization,
  //   it's the first created application task
  tn_task_create(
         &task_LED1,                   //-- task structure
         task_LED1_body,               //-- task body function
         TASK_LED1_PRIORITY,           //-- task priority
         task_LED1_stack,              //-- task stack
         TASK_LED1_STK_SIZE,           //-- task stack size (in words)
         NULL,                      //-- task function parameter
         TN_TASK_CREATE_OPT_START   //-- creation option
         );
}

int main(void) {

  //-- unconditionally disable interrupts
  tn_arch_int_dis();

  //-- init hardware
  hw_init();

  //-- call to tn_sys_start() never returns
  tn_sys_start(
         idle_task_stack,
         IDLE_TASK_STACK_SIZE,
         interrupt_stack,
         INTERRUPT_STACK_SIZE,
         init_task_create,
         idle_task_callback
         );

  //-- unreachable
  return 1;
}
