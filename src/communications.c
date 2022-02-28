#include "communications.h"

void UART_run()
{
    int output_length = 16;                                             //the number of characters in each output message
    char out[output_length + 1];                                        //buffer to store output message. Needs extra index at end or sprintf adds an endline character over message.
    char gps_buffer[23+1];

    sprintf(out,"ACC-%3d-%3d-%3d\n",accelx,accely,accelz);                              //output message to be sent -> can be copypasted to to make messages for any (int/char/float/etc.) global variable, just not sure how many we'll need yet. Might need edits if float value is >999
    uart_write_bytes(UART_NUM_0, (const char *) out, output_length);    //outputs buffer to port0, each character is 1 byte as per ASCII, output must have exactly the same number of characters as the output_length variable

    sprintf(out,"GYR-%3d-%3d-%3d\n",gyrx,gyry,gyrz);                              
    uart_write_bytes(UART_NUM_0, (const char *) out, output_length);

    sprintf(out,"LID-%3d-%3d-%3d\n",lid1,lid2,lid3);                              
    uart_write_bytes(UART_NUM_0, (const char *) out, output_length);

    sprintf(out,"COM-%3d\n",heading);                              
    uart_write_bytes(UART_NUM_0, (const char *) out, output_length-8);

    sprintf(out,"RPM-%3d-%3d-%3d\n",rpm1,rpm2);                              
    uart_write_bytes(UART_NUM_0, (const char *) out, output_length);

    sprintf(out,"TMP-%3d-%3d\n",tmp1,tmp2);                              
    uart_write_bytes(UART_NUM_0, (const char *) out, output_length);

    sprintf(out,"WET-%3d-%3d\n",wet1,wet2);                              
    uart_write_bytes(UART_NUM_0, (const char *) out, output_length);

    sprintf(gps_buffer,"GPS-%9d-%9d\n",longitude,latitude);                              
    uart_write_bytes(UART_NUM_0, (const char *) out, 23);
}

void I2C_run()
{
    
}