#include "clock.h"
#include "stdio.h"

#define HOUR_MASK   0x1F000  // 5 bits for hour (bits 12-16)
#define MIN_MASK    0x0FC0   // 6 bits for minutes (bits 6-11)
#define SEC_MASK    0x003F   // 6 bits for seconds (bits 0-5)
#define HOUR_SHIFT  12
#define MIN_SHIFT   6
#define SEC_SHIFT   0

// Static function: Update time, show things on display
//                  and check alarm trigger
static void update_time(void* clock)
{
    Clock* clk = (Clock*)clock;
    
    clk->half_second_counter++;
    clk->colon_on = !clk->colon_on;
    
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

// Empty constructor
Clock::Clock() : display(nullptr), timer(nullptr), buzzer_pin(0), 
                 current_time(0), alarm_time(0), alarm_enabled(false),
                 alarm_ringing(false), alarm_start_time(0), 
                 menu_state(MENU_SHOW_TIME), half_second_counter(0),
                 colon_on(true) {}

// Clock::init(): Initialize internal variables,
//                set display to use and buzzer pin
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
}

// Clock::set_time(): Set the time hour, minutes and seconds
//                    to internal binary representation
void Clock::set_time(uint8_t hour, uint8_t minutes, uint8_t seconds)
{
    current_time = ((uint32_t)hour << HOUR_SHIFT) | 
                   ((uint32_t)minutes << MIN_SHIFT) | 
                   ((uint32_t)seconds << SEC_SHIFT);
}

// Clock::set_alarm(): Set the alarm hour, minutes and seconds
//                     to internal binary representation
void Clock::set_alarm(uint8_t hour, uint8_t minutes)
{
    alarm_time = ((uint32_t)hour << HOUR_SHIFT) | 
                 ((uint32_t)minutes << MIN_SHIFT);
}

// Clock::button_pressed(): Tell the internal clock that a button was pressed
//                        type: The button that was pressed
void Clock::button_pressed(ButtonType type) 
{
    if (alarm_ringing && type == BUTTON_OK) {
        alarm_ringing = false;
        alarm_tone.stop();
        menu_state = MENU_SHOW_TIME;
        return;
    }
    
    uint8_t hour = (current_time & HOUR_MASK) >> HOUR_SHIFT;
    uint8_t min = (current_time & MIN_MASK) >> MIN_SHIFT;
    uint8_t alarm_hour = (alarm_time & HOUR_MASK) >> HOUR_SHIFT;
    uint8_t alarm_min = (alarm_time & MIN_MASK) >> MIN_SHIFT;
    
    switch (type) {
        case BUTTON_MENU:
            menu_state = (MenuState)((menu_state + 1) % 5);
            break;
            
        case BUTTON_UP:
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
            
        case BUTTON_DOWN:
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
            menu_state = MENU_SHOW_TIME;
            break;
    }
}

// Clock::turn_alarm(): Enable or disable alarm
void Clock::turn_alarm(bool on_off)
{
    alarm_enabled = on_off;
}

// Clock::show(): Show time or menu on display
void Clock::show()
{
    if (!display) return;
    
    uint8_t hour, min;
    
    switch (menu_state) {
        case MENU_SHOW_TIME:
            hour = (current_time & HOUR_MASK) >> HOUR_SHIFT;
            min = (current_time & MIN_MASK) >> MIN_SHIFT;
            display->showNumberDecEx(hour * 100 + min, colon_on ? 0b01000000 : 0, true);
            break;
            
        case MENU_SET_HOUR:
        case MENU_SET_MIN:
            hour = (current_time & HOUR_MASK) >> HOUR_SHIFT;
            min = (current_time & MIN_MASK) >> MIN_SHIFT;
            display->showNumberDecEx(hour * 100 + min, 0b01000000, true);
            break;
            
        case MENU_SET_ALARM_HOUR:
        case MENU_SET_ALARM_MIN:
            hour = (alarm_time & HOUR_MASK) >> HOUR_SHIFT;
            min = (alarm_time & MIN_MASK) >> MIN_SHIFT;
            display->showNumberDecEx(hour * 100 + min, 0b01000000, true);
            break;
    }
}

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
        alarm_tone.play();
    }
    
    if (alarm_ringing) {
        if (millis() - alarm_start_time >= 30000) {
            alarm_ringing = false;
            alarm_tone.stop();
        }
    }
}

// Clock::run(): Start running the clock
//               This function MUST not block, everything should be handled
//               by interrupts
void Clock::run()
{
    timer = timerBegin(0, 80, true);
    
    timerAttachInterrupt(timer, &update_time, true);
    
    timerAlarmWrite(timer, 500000, true);
    
    timerAlarmEnable(timer);
}