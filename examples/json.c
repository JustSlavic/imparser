#include <stdio.h>

#define IMPARSER_IMPLEMENTATION
#include "../imparser.h"

char const source_code[] =
"{\n"
"    \"first\": null,\n"
"    \"second\": true,\n"
"    \"third\": 1234,\n"
"    \"fourth\": 12.34,\n"
"    \"fifth\": \"This is a string\",\n"
"    \"sixth\": { \"sub\": \"it works\" },\n"
"    \"seventh\": [ null, false, 12.34, \"This is a string\" ]\n"
"}\n"
;


int parse_json(void)
{
    if (imp_brace_open(NULL))
    {
        /* This is an object { ... } */
        while (true)
        {
            if (!imp_string(NULL)) break;
            if (!imp_colon(NULL)) { /* @todo: report error */ return 0; }
            if (!parse_json()) { /* @todo: report error */ return 0; }
            if (!imp_comma(NULL)) break;
        }

        if (imp_brace_close(NULL))
        {
            /* @todo: report error */
            return 1;
        }
    }
    return 0;
}

int main()
{
    imp_begin(source_code, sizeof(source_code));

    if (parse_json()) { printf("ok!\n"); }
    printf("end parse.\n");

    return 0;
}
