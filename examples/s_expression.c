#include <stdio.h>

#define IMPARSER_IMPLEMENTATION
#include "../imparser.h"

/*
    S-expression example, just print it differently.
*/

static char const source_code[] = "(foo bar () baz (one two (three)))";

char const spaces[] = "                                             ";
int parse_s_expression(int depth)
{
    if (imp_paren_open(NULL))
    {
        printf("%.*s(\n", depth * 2, spaces);
        int ok = 1;
        while (ok)
        {
            imp_token ident;
            if (imp_paren_close(NULL))
            {
                printf("%.*s)\n", depth * 2, spaces);
                break;
            }
            else if (imp_paren_open_peek(NULL))
                ok = parse_s_expression(depth + 1);
            else if (imp_identifier(&ident))
                printf("%.*s%.*s\n", (depth + 1) * 2, spaces, (int) ident.span.size, ident.span.data);
            else
                ok = 0;
        }

        return ok;
    }
    return 0;
}

int main()
{
    imp_begin(source_code, sizeof(source_code));

    int success = parse_s_expression(0);
    printf("\nend parse (%s)\n", success ? "success" : "failure");

    return 0;
}
