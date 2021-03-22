/*******************************************************************************
 * Hello.c: hello command                                                      *
 * Copyleft DamieFC                                                            *
 *******************************************************************************/

#include "prompt.h"

int main(void){
    printf("%s", prompt);
    scanf("%s", input);
    if (input == "hello"){
        printf("Oh, uh, hello to you too?");
        return 0;
    }
}
