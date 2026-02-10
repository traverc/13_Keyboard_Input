#include <stdio.h>
#include "freertos/FreeRTOS.h"


#define MAX_LENGTH 50       // Maximum string length (including '\0')
#define NO_CHAR 255         // Returned by getchar() when buffer is empty
#define LOOP_TIME_MS 50     // Loop delay in msec


char ret_string[MAX_LENGTH];
char prompt[MAX_LENGTH] = "min and max duty (usec): ";


bool string_ready = false;
bool task_over = false;


void user_input()
{
    int num = 0;                            // Index into string
   
    printf("\n%s", prompt);
   
    while (!task_over) {
       
        char ch = getchar();                // Read a character from keyboard.
       
        if (ch != NO_CHAR){                 // If it's a valid character,
            printf("%c", ch);               //  echo it to the monitor.
           
            if (ch != '\n'){                // If it's NOT a newline,
                ret_string[num] = ch;       //  store it and
                num = num + 1;              //  increment the index.
            } else {                        // If it IS a newline,
                ret_string[num] = '\0';     //  terminate current string and
                num = 0;                    //  reset index for next string.
               
                printf("            You entered: %s\n", ret_string);
                printf("\n%s", prompt);
                string_ready = true;
            }
        }
       
        vTaskDelay(LOOP_TIME_MS / portTICK_PERIOD_MS);        
    }


    vTaskDelete(NULL);
}
