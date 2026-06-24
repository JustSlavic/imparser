#include <stdio.h>

#define IMPARSER_IMPLEMENTATION
#include "../imparser.h"

char const source_code[] =
"if (condition)\n"
;

int main()
{

    imp_begin(source_code, sizeof(source_code));

    if (imp_comma(NULL))
    {
        printf("ident\n");
    }
    printf("end parse.\n");

    return 0;
}
