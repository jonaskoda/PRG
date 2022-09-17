#include "mbed.h"
DigitalOut myled(LED1);
Ticker ledticker;
Serial serial(SERIAL_TX, SERIAL_RX);

void flip() {
    myled = !myled;
    while (!serial.writeable()) {}
    serial.putc('m');
}
int main() {
    for (int i = 0; i < 5 * 2; ++i) {
        myled = !myled;
        wait(0.5);
    }
    serial.baud(115200);
    while (serial.readable())
        serial.getc();
    serial.putc('i');
    float periods[] = {0.05, 0.1, 0.2, 0.5, 1.0};
    while (1) {
        while (!serial.readable()) {}
        char c = serial.getc();
        bool ok = true;
        switch (c) {
            case 's':
                ledticker.detach();
                myled = 1;
                break;
            case 'e':
                ledticker.detach();
                myled = 0;
                break;
            case 'h':
                while (!serial.writeable()) {}
                serial.putc('h');
                break;
            default:
                if (c >= '1' && c <= '5')
                    ledticker.attach(&flip, periods[c - '1']);
                else
                    ok = false;
        }
        while (!serial.writeable()) {}
        serial.putc(ok ? 'a' : 'f');
    }
}