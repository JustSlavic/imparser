#include <stdio.h>

#define IMPARSER_IMPLEMENTATION
#include "../imparser.h"

char const source_code[] =
"{\n"
"    \"first\": null,\n"
"    \"second\": true,\n"
"    \"third\": 1234,\n"
"    \"fifth\": \"This is a string\",\n"
"    \"sixth\": { \"sub\": \"it works\" },\n"
"    \"seventh\": [ null, false, 1234, \"This is a string\" ]\n"
"}\n"
;

enum
{
    TOKEN_NULL,
    TOKEN_TRUE,
    TOKEN_FALSE,
};

static char const *spaces = "                                                             ";
int parse_json(int depth)
{
    if (imp_extension(TOKEN_NULL, NULL))
    {
        printf("%.*snull\n", depth, spaces);
        return 1;
    }
    else if (imp_extension(TOKEN_TRUE, NULL) || imp_extension(TOKEN_FALSE, NULL))
    {
        printf("%.*sbool\n", depth, spaces);
        return 1;
    }
    else if (imp_integer(NULL))
    {
        printf("%.*sint\n", depth, spaces);
        return 1;
    }
    else if (imp_string(NULL))
    {
        printf("%.*sstring\n", depth, spaces);
        return 1;
    }
    else if (imp_brace_open(NULL))
    {
        printf("%.*sobject\n", depth, spaces);
        while (1)
        {
            if (!imp_string(NULL)) { break; }
            if (!imp_colon(NULL)) { /* @todo: report error */ return 0; }
            if (!parse_json(depth + 2)) { /* @todo: report error */ return 0; }
            if (!imp_comma(NULL)) { break; }
        }

        if (imp_brace_close(NULL))
        {
            return 1;
        }
    }
    else if (imp_bracket_open(NULL))
    {
        printf("%.*slist\n", depth, spaces);
        if (imp_bracket_close(NULL)) { return 1; }
        while (1)
        {
            if (!parse_json(depth + 2)) { /* @todo: report error */ return 0; }

            /*
               Json doesn't support trailing commas, so we have to write additional code
               to return 0, when there's a trailing comma. Isn't it just bad design?
            */
            int comma_exists = imp_comma(NULL);
            int close_bracket = imp_bracket_close(NULL);
            if (comma_exists && close_bracket) { /* @todo: report error */ return 0; }
            if (close_bracket) { break; }
        }
        return 1;
    }
    return 0;
}

int main()
{
    imp_begin(source_code, sizeof(source_code));
    imp_extend("null", TOKEN_NULL);
    imp_extend("true", TOKEN_TRUE);
    imp_extend("false", TOKEN_FALSE);

    int ok = parse_json(0);
    printf("end parse (%s)\n", ok ? "success" : "failure");

    return 0;
}
