#include "communications.h"

int isno( char in ) { //Checks if an input char is a digit, returns that digit as an int. If not, returns 500. used in UART_run()
    if(in >= 48 && in <= 57){
        return in - 48;
    }
    return 500;
}

void UART_run()
{
    int output_length = 16;                                             //the number of characters in each output message
    char out[output_length + 1];                                        //buffer to store output message. Needs extra index at end or sprintf adds an endline character over message.
    char gps_buffer[24+1];
    int input_length = 7;
    char in[input_length + 1];
    in[0] = 'a'; in[5] = 'a';
    char confirmation[23];
    int mpr;
    int ran;

    sprintf(out,"ACC-%03d-%03d-%03d\n",accelx,accely,accelz);                              //output message to be sent -> can be copypasted to to make messages for any (int/char/float/etc.) global variable, just not sure how many we'll need yet. Might need edits if float value is >999
    uart_write_bytes(UART_NUM_0, (const char *) out, output_length);    //outputs buffer to port0, each character is 1 byte as per ASCII, output must have exactly the same number of characters as the output_length variable

    sprintf(out,"GYR-%03d-%03d-%03d\n",gyrx,gyry,gyrz);                              
    uart_write_bytes(UART_NUM_0, (const char *) out, output_length);

    sprintf(out,"LID-%03d-%03d-%03d\n",lid1,lid2,lid3);                              
    uart_write_bytes(UART_NUM_0, (const char *) out, output_length);

    sprintf(out,"COM-%03d\n",heading);                              
    uart_write_bytes(UART_NUM_0, (const char *) out, output_length-8);

    sprintf(out,"RPM-%03d-%03d\n",rpm1,rpm2);                              
    uart_write_bytes(UART_NUM_0, (const char *) out, output_length-4);

    sprintf(out,"TMP-%03d-%03d\n",tmp1,tmp2);                              
    uart_write_bytes(UART_NUM_0, (const char *) out, output_length-4);

    sprintf(out,"WET-%03d-%03d\n",wet1,wet2);                              
    uart_write_bytes(UART_NUM_0, (const char *) out, output_length-4);

    sprintf(gps_buffer,"GPS-%09d-%09d\n",longitude,latitude);                              
    uart_write_bytes(UART_NUM_0, (const char *) gps_buffer, 24);

    uart_read_bytes(UART_NUM_0, in, input_length, 500/ portTICK_PERIOD_MS);
    if(in[0] == 'm' && in[1] == 'p' && in[2] == 'r' && in[3] == '-'){
        unsigned int read = 0;
        read = isno(in[4])*100 + isno(in[5])*10 + isno(in[6]);
        if(read > 100){
            sprintf(confirmation, "err, invalid motor   \n");
        }else{
            mpr = read;
            sprintf(confirmation, "motor updated to %03d \n", mpr);
        }
        uart_write_bytes(UART_NUM_0, (const char *) confirmation, 22);
    }else if(in[0] == 'r' && in[1] == 'a' && in[2] == 'n' && in[3] == '-'){
        unsigned int read = 0;
        read = isno(in[4])*100 + isno(in[5])*10 + isno(in[6]);
        if(read > 360){
            sprintf(confirmation, "err, invalid rudder  \n");
        }else{
            ran = read;
            sprintf(confirmation, "rudder updated to %03d\n", ran);
        }
        uart_write_bytes(UART_NUM_0, (const char *) confirmation, 22);
    }
}

void I2C_run()
{
    
}