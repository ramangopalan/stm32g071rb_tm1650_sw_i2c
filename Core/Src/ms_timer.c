
#include <ms_timer.h>

volatile unsigned long millis_counter;

/* Link the actual implementation of millis here. Perhaps, we
 * use SysTick OR another system timer. In Raman's case, he's
 * using another system timer.
 */
void millis_interrupt (void) {
  millis_counter++;
}

unsigned long millis (void) {
  return HAL_GetTick();
}

// Initialization function.
void millis_timer_init (millis_timer_t *t, uint32_t interval, int (*handler)(void *data)) {
  t->m_interval = interval;
  t->m_state = STOPPED;                       // Stopped
  t->m_repeat = t->m_repeat_count = 0;        // Repeat indefinitely
  t->m_target_time = millis() + interval;
  t->m_remaining_time = 0;
  t->cb_on_expired = handler;
}

bool millis_timer_is_running (millis_timer_t *t) {
  return (t->m_state == RUNNING);
}

void millis_timer_run (millis_timer_t *t) {
  millis_timer_expired(t);
}

bool millis_timer_expired (millis_timer_t *t) {
  // Only if we're running
  if (t->m_state == RUNNING) {
    // If we have passed the target time...
    if (millis() >= t->m_target_time) {
      // Calculate repeat. If repeat = 0, then we
      // repeat forever until stopped.
      // Otherwise, when we've hit the last repeat (1),
      // then we stop.
      if (t->m_repeat_count != 1) {
        if (t->m_repeat_count > 0) {
          t->m_repeat_count--;
        }
        // Set the new target (based on our last target time
        // for accuracy)
        t->m_target_time += t->m_interval;
      } else {
        t->m_state = EXPIRED;
      }

      // Fire the call back.
      if (t->cb_on_expired) {
        t->cb_on_expired(NULL);
      }

      return true;
    } else {
      return false;
    }
  } else {
    return false;
  }
}

void millis_timer_stop (millis_timer_t *t) {
  t->m_state = STOPPED;
  
  // If we have stopped before the target time,
  // let's save the remaining time so we can resume later.
  if (millis() < t->m_target_time) {
    t->m_remaining_time = t->m_target_time - millis();
  }
}

// Start the timer.
void millis_timer_start (millis_timer_t *t) {
  millis_timer_start_from(t, millis());
}

// Start from a specific time provided.
void millis_timer_start_from (millis_timer_t *t, uint32_t start_time) {
  t->m_state = RUNNING;
  // If we have some remaining time, then let's use that.
  if (t->m_remaining_time > 0) {
    t->m_target_time = start_time + t->m_remaining_time;
    t->m_remaining_time = 0;
  } else {
    // otherwise, we start normally
    t->m_target_time = start_time + t->m_interval;
  }
}

// Arbitrarily set the target time.
void millis_timer_set_target_time (millis_timer_t *t, uint32_t target_time) {
  t->m_target_time = target_time;
}

// Reset the timer. Stop, and reset repeat count.
void millis_timer_reset (millis_timer_t *t) {
  t->m_state = STOPPED;
  t->m_remaining_time = 0;
  t->m_repeat_count = t->m_repeat;
}

void millis_timer_set_interval (millis_timer_t *t, uint32_t interval) {
  t->m_interval = interval;
}

void millis_timer_set_repeats (millis_timer_t *t, uint32_t repeat_count) {
  t->m_repeat = t->m_repeat_count = repeat_count;
}

void millis_timer_expired_handler (millis_timer_t *t, int (*handler)(void *data)) {
  t->cb_on_expired = handler;
}

uint32_t millis_timer_get_target_time (millis_timer_t *t) {
  return t->m_target_time;
}

uint32_t millis_timer_get_remaining_time (millis_timer_t *t)  {
  if (t->m_state == RUNNING) {
    return t->m_target_time - millis();
  } else {
    return t->m_remaining_time;
  }
}

uint32_t millis_timer_get_remaining_repeats (millis_timer_t *t) {
  if (t->m_state == EXPIRED && t->m_repeat_count == 1)
    return 0;
  else
    return t->m_repeat_count;
}

