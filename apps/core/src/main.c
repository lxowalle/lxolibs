#include "main.h"
#include "core.h"
#include <stdio.h>

int main(int argc, char** argv)
{
    neurons_t * n = neurons_create();
    if (!n) return -1;
    printf("Create neurons ok!\n");

    return 0;
}