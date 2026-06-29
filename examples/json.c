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
        while (1)
        {
            if (!imp_string(NULL)) break;
            printf("key - ok\n");
            if (!imp_colon(NULL)) { /* @todo: report error */ return 0; }
            printf("colon - ok\n");
            if (!parse_json()) { /* @todo: report error */ return 0; }
            printf("sub-json - ok\n");
            if (!imp_comma(NULL)) break;
            printf("comma - ok\n");
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

    int ok = parse_json();
    printf("end parse (%s)\n", ok ? "success" : "failure");

    return 0;
}
