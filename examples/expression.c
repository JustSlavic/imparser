#include <stdio.h>

#define IMPARSER_IMPLEMENTATION
#include "../imparser.h"

/*
    This is the example of an expression parser.

    It just recognizes that an expression is valid. It does not build AST.
    If you want to build an AST from it, you should just add a return parameter
    something like:

        int parse_expression(ast_t **expr, int precedence);

    Then you should allocate nodes in parse_expression_operand,
    and in parse_expression right at the end of the loop, where
    the rhs of the expression is parsed succesfully.

    This program uses Pratt's [?citation needed] algorithm to determine operator precedence.
    It parses the expression and just prints it in the Reverse Polish Notation.
*/

static char const source_code[] = "a + b * (140 - c) / 2 + d";

int parse_expression_operand(void)
{
    imp_token t;
    if (imp_identifier(&t))
    {
        /* Variable */
        printf("%.*s ", (int) t.span.size, t.span.data);
        return 1;
    }
    else if (imp_integer(&t))
    {
        /* A number */
        printf("%.*s ", (int) t.span.size, t.span.data);
        return 1;
    }

    printf("Error: Could not parse expression operand.\n");
    return 0;
}

int parse_expression(int precedence)
{
    if (imp_paren_open(NULL))
    {
        if (!parse_expression(0))
        {
            printf("Error: Could not parse expression inside parenthesis.\n");
            return 0;
        }

        if (!imp_paren_close(NULL))
        {
            printf("Error: There's open parenthesis, but no close parenthesis.\n");
            return 0;
        }

        return 1;
    }

    if (!parse_expression_operand())
    {
        printf("Error: Could not parse left hand side of the expression.\n");
        return 0;
    }

    while (1)
    {
        int operator_precedence = 0;
        imp_checkpoint checkpoint = imp_save();
        imp_token operator;
        if (imp_plus(&operator) || imp_minus(&operator))
        {
            operator_precedence = 1;
        }
        else if (imp_asterisk(&operator) || imp_slash(&operator))
        {
            operator_precedence = 2;
        }
        else
        {
            /* There's no operator, the expression has ended. */
            break;
        }

        if (operator_precedence < precedence)
        {
            /* Operator precedence requires for the rest of the expression be handled by the while loop higher up on the recursion. */
            imp_restore(checkpoint);
            break;
        }

        /* +1 for left associativity, +0 for right associativity */
        if (!parse_expression(operator_precedence + 1))
        {
            printf("Error: Could not parse right hand side of the expression.\n");
            return 0;
        }

        printf("%.*s ", (int) operator.span.size, operator.span.data);
    }

    return 1;
}

int main()
{
    imp_begin(source_code, sizeof(source_code));

    int success = parse_expression(0);
    printf("\nend parse (%s)\n", success ? "success" : "failure");

    return 0;
}
