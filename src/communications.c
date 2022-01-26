#include "communications.h"

void UART_run()
{
    int output_length = 19;                                             //the number of characters in each output message
    char out[output_length + 1];                                        //buffer to store output message. Needs extra index at end or sprintf has a fit I think.

    float lati;                                                         //example variable storing sensor reading. These would normally be global and if they are non-int/char/float converting them in the sensor's read funciton might be best

    sprintf(out,"gps-lati:%09.5f\n",lati);                              //output message to be sent -> can be copypasted to to make messages for any (int/char/float/etc.) global variable, just not sure how many we'll need yet. Might need edits if float value is >999
    uart_write_bytes(UART_NUM_0, (const char *) out, output_length);    //outputs buffer to port0, each character is 1 byte as per ASCII, output must have exactly the same number of characters as the output_length variable
}

void I2C_run()
{
    
}