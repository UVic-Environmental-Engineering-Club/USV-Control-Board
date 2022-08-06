#include "communications.h"

int accelx,accely,accelz;
int gyrx,gyry,gyrz;
int lid1,lid2,lid3;
int heading;
int rpm1,rpm2;
int tmp1,tmp2;
int wet1,wet2;
int longitude,latitude;
int mpr;
int ran;

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
    int input_length = 10;
    uint8_t in[input_length];
    in[0] = 'a'; in[9] = '\n';
    char confirmation[23];

    // sprintf(out,"ACC:%03d:%03d:%03d\n",accelx,accely,accelz);                              //output message to be sent -> can be copypasted to to make messages for any (int/char/float/etc.) global variable, just not sure how many we'll need yet. Might need edits if float value is >999
    // uart_write_bytes(UART_NUM_0, (const char *) out, output_length);    //outputs buffer to port0, each character is 1 byte as per ASCII, output must have exactly the same number of characters as the output_length variable

    // sprintf(out,"GYR:%03d:%03d:%03d\n",gyrx,gyry,gyrz);                              
    // uart_write_bytes(UART_NUM_0, (const char *) out, output_length);

    // sprintf(out,"LID:%03d:%03d:%03d\n",lid1,lid2,lid3);                              
    // uart_write_bytes(UART_NUM_0, (const char *) out, output_length);

    // sprintf(out,"COM:%03d\n",heading);                              
    // uart_write_bytes(UART_NUM_0, (const char *) out, output_length-8);

    // sprintf(out,"RPM:%03d:%03d\n",rpm1,rpm2);                              
    // uart_write_bytes(UART_NUM_0, (const char *) out, output_length-4);

    // sprintf(out,"TMP:%03d:%03d\n",tmp1,tmp2);                              
    // uart_write_bytes(UART_NUM_0, (const char *) out, output_length-4);

    // sprintf(out,"WET:%03d:%03d\n",wet1,wet2);                              
    // uart_write_bytes(UART_NUM_0, (const char *) out, output_length-4);

    // sprintf(gps_buffer,"GPS:%09d:%09d\n",longitude,latitude);                              
    // uart_write_bytes(UART_NUM_0, (const char *) gps_buffer, 24);

    size_t number_of_bytes = 0;
    uart_get_buffered_data_len(UART_NUM_0, &number_of_bytes);
    printf("%d\n", number_of_bytes);
    // printf("%d\n", uart_read_bytes(UART_NUM_0, in, input_length, 100/ portTICK_PERIOD_MS));
    // uart_write_bytes(UART_NUM_0, (char *)in, input_length);
    // if(in[0] == 'M' && in[1] == 'P' && in[2] == 'R' && in[3] == ':'){
    //     unsigned int read = 0;
    //     read = isno(in[4])*100 + isno(in[5])*10 + isno(in[6]);
    //     if(read > 100){
    //         sprintf(confirmation, "err, invalid motor   \n");
    //     }else{
    //         mpr = read;
    //         sprintf(confirmation, "motor updated to %03d \n", mpr);
    //     }
    //     uart_write_bytes(UART_NUM_0, (const char *) confirmation, 22);
    // }else if(in[0] == 'R' && in[1] == 'A' && in[2] == 'N' && in[3] == ':'){
    //     unsigned int read = 0;
    //     read = isno(in[4])*100 + isno(in[5])*10 + isno(in[6]);
    //     if(read > 360){
    //         sprintf(confirmation, "err, invalid rudder  \n");
    //     }else{
    //         ran = read;
    //         sprintf(confirmation, "rudder updated to %03d\n", ran);
    //     }
    //     uart_write_bytes(UART_NUM_0, (const char *) confirmation, 22);
    // }
}

void I2C_run()
{
    //accelerometer_run();
    
}