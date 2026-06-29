#include <stdio.h>

#define IMPARSER_IMPLEMENTATION
#include "../imparser.h"

/*
    Here I typed in simple parser of an if statement.
    It consists of recognizing the 'if' identifier (probably will change to keyword),
    some condition identifier, and a block with a function call (no argument, no expressions, etc.)

    This is not full-fledged language parsing, just a demonstration how you might use this library.

    I intentionally used different styles to show that this library might adapt to your own style.
*/

int parse_function_call(void)
{
    if (imp_identifier(NULL) &&
        imp_paren_open(NULL) &&
        imp_paren_close(NULL) &&
        imp_semicolon(NULL))
    {
        return 1;
    }
    return 0;
}

int parse_block(void)
{
    if (!imp_brace_open(NULL)) return 0;
    if (!parse_function_call()) return 0;
    if (!imp_brace_close(NULL)) return 0;
    return 1;
}

int parse_if_statement(void)
{
    imp_token ident, paren_open, paren_close;
    if (imp_identifier(&ident))
    {
        if (imp_span_is_equal(ident.span, (imp_span){ .data="if", .size=2 }))
        {
            if (imp_paren_open(&paren_open))
            {
                /*
                    You probably want to call into parse_expression here,
                    but because it's just an example, I haven't bother to
                    call it (the example of expression statement in another file).
                */
                imp_token condition;
                if (imp_identifier(&condition))
                {
                    if (imp_paren_close(&paren_close))
                    {
                        printf("if (%.*s)\n", (int) condition.span.size, condition.span.data);
                        if (parse_block())
                        {
                            printf("    { block }\n");
                            return 1;
                        }
                        else
                        {
                            printf("Failed to parse {block}\n");
                            return 0;
                        }
                    }
                    else
                    {
                        printf("There have to be a closing parenthesis, found %.*s instead at %d:%d\n", (int) paren_close.span.size, paren_close.span.data, paren_close.line, paren_close.column);
                        return 0;
                    }
                }
            }
            else
            {
                printf("There have to be an open parenthesis, found %.*s instead at %d:%d\n", (int) paren_open.span.size, paren_open.span.data, paren_open.line, paren_open.column);
                return 0;
            }
        }
        else
        {
            return 0;
        }
    }
    else
    {
        printf("Expected 'if' found %.*s at %d:%d\n", (int) ident.span.size, ident.span.data, ident.line, ident.column);
        return 0;
    }
    return 0;
}

char const source_code[] =
"if (condition) {\n"
"    printf();\n"
"}\n";

enum
{
    TOKEN_IF = 1,
};

int main()
{
    imp_begin(source_code, sizeof(source_code));
    imp_extend("if", TOKEN_IF);

    int ok = parse_if_statement();
    printf("end parse (%s)\n", ok ? "success" : "faulure");

    return 0;
}
