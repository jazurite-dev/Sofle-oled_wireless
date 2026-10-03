#include <zephyr/init.h>
#include <zephyr/kernel.h>

#include <zmk/event_manager.h>
#include <zmk/events/usb_conn_state_changed.h>
#include <zmk/rgb_underglow.h>
#include <zmk/usb.h>

static void rgb_usb_only_apply(struct k_work *work) {
    if (!zmk_usb_is_powered()) {
        // The LEDs hold whatever they latched at power-up, and nothing blanks them while
        // RGB is already marked off, so always write the off frame.
        zmk_rgb_underglow_off();
        return;
    }

    bool on;
    if (zmk_rgb_underglow_get_state(&on) == 0 && !on) {
        zmk_rgb_underglow_on();
    }
}

static K_WORK_DELAYABLE_DEFINE(rgb_usb_only_work, rgb_usb_only_apply);

static int rgb_usb_only_listener(const zmk_event_t *eh) {
    k_work_reschedule(&rgb_usb_only_work, K_MSEC(100));
    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(rgb_usb_only, rgb_usb_only_listener);
ZMK_SUBSCRIPTION(rgb_usb_only, zmk_usb_conn_state_changed);

// The saved RGB on/off state is restored after init, so re-check once it has loaded.
static int rgb_usb_only_init(void) {
    k_work_reschedule(&rgb_usb_only_work, K_SECONDS(3));
    return 0;
}

SYS_INIT(rgb_usb_only_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
