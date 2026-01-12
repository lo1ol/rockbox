#include "plugin.h"

#include "lib/display_text.h"
#include "lib/pluginlib_actions.h"
#include "lib/pluginlib_exit.h"

static int inv_freq=45;
static int form=222;
static int period=3000;

bool lock = false;

void do_click(void) {
#ifndef SIMULATOR
        PWM0_CTRL = 0x80000000 | (unsigned int)(form << 16) | inv_freq;

        unsigned long long piezo_usec_off = USEC_TIMER + period;
        while (TIME_BEFORE(USEC_TIMER, piezo_usec_off));
        
        PWM0_CTRL = 0;
#endif
}

void print_info(void) {
    rb->lcd_clear_display();
    if (lock)
        rb->lcd_puts(0, 1, "lock");

    char buffer[64];

    rb->snprintf(buffer, sizeof(buffer), "Time: %d", period);
    rb->lcd_puts(0, 2, buffer);

    rb->snprintf(buffer, sizeof(buffer), "Form: %d", form);
    rb->lcd_puts(0, 3, buffer);
    
    rb->snprintf(buffer, sizeof(buffer), "Inv freq: %d", inv_freq);
    rb->lcd_puts(0, 4, buffer);
    
    rb->lcd_update();

}

static int set_period(void)
{
    print_info();

    while(true) {
        int button = rb->button_get(true);

        if (button == BUTTON_SELECT)
            lock = !lock;

        if (!lock) {
            if (button & BUTTON_SCROLL_FWD) {
                period += 10;
            } else if (button & BUTTON_SCROLL_BACK && period != 0) {
                period -= 10;
            } else if (button == BUTTON_MENU) {
                return 0;
            }
        }

        print_info();
        do_click();
    }
    return 0;
}

static int set_form(void)
{
    print_info();

    while(true) {
        int button = rb->button_get(true);

        if (button == BUTTON_SELECT)
            lock = !lock;

        if (!lock) {
            if (button & BUTTON_SCROLL_FWD && form != 256) {
                ++form;
            } else if (button & BUTTON_SCROLL_BACK && form != 0) {
                --form;
            } else if (button == BUTTON_MENU) {
                return 0;
            }
        }

        print_info();
        do_click();
    }
    return 0;
}

static int set_inv_freq(void)
{
    print_info();

    while(true) {
        int button = rb->button_get(true);

        if (button == BUTTON_SELECT)
            lock = !lock;

        if (!lock) {
            if (button & BUTTON_SCROLL_FWD) {
                if (inv_freq < 100)
                    ++inv_freq;
                else if (inv_freq < 1000)
                    inv_freq += 100;
                else if (inv_freq < 10000)
                    inv_freq += 1000;
                else
                    inv_freq += 10000;
            } else if (button & BUTTON_SCROLL_BACK && inv_freq != 0) {
                if (inv_freq <= 100)
                    --inv_freq;
                else if (inv_freq <= 1000)
                    inv_freq -= 100;
                else if (inv_freq <= 10000)
                    inv_freq -= 1000;
                else
                    inv_freq -= 10000;
            } else if (button == BUTTON_MENU) {
                return 0;
            }
        }

        print_info();
        do_click();
    }
    return 0;
}

/* this is the plugin entry point */
enum plugin_status plugin_start(const void* parameter)
{
    (void)parameter;

#ifndef SIMULATOR
    DEV_INIT1 &= ~0xc;
    DEV_EN |= DEV_PWM;
#endif

    MENUITEM_STRINGLIST(menu, "check haptic", NULL,
                        "inv_freq",
                        "form",
                        "period",
                        "Quit");

    bool quit = false;
    int sel = 0;
    while(!quit)
    {
        switch(rb->do_menu(&menu, &sel, NULL, false))
        {
        case 0:
            set_inv_freq();
            break;
        case 1:
            set_form();
            break;
        case 2:
            set_period();
            break;
        case 3:
            quit = 1;
            break;
        default:
            break;
        }
    }

    return PLUGIN_OK;
}
