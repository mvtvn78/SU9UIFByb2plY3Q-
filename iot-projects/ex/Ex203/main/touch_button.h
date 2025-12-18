#include "stdint.h"

#define TOUCH_GPIO 20

typedef enum {
    TOUCH_EVT_SINGLE,
    TOUCH_EVT_DOUBLE,
    TOUCH_EVT_LONG
} touch_evt_t;

typedef void (*touch_btn_callback_t)(touch_evt_t evt, int64_t timestamp_ms);

void touch_btn_init(touch_btn_callback_t cb);
void touch_btn_task(void *arg);
