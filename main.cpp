#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "ssd1306.h"


int main(int argc, char **argv)
{
    uint8_t rc = 0;
    
    // open the I2C device node
    uint8_t i2c_node_address = 1;
    rc = ssd1306_init(i2c_node_address);
    
    if (rc != 0)
    {
        printf("no oled attached to /dev/i2c-%d\n", i2c_node_address);
        return 1;
    }
    
    // init oled module
    rc += ssd1306_oled_default_config(64, 128);
    
    // clear display
    rc += ssd1306_oled_clear_screen();

    // set rotate orientation
    rc += ssd1306_oled_set_rotate(0);
    
   
    // set cursor XY
    rc += ssd1306_oled_set_XY(1, 1);

    // print text
    char line[25] = {0};
    strncpy(line, "Hello", sizeof(line));
    rc += ssd1306_oled_write_line(0, line);
 
    // close the I2C device node
    ssd1306_end();
    
    return rc;
}