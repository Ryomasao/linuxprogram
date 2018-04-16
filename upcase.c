#include <stdio.h>


int main()
{
    char temp[] = "aiueo\0";
    char* temp2;


/*
    // *chで宣言するとreadonlの文字列になるかも
    //https://stackoverflow.com/questions/23687437/segmentation-fault-cause-by-toupper
    char ch[] = "ai\0";    
    char* test;    

    *ch = (char)toupper((int)*ch);
    printf("%c\n", (int)*ch);

    for(;;){
        if(*temp == '\0') break;

        printf("%c\n", *temp);

        temp++;
    }

*/
    /*
    while(*temp != '\0'){
        printf("%c\n", *temp);
    }
    */

    for(temp2 = temp; *temp2; temp2++){
        *temp2 = (char)toupper((int)*temp2);
        printf("%c\n", *temp2);
    }

    return 0;
}
