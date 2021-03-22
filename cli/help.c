/******************************************************************************************
 * Help command                                                                           *
 * Copyleft DamieFC                                                                       *
 ******************************************************************************************/

#include "prompt.h"

int main(){
    printf("%s", prompt);
    scanf("%s", input);
    if (input == "help"){
        printf("I'm not sure what you need help with, so I'm giving you a prompt. Type what you need help with there.");
        printf("Options for help: os (OS help), prompt (prompt help)");
        char hprompt = "Cozen help>> "
        printf("%s", hprompt);
        char hinput
        scanf("%s", hinput);
        if (hinput == "os"){
            printf("This OS is Cozen. It's in-development, with a few drivers and stuff.");
            return 0;
        }
        else if (hinput == "prompt"){
            printf("This CLI is (currently, this is in development) the main way of interacting with the OS.");
            return 0;
        }
    }
}
