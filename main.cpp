#include <iostream>
#include <cstring>
// For stringstream
#include <sstream>
// For setprecision
#include <iomanip>
#include <thread>
#include <stdint.h>
#include <stdlib.h>
#include <signal.h>
#include <inttypes.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "ssd1306.h"
#include "linux_i2c.h"
#include "HTU21D.h"

using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::strcpy;
using std::to_string;
using std::stringstream;
using std::fixed;
using std::setprecision;

uint8_t showText(string text)
{
    char * line = new char [text.length()+1];
    strcpy(line, text.c_str());
    return ssd1306_oled_write_line(SSD1306_FONT_NORMAL, line);
}

void cleanup(int signal_number)
{
    ssd1306_oled_clear_screen();
    // close the I2C device node
    ssd1306_end();
    exit(0);
}

int main(int argc, char **argv)
{
    uint8_t rc = 0;

    // Handle Ctrl-C
    signal(SIGINT, cleanup);
    // Handle other cases that lead to termination
    signal(SIGKILL, cleanup);
    signal(SIGTERM, cleanup);
    
    // open the I2C device node
    uint8_t i2c_node_address = 1;
    rc = _i2c_init_bus(i2c_node_address);
    if (rc != 0)
    {
        cerr << "Unable to open I2C bus /dev/i2c-" << i2c_node_address << endl;
        return 1;
    }

    // Initialize display and show text on it
    rc = ssd1306_init(i2c_node_address);
    if (rc != 0)
    {
        cerr << "No OLED display attached to /dev/i2c-" << i2c_node_address << endl;
        ssd1306_end();
        return 1;
    }

    // init oled module
    rc += ssd1306_oled_default_config(64, 128);
    
    // clear display
    rc += ssd1306_oled_clear_screen();

    // set rotate orientation
    rc += ssd1306_oled_set_rotate(0);

    while (true) {
        // Retrieve temperature and humidity
        double temperature = 0;
        double humidity = 0;
        int i2cfile = get_i2c_file();
        if ( (0 > getHumidity(i2cfile, &humidity)) || (0 > getTemperature(i2cfile, &temperature)) )
        {
            cerr << "ERROR: HTU21D sensor module not found" << endl;
            ssd1306_init(i2c_node_address);
            ssd1306_oled_clear_screen();
            ssd1306_end();
            exit(-1);
        }

        // Initialize display
        rc = ssd1306_init(i2c_node_address);
        if (rc != 0)
        {
            cerr << "ERROR: no oled attached to /dev/i2c-" << i2c_node_address << endl;
            return 1;
        }
                
        // Show title
        rc += ssd1306_oled_set_XY(1, 0);
        rc += showText("Thermometer");

        // Empty line
        rc += ssd1306_oled_set_XY(1, 1);
        rc += showText("");
        
        // Temperature on 2 lines
        rc += ssd1306_oled_set_XY(1, 2);
        rc += showText("Temperature:");
        rc += ssd1306_oled_set_XY(1, 3);
        rc += ssd1306_oled_clear_line(3);
        stringstream textTemperature;
        textTemperature << fixed << setprecision(2) << temperature << "C";
        rc += showText(textTemperature.str());
        // Print temperature and humidity on the screen
        cout << "Temperature: " << textTemperature.str() << endl;

        // Empty line
        rc += ssd1306_oled_set_XY(1, 4);
        rc += showText("");

        // Humidity on 2 lines
        rc += ssd1306_oled_set_XY(1, 5);
        rc += showText("Humidity:");
        rc += ssd1306_oled_clear_line(6);
        rc += ssd1306_oled_set_XY(1, 6);
        stringstream textHumidity;
        textHumidity << fixed << setprecision(0) << humidity << "%";
        rc += showText(textHumidity.str());
        // Print humidity on the screen
        cout << "Humidity: " << textHumidity.str() << endl;

        sleep(1);
    }
}
