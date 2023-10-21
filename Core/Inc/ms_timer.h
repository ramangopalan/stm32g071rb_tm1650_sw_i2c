
#ifndef MILLIS_TIMER_H
#define MILLIS_TIMER_H

#include <main.h>
#include <stdbool.h>

typedef enum MILLIS_TIMER_STATE_t {
  RUNNING,
  STOPPED,
  EXPIRED
} MILLIS_TIMER_STATE_t;

/* Millis timer structure */
typedef struct millis_timer_t {
  uint8_t id;
  uint32_t m_target_time;
  uint32_t m_remaining_time;
  uint32_t m_interval;
  uint32_t m_repeat;
  uint32_t m_repeat_count;
  int (*cb_on_expired)(void *);
  MILLIS_TIMER_STATE_t m_state;
} millis_timer_t;

/* Initialization function. */
void millis_timer_init (millis_timer_t *t, uint32_t interval, int (*handler)(void *data));

/* Is the timer running? */
bool millis_timer_is_running (millis_timer_t *t);

/* Run the timer */
void millis_timer_run (millis_timer_t *t);

/* Is the timer expired? */
bool millis_timer_expired (millis_timer_t *t);

/* Stop timer. */
void millis_timer_stop (millis_timer_t *t);

/* Start the timer. */
void millis_timer_start (millis_timer_t *t);

/* Start from a specific time provided. */
void millis_timer_start_from (millis_timer_t *t, uint32_t start_time);

/* Arbitrarily set the target time. */
void millis_timer_set_target_time (millis_timer_t *t, uint32_t target_time);

/* Reset the timer. Stop, and reset repeat count */
void millis_timer_reset (millis_timer_t *t);

/* Set timer interval */
void millis_timer_set_interval (millis_timer_t *t, uint32_t interval);

/* Set number of timer repeats */
void millis_timer_set_repeats (millis_timer_t *t, uint32_t repeat_count);

/* Set timer handler callback */
void millis_timer_expired_handler (millis_timer_t *t, int (*handler)(void *data));

/* Get timer target time */
uint32_t millis_timer_get_target_time (millis_timer_t *t);

/* Get timer elapsed/remaining time */
uint32_t millis_timer_get_remaining_time (millis_timer_t *t);

/* How many more repeats are there? */
uint32_t millis_timer_get_remaining_repeats (millis_timer_t *t);

#endif /* #ifndef MILLIS_TIMER_H */
