
#include <stdio.h>
#include <stdlib.h>

#include "../inc/sscp.h"

SSCP_Handle handle;

int main(void)
{
    SSCP_init(&handle);
    SSCP_printInfo(&handle);

    return 0;
}