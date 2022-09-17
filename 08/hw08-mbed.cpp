#include "mbed.h"

#define MAX_PERIODS 7
#define MAX_TIME 100

DigitalOut my_led(LED1);
InterruptIn my_button(USER_BUTTON);
DigitalIn btn_long(USER_BUTTON);

Ticker led_ticker;
Ticker btn_ticker;
Ticker btn_long_ticker;

bool press_ignore = false;

void clear_ignore();
void flip();
void pressed();
void control_long_btn();

int count_press = 0;
int count_periods = 0;
int period = 1;
float per_arr[] = {1.0, 0.5, 0.4, 0.3, 0.2, 0.1, 0.05}; // periods in seconds

// detach -> odpojeni funkce od Tickeru (napr. prestane blikani ... )
// attach -> pripojeni funkce Tickeru (napr. blikani zacne ... )

int main() 
{            
   led_ticker.attach(&flip, per_arr[count_periods]); // spusteni blikani poprve
   my_button.fall(&pressed);                         // .fall for while loop
}


void flip()       // function that switches between on and off LED
{
    my_led = !my_led;  // switches from on to off and vise versa from off to on again and again
    printf("\rtik\n");
}


void clear_ignore()
{
    press_ignore = false;
}


void control_long_btn()                    // function for controlling long press
{
    if (btn_long == 1)                     // 0 = pressed, 1 = free
    {                                      // if button not pressed -> end the function
        btn_long_ticker.detach();          // ending
        return;
    }
    
    count_press++;  
    
    if (count_press >= MAX_TIME)           // button pressed for longer than 200ms
    {                                      // reset period to 1s
        btn_long_ticker.detach();
        led_ticker.detach();
        period = 1.0;                      // set period to default
        count_periods = 0;                 // array indexing back to zero
        count_press = 0;
        led_ticker.attach(&flip, period);  // start again with default settings
    }
}


void pressed()                             // changes the periods of light signals
{
    if (!press_ignore)
    {
        led_ticker.detach();               // disconnect led, stops blicking
        
        if (count_periods < MAX_PERIODS)
        {
            led_ticker.attach(&flip, per_arr[count_periods]);  
            count_periods++;
        }
        else
        {
            my_led = 1;
        }
        
        press_ignore = true;
        btn_ticker.attach(&clear_ignore, 0.25); // tolerance for not super quick press
        btn_long_ticker.attach(&control_long_btn, 0.01);
    }
}




