#include "mbed.h"

#define REPEAT 5       

DigitalOut my_led(LED1);

Ticker led_ticker;

Serial serial(SERIAL_TX, SERIAL_RX);       // serial communication

void start();
void flip();

int main() 
{
    start();
    
    float periods[] = {0.05, 0.1, 0.2, 0.5, 1};  // period array

    while (1)
    {
        while (!serial.readable()) {};           // while not read for reading -> wait
    
        char c = serial.getc();                  // load the char
        bool ok = true;
        switch(c)
        {
            case 'b':                            // base case
                led_ticker.detach();
                my_led = 0;
                while (!serial.writeable()) {};
                serial.putc('b');
                break;
            case 'e':                            // LED off
                led_ticker.detach();
                my_led = 0;
                break;
            case 'h':                            // hello
                while (!serial.writeable()) {};
                serial.putc('h');
                break;
            case 's':                            // LED on   
                led_ticker.detach();
                my_led = 1;
                break;
            default:
                if (c >= '1' && c <= '5')        // set period
                {
                    led_ticker.attach(&flip, periods[c - '1']);
                }
                else
                    ok = false;
        }
        while (!serial.writeable()) {};         // while not read for writing -> wait
        if (c != 'h' && c!= 'b')
        {
            serial.putc(ok ? 'a' : 'f');
        }
        if (c == 'b')
        {
            exit(0);
        }
    }
}

void start()
{
    serial.baud(115200);
    for (int i = 0; i < REPEAT*2; i++)
    {
        my_led = !my_led;
        wait(0.1);
    }
    while (serial.readable());                   // read eveything one by one
    {
        serial.getc();                           // read char
    } 
    my_led = 0;
    serial.putc('i');
}

void flip()
{
    my_led = !my_led;                           // switch from on to off and vice versa
    while (!serial.writeable()) {};  
    if (my_led)                                 // send signal char 
    {
        serial.putc('x');                   
    } 
    else 
    {
        serial.putc('o');
    }           
}



