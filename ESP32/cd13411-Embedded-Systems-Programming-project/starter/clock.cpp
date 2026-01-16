#include "clock.h"
#include "stdio.h"

#define HOUR_MASK   0x1F000  // 5 bits for hour (bits 12-16)
#define MIN_MASK    0x0FC0   // 6 bits for minutes (bits 6-11)
#define SEC_MASK    0x003F   // 6 bits for seconds (bits 0-5)
#define HOUR_SHIFT  12
#define MIN_SHIFT   6
#define SEC_SHIFT   0

#define OFF_DISPLAY_TIME 3000

Clock* g_clock_instance = nullptr;

// Static function: Update time, show things on display and check alarm trigger
void IRAM_ATTR update_time()
{
    if (!g_clock_instance) return;
    
    Clock* clk = g_clock_instance;
    
    clk->half_second_counter++;
    clk->colon_on = !clk->colon_on;
    
    clk->display_blink_state = !clk->display_blink_state;
    
    if (clk->half_second_counter >= 2) {
        clk->half_second_counter = 0;
        
        uint8_t hour = (clk->current_time & HOUR_MASK) >> HOUR_SHIFT;
        uint8_t min = (clk->current_time & MIN_MASK) >> MIN_SHIFT;
        uint8_t sec = (clk->current_time & SEC_MASK) >> SEC_SHIFT;
        
        sec++;
        if (sec >= 60) {
            sec = 0;
            min++;
            if (min >= 60) {
                min = 0;
                hour++;
                if (hour >= 24) {
                    hour = 0;
                }
            }
        }
        
        clk->current_time = ((uint32_t)hour << HOUR_SHIFT) | 
                           ((uint32_t)min << MIN_SHIFT) | 
                           ((uint32_t)sec << SEC_SHIFT);
    }
    
    clk->show();
    clk->check_alarm();
}

Clock::Clock() : display(nullptr), timer(nullptr), buzzer_pin(0), 
                 current_time(0), alarm_time(0), alarm_enabled(false),
                 alarm_ringing(false), alarm_start_time(0), 
                 menu_state(MENU_SHOW_TIME), half_second_counter(0),
                 colon_on(true), display_blink_state(true),
                 off_display_start_time(0), showing_off(false) {}

// Clock::init(): Initialize internal variables, set display to use and buzzer pin
void Clock::init(TM1637* display, uint8_t buzzer_pin)
{ 
    this->display = display;
    this->buzzer_pin = buzzer_pin;
    this->alarm_tone.init(buzzer_pin);
    this->current_time = 0;
    this->alarm_time = 0;
    this->alarm_enabled = false;
    this->alarm_ringing = false;
    this->menu_state = MENU_SHOW_TIME;
    this->half_second_counter = 0;
    this->colon_on = true;
    this->display_blink_state = true;
    this->off_display_start_time = 0;
    this->showing_off = false;
    
    g_clock_instance = this;
}

// Clock::set_time(): Set the time hour, minutes and seconds to internal binary representation
void Clock::set_time(uint8_t hour, uint8_t minutes, uint8_t seconds)
{
    current_time = ((uint32_t)hour << HOUR_SHIFT) | 
                   ((uint32_t)minutes << MIN_SHIFT) | 
                   ((uint32_t)seconds << SEC_SHIFT);
}

// Clock::set_alarm(): Set the alarm hour and minutes to internal binary representation
void Clock::set_alarm(uint8_t hour, uint8_t minutes)
{
    alarm_time = ((uint32_t)hour << HOUR_SHIFT) | 
                 ((uint32_t)minutes << MIN_SHIFT);
}

// Clock::button_pressed(): Tell the internal clock that a button was pressed
void Clock::button_pressed(ButtonType type) 
{
    if (alarm_ringing && type == BUTTON_OK) {
        alarm_ringing = false;
        alarm_tone.stop();
        menu_state = MENU_SHOW_TIME;
        display_blink_state = true;
        return;
    }
    
    uint8_t hour = (current_time & HOUR_MASK) >> HOUR_SHIFT;
    uint8_t min = (current_time & MIN_MASK) >> MIN_SHIFT;
    uint8_t alarm_hour = (alarm_time & HOUR_MASK) >> HOUR_SHIFT;
    uint8_t alarm_min = (alarm_time & MIN_MASK) >> MIN_SHIFT;
    
    switch (type) {
        case BUTTON_MENU:
            // SHOW_TIME -> SHOW_SET -> SHOW_AL -> SHOW_TIME
            if (menu_state == MENU_SHOW_TIME) {
                menu_state = MENU_SHOW_SET;
            } else if (menu_state == MENU_SHOW_SET) {
                menu_state = MENU_SHOW_AL;
            } else if (menu_state == MENU_SHOW_AL) {
                menu_state = MENU_SHOW_TIME;
            } else {
                // If in any setting mode, go back to showing time
                menu_state = MENU_SHOW_TIME;
            }
            showing_off = false;
            break;
            
        case BUTTON_PLUS:
            if (menu_state == MENU_SET_HOUR) {
                hour = (hour + 1) % 24;
                current_time = ((uint32_t)hour << HOUR_SHIFT) | (current_time & ~HOUR_MASK);
            } else if (menu_state == MENU_SET_MIN) {
                min = (min + 1) % 60;
                current_time = ((uint32_t)min << MIN_SHIFT) | (current_time & ~MIN_MASK);
            } else if (menu_state == MENU_SET_ALARM_HOUR) {
                alarm_hour = (alarm_hour + 1) % 24;
                alarm_time = ((uint32_t)alarm_hour << HOUR_SHIFT) | (alarm_time & ~HOUR_MASK);
            } else if (menu_state == MENU_SET_ALARM_MIN) {
                alarm_min = (alarm_min + 1) % 60;
                alarm_time = ((uint32_t)alarm_min << MIN_SHIFT) | (alarm_time & ~MIN_MASK);
            }
            break;
            
        case BUTTON_MINUS:
            if (menu_state == MENU_SET_HOUR) {
                hour = (hour == 0) ? 23 : hour - 1;
                current_time = ((uint32_t)hour << HOUR_SHIFT) | (current_time & ~HOUR_MASK);
            } else if (menu_state == MENU_SET_MIN) {
                min = (min == 0) ? 59 : min - 1;
                current_time = ((uint32_t)min << MIN_SHIFT) | (current_time & ~MIN_MASK);
            } else if (menu_state == MENU_SET_ALARM_HOUR) {
                alarm_hour = (alarm_hour == 0) ? 23 : alarm_hour - 1;
                alarm_time = ((uint32_t)alarm_hour << HOUR_SHIFT) | (alarm_time & ~HOUR_MASK);
            } else if (menu_state == MENU_SET_ALARM_MIN) {
                alarm_min = (alarm_min == 0) ? 59 : alarm_min - 1;
                alarm_time = ((uint32_t)alarm_min << MIN_SHIFT) | (alarm_time & ~MIN_MASK);
            }
            break;
            
        case BUTTON_OK:
            if (menu_state == MENU_SHOW_SET) {
                menu_state = MENU_SET_HOUR;
            } else if (menu_state == MENU_SET_HOUR) {
                menu_state = MENU_SET_MIN;
            } else if (menu_state == MENU_SET_MIN) {
                menu_state = MENU_SHOW_TIME;
            } else if (menu_state == MENU_SHOW_AL) {
                if (alarm_enabled) {
                    menu_state = MENU_SET_ALARM_HOUR;
                } else {
                    showing_off = true;
                    off_display_start_time = millis();
                }
            } else if (menu_state == MENU_SET_ALARM_HOUR) {
                menu_state = MENU_SET_ALARM_MIN;
            } else if (menu_state == MENU_SET_ALARM_MIN) {
                menu_state = MENU_SHOW_TIME;
            } else {
                menu_state = MENU_SHOW_TIME;
            }
            break;
    }
    
    show();
}

// Clock::turn_alarm(): Enable or disable alarm
void Clock::turn_alarm(bool on_off)
{
    alarm_enabled = on_off;
    if (!on_off && alarm_ringing) {
        alarm_ringing = false;
        alarm_tone.stop();
        display_blink_state = true;
    }
}

// Clock::show(): Show time or menu on display
void Clock::show()
{
    if (!display) return;
    
    if (showing_off) {
        if (millis() - off_display_start_time >= OFF_DISPLAY_TIME) {
            showing_off = false;
            menu_state = MENU_SHOW_TIME;
        }
    }
    
    uint8_t hour, min;
    
    if (alarm_ringing) {
        if (display_blink_state) {
            hour = (current_time & HOUR_MASK) >> HOUR_SHIFT;
            min = (current_time & MIN_MASK) >> MIN_SHIFT;
            
            display->display(0, hour / 10);
            display->display(1, hour % 10);
            display->display(2, min / 10);
            display->display(3, min % 10);
            display->point(true);
        } else {
            display->clearDisplay();
        }
        return;
    }
    
    if (showing_off) {
        display->display(0, 0x7f);
        display->display(1, '0');
        display->display(2, 'F');
        display->display(3, 'F');
        display->point(false);
        return;
    }
    
    switch (menu_state) {
        case MENU_SHOW_TIME:
            hour = (current_time & HOUR_MASK) >> HOUR_SHIFT;
            min = (current_time & MIN_MASK) >> MIN_SHIFT;
            
            display->display(0, hour / 10);
            display->display(1, hour % 10);
            display->display(2, min / 10);
            display->display(3, min % 10);
            
            display->point(colon_on);
            break;
            
        case MENU_SHOW_SET:
            // Set hour mode
            display->display(0, 'S');
            display->display(1, 'E');
            display->display(2, 't');
            display->display(3, 0x7f); 
            display->point(false);
            break;
            
        case MENU_SHOW_AL:
            // Alarm mode
            display->display(0, 'A');
            display->display(1, 'L');
            display->display(2, 0x7f);
            display->display(3, 0x7f);
            display->point(false);
            break;
            
        case MENU_SET_HOUR:
            hour = (current_time & HOUR_MASK) >> HOUR_SHIFT;
            min = (current_time & MIN_MASK) >> MIN_SHIFT;
            
            if (display_blink_state) {
                display->display(0, hour / 10);
                display->display(1, hour % 10);
            } else {
                display->display(0, 0x7f);
                display->display(1, 0x7f);
            }
            
            display->display(2, min / 10);
            display->display(3, min % 10);
            display->point(true);
            break;
            
        case MENU_SET_MIN:
            hour = (current_time & HOUR_MASK) >> HOUR_SHIFT;
            min = (current_time & MIN_MASK) >> MIN_SHIFT;
            
            display->display(0, hour / 10);
            display->display(1, hour % 10);
            
            if (display_blink_state) {
                display->display(2, min / 10);
                display->display(3, min % 10);
            } else {
                display->display(2, 0x7f);
                display->display(3, 0x7f);
            }
            
            display->point(true);
            break;
            
        case MENU_SET_ALARM_HOUR:
            hour = (alarm_time & HOUR_MASK) >> HOUR_SHIFT;
            min = (alarm_time & MIN_MASK) >> MIN_SHIFT;
            
            if (display_blink_state) {
                display->display(0, hour / 10);
                display->display(1, hour % 10);
            } else {
                display->display(0, 0x7f);
                display->display(1, 0x7f);
            }
            
            display->display(2, min / 10);
            display->display(3, min % 10);
            display->point(true);
            break;
            
        case MENU_SET_ALARM_MIN:
            hour = (alarm_time & HOUR_MASK) >> HOUR_SHIFT;
            min = (alarm_time & MIN_MASK) >> MIN_SHIFT;
            
            display->display(0, hour / 10);
            display->display(1, hour % 10);
            
            if (display_blink_state) {
                display->display(2, min / 10);
                display->display(3, min % 10);
            } else {
                display->display(2, 0x7f);
                display->display(3, 0x7f);
            }
            
            display->point(true);
            break;
    }
}

// Clock::check_alarm(): Check if alarm needs to be triggered
void Clock::check_alarm()
{
    if (!alarm_enabled) return;
    
    uint8_t current_hour = (current_time & HOUR_MASK) >> HOUR_SHIFT;
    uint8_t current_min = (current_time & MIN_MASK) >> MIN_SHIFT;
    uint8_t current_sec = (current_time & SEC_MASK) >> SEC_SHIFT;
    
    uint8_t alarm_hour = (alarm_time & HOUR_MASK) >> HOUR_SHIFT;
    uint8_t alarm_min = (alarm_time & MIN_MASK) >> MIN_SHIFT;
    
    if (!alarm_ringing && current_hour == alarm_hour && 
        current_min == alarm_min && current_sec == 0) {
        alarm_ringing = true;
        alarm_start_time = millis();
        display_blink_state = true;
    }
    
    if (alarm_ringing) {
        alarm_tone.play();
        
        if (millis() - alarm_start_time >= 30000) {
            alarm_ringing = false;
            alarm_tone.stop();
            display_blink_state = true;
        }
    }
}

// Clock::run(): Start running the clock
void Clock::run()
{
    timer = timerBegin(1000000);
    
    timerAttachInterrupt(timer, &update_time);
    timerAlarm(timer, 500000, true, 0);
}