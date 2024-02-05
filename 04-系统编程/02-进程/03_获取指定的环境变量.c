#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    const char* javaHome = getenv("HOME");
    printf("%s \n",javaHome); // /Users/zero

    const char* custom = getenv("CUSTOM");
    printf("%s \n",custom); // (null)

    putenv();
    return 0;
}
