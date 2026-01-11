#ifndef CLOCK_H
#define CLOCK_H

#include <cstdint>
#include <Arduino.h>
#include "tm1637.h"
#include "alarm_tone.h"

enum ButtonType
{
  BUTTON_MENU,
  BUTTON_PLUS,
  BUTTON_MINUS,
  BUTTON_OK,
};

// Menu states
enum MenuState
{
    MENU_SHOW_TIME,
    MENU_SET_HOUR,
    MENU_SET_MIN,
    MENU_SET_ALARM_HOUR,
    MENU_SET_ALARM_MIN
};

class Clock
{
private:
    // 7-segment Display
    TM1637* display = NULL;

    // Timer variable to count time
    hw_timer_t *timer = NULL;

    // Buzzer pin and alarm tone
    uint8_t buzzer_pin;
    AlarmTone alarm_tone;
    
    // Time variables (binary encoded: 5 bits hour, 6 bits min, 6 bits sec)
    uint32_t current_time;
    uint32_t alarm_time;
    
    // Alarm state
    bool alarm_enabled;
    bool alarm_ringing;
    unsigned long alarm_start_time;
    
    // Menu state
    MenuState menu_state;
    
    // Timer counters
    uint8_t half_second_counter;
    bool colon_on;
    
    // Display blink state for alarm
    bool display_blink_state;

public:
    // Constructor
    Clock();

    // Init function
    void init(TM1637* display, uint8_t buzzer_pin);
    
    // Set time and alarm time
    void set_time(uint8_t hour, uint8_t minutes, uint8_t seconds);
    void set_alarm(uint8_t hour, uint8_t minutes);
    
    // Button pressed function
    void button_pressed(ButtonType type);
    
    // Alarm functions
    void turn_alarm(bool on_off);
    void check_alarm();

    // Clock functions 
    void show();
    void run();

    friend void update_time();
};

#endif