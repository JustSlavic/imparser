/*
    MIT License

    Copyright (c) 2026 Viacheslav Radko

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/

#ifndef IMPARSER_H_
#define IMPARSER_H_

/*
                            Immediate parser

    This library is suppose to help you to parse some simple expressions and statements.
    It's very easy to setup and use.

    How to use:
        - Copy this file into your source tree, and commit.
        - Define a preprocessor macro IMPARSER_IMPLEMENTATION, and include this file into
          some of your C files, or compile it into an object file with the compiler flag
          -DIMPARSER_IMPLEMENTATION, then link it in your build.
        - Include this file without implementation wherever you want to parse something.
        - Feed your string into the imp_begin function like that:

          imp_begin(source_code, sizeof(source_code));

        - Use the following functions in order you expect the tokens to appear in the
          string you fed into the library:

          imp_eof
          imp_paren_open
          imp_paren_close
          imp_bracket_open
          imp_bracket_close
          imp_brace_open
          imp_brace_close
          imp_colon
          imp_semicolon
          imp_plus
          imp_minus
          imp_asterisk
          imp_slash
          imp_comma
          imp_period
          imp_equals
          imp_more
          imp_less
          imp_identifier
          imp_keyword
          imp_integer

        - Each of those functions return 1 (true), if it recognized the corresponding token, and 0 (false) if not.
          Also, it accepts the optional pointer to imp_token, which is the struct containing information about this
          token, you might use it to display informative errors, or whatever you want.
*/


#include <stdint.h>
#include <string.h>

typedef struct
{
    char const *data;
    uint32_t    size;
} imp_span;

enum
{
    IMP_TOKEN_INVALID = -1,
    IMP_TOKEN_EOF = 0,

    IMP_TOKEN_PAREN_OPEN = '(',
    IMP_TOKEN_PAREN_CLOSE = ')',

    IMP_TOKEN_BRACKET_OPEN = '[',
    IMP_TOKEN_BRACKET_CLOSE = ']',

    IMP_TOKEN_BRACE_OPEN = '{',
    IMP_TOKEN_BRACE_CLOSE = '}',

    IMP_TOKEN_COLON = ':',
    IMP_TOKEN_SEMICOLON = ';',

    IMP_TOKEN_PLUS = '+',
    IMP_TOKEN_MINUS = '-',
    IMP_TOKEN_ASTERICS = '*',
    IMP_TOKEN_SLASH = '/',
    IMP_TOKEN_COMMA = ',',
    IMP_TOKEN_PERIOD = '.',

    IMP_TOKEN_EQUALS = '=',
    IMP_TOKEN_MORE = '>',
    IMP_TOKEN_LESS = '<',

    IMP_TOKEN_IDENTIFIER = 0x100, /* 256 */

    IMP_TOKEN_LITERAL_INTEGER,
    IMP_TOKEN_LITERAL_FLOAT,
    IMP_TOKEN_LITERAL_STRING,

    IMP_TOKEN_KEYWORD = 0x10000, /* 65536 */
};

char const *token_tag_to_cstring(int tag);

typedef struct
{
    int32_t tag;
    int32_t line;
    int32_t column;

    union
    {
        int64_t boolean_value;
        int64_t integer_value;
        double  float_value;
    };

    imp_span span;
} imp_token;

typedef struct
{
    char const *data;
    uint32_t    size;

    uint32_t  cursor;
    uint32_t  line;
    uint32_t  column;

    imp_token current_token;
    int       current_token_ok;

    imp_span *keywords;
    int32_t  *keyword_tags;
    uint32_t  keyword_count;
} imp_lexer;

typedef imp_lexer imp_checkpoint;
typedef int imp_function(imp_token *);

void imp_begin(char const *data, uint64_t size);

int imp_eof(imp_token *t);
int imp_paren_open(imp_token *t);
int imp_paren_close(imp_token *t);
int imp_bracket_open(imp_token *t);
int imp_bracket_close(imp_token *t);
int imp_brace_open(imp_token *t);
int imp_brace_close(imp_token *t);
int imp_colon(imp_token *t);
int imp_semicolon(imp_token *t);
int imp_plus(imp_token *t);
int imp_minus(imp_token *t);
int imp_asterisk(imp_token *t);
int imp_slash(imp_token *t);
int imp_comma(imp_token *t);
int imp_period(imp_token *t);
int imp_equals(imp_token *t);
int imp_more(imp_token *t);
int imp_less(imp_token *t);
int imp_identifier(imp_token *t);
int imp_keyword(imp_token *t);
int imp_integer(imp_token *t);

int imp_eof_peek(imp_token *t);
int imp_paren_open_peek(imp_token *t);
int imp_paren_close_peek(imp_token *t);
int imp_bracket_open_peek(imp_token *t);
int imp_bracket_close_peek(imp_token *t);
int imp_brace_open_peek(imp_token *t);
int imp_brace_close_peek(imp_token *t);
int imp_colon_peek(imp_token *t);
int imp_semicolon_peek(imp_token *t);
int imp_plus_peek(imp_token *t);
int imp_minus_peek(imp_token *t);
int imp_asterisk_peek(imp_token *t);
int imp_slash_peek(imp_token *t);
int imp_comma_peek(imp_token *t);
int imp_period_peek(imp_token *t);
int imp_equals_peek(imp_token *t);
int imp_more_peek(imp_token *t);
int imp_less_peek(imp_token *t);
int imp_identifier_peek(imp_token *t);
int imp_keyword_peek(imp_token *t);
int imp_integer_peek(imp_token *t);

int imp_peek(imp_function *f, imp_token *t);

imp_checkpoint imp_safe(void);
void imp_restore(imp_checkpoint checkpoint);

#endif /* IMPARSER_H_ */


#ifdef IMPARSER_IMPLEMENTATION

static imp_lexer imp_lexer_global;

char const *token_tag_to_cstring(int tag)
{
    switch (tag)
    {
        case IMP_TOKEN_INVALID: return "IMP_TOKEN_INVALID";
        case IMP_TOKEN_EOF: return "IMP_TOKEN_EOF";
        case IMP_TOKEN_PAREN_OPEN: return "IMP_TOKEN_PAREN_OPEN";
        case IMP_TOKEN_PAREN_CLOSE: return "IMP_TOKEN_PAREN_CLOSE";
        case IMP_TOKEN_BRACKET_OPEN: return "IMP_TOKEN_BRACKET_OPEN";
        case IMP_TOKEN_BRACKET_CLOSE: return "IMP_TOKEN_BRACKET_CLOSE";
        case IMP_TOKEN_BRACE_OPEN: return "IMP_TOKEN_BRACE_OPEN";
        case IMP_TOKEN_BRACE_CLOSE: return "IMP_TOKEN_BRACE_CLOSE";
        case IMP_TOKEN_COLON: return "IMP_TOKEN_COLON";
        case IMP_TOKEN_SEMICOLON: return "IMP_TOKEN_SEMICOLON";
        case IMP_TOKEN_PLUS: return "IMP_TOKEN_PLUS";
        case IMP_TOKEN_MINUS: return "IMP_TOKEN_MINUS";
        case IMP_TOKEN_ASTERICS: return "IMP_TOKEN_ASTERICS";
        case IMP_TOKEN_SLASH: return "IMP_TOKEN_SLASH";
        case IMP_TOKEN_COMMA: return "IMP_TOKEN_COMMA";
        case IMP_TOKEN_PERIOD: return "IMP_TOKEN_PERIOD";
        case IMP_TOKEN_EQUALS: return "IMP_TOKEN_EQUALS";
        case IMP_TOKEN_MORE: return "IMP_TOKEN_MORE";
        case IMP_TOKEN_LESS: return "IMP_TOKEN_LESS";
        case IMP_TOKEN_IDENTIFIER: return "IMP_TOKEN_IDENTIFIER";
        case IMP_TOKEN_LITERAL_INTEGER: return "IMP_TOKEN_LITERAL_INTEGER";
        case IMP_TOKEN_LITERAL_FLOAT: return "IMP_TOKEN_LITERAL_FLOAT";
        case IMP_TOKEN_LITERAL_STRING: return "IMP_TOKEN_LITERAL_STRING";
    }
    return "IMP_TOKEN_UNLISTED";
}

typedef int imp_predicate_t(char);

/* Examples of possible predicates you might use */
static int imp_ascii_is_newline(char c) { return (c == '\n'); }
static int imp_ascii_is_crlf(char c) { return (c == '\r') || (c == '\n'); }
static int imp_ascii_is_space(char c) { return (c == ' '); }
static int imp_ascii_is_whitespace(char c) { return (c == ' ') || (c == '\t') || (c == '\r') || (c == '\n'); }
static int imp_ascii_is_lowercase_alpha(char c) { return ('a' <= c) && (c <= 'z'); }
static int imp_ascii_is_uppercase_alpha(char c) { return ('A' <= c) && (c <= 'Z'); }
static int imp_ascii_is_alpha(char c) { return imp_ascii_is_uppercase_alpha(c) || imp_ascii_is_lowercase_alpha(c); }
static int imp_ascii_is_digit(char c) { return ('0' <= c) && (c <= '9'); }

static int imp_ascii_is_oct(char c) { return  ('0' <= c) && (c <= '7'); }
static int imp_ascii_is_dec(char c) { return  ('0' <= c) && (c <= '9'); }
static int imp_ascii_is_hex(char c) { return (('0' <= c) && (c <= '9')) || (('a' <= c) && (c <= 'f')) || (('A' <= c) && (c <= 'F')); }

static int imp_ascii_to_oct(char c) { return (c - '0'); }
static int imp_ascii_to_dec(char c) { return (c - '0'); }
static int imp_ascii_to_hex(char c) { return (('0' <= c) && (c <= '9')) ? (c - '0') :
                                              (('a' <= c) && (c <= 'f')) ? (c - 'a' + 10) :
                                              (('A' <= c) && (c <= 'F')) ? (c - 'A' + 10) :
                                              0xff; }

static int imp_ascii_is_valid_identifier_head(char c) { return (c == '_') || imp_ascii_is_alpha(c); }
static int imp_ascii_is_valid_identifier_body(char c) { return (c == '_') || imp_ascii_is_alpha(c) || imp_ascii_is_digit(c); }

int imp_span_is_equal(imp_span s1, imp_span s2)
{
    if (s1.size != s2.size) return 0;
    int i;
    for (i = 0; i < s1.size; i++)
    {
        if (s1.data[i] != s2.data[i]) return 0;
    }
    return 1;
}

static uint32_t imp_parse_integer(char const *data, uint64_t size, int64_t *result)
{
    char c = data[0];
    if (c == '0')
    {
        /* Either octal or hex */
        if (data[1] == 'x')
        {
            if (size < 3)
            {
                /* '0x' is not acceptable string. */
                return 0;
            }
            /* Hex */
            int64_t n = 0;
            int32_t i = 2;

            for (; i < size; i++)
            {
                c = data[i];
                if (!imp_ascii_is_hex(c)) break;

                n *= 16;
                n += imp_ascii_to_hex(c);
            }

            *result = n;
            return i;
        }
        else
        {
            /* Oct */
            int64_t n = 0;
            int32_t i = 1;

            for (; i < size; i++)
            {
                c = data[i];
                if (!imp_ascii_is_oct(c)) break;

                n *= 8;
                n += imp_ascii_to_oct(c);
            }

            *result = n;
            return i;
        }
    }
    else if (imp_ascii_is_digit(c))
    {
        /* Decimal */
        int64_t n = 0;
        int32_t i = 0;

        for (; i < size; i++)
        {
            char c = data[i];
            if (!imp_ascii_is_dec(c)) break;

            n *= 10;
            n += imp_ascii_to_dec(c);
        }

        *result = n;
        return i;
    }

    return 0;
}

static char imp_lexer_get_char(imp_lexer *lexer)
{
    char c = 0;
    if (lexer->cursor < lexer->size)
    {
        c = lexer->data[lexer->cursor];
    }
    return c;
}

static char imp_lexer_eat_char(imp_lexer *lexer)
{
    char c = imp_lexer_get_char(lexer);
    if (c != 0)
    {
        lexer->cursor += 1;
        lexer->column += 1;
    }

    if ((c == '\r') || (c == '\n'))
    {
        lexer->column = 1;
        if (c == '\n')
        {
            lexer->line += 1;
        }
    }
    return c;
}

static int imp_lexer_consume_while(imp_lexer *lexer, imp_predicate_t *p)
{
    int count = 0;
    char c = imp_lexer_get_char(lexer);
    while ((c > 0) && p(c))
    {
        imp_lexer_eat_char(lexer);
        count += 1;
        c = imp_lexer_get_char(lexer);
    }
    return count;
}

static int imp_lexer_consume_until(imp_lexer *lexer, imp_predicate_t *p)
{
    int count = 0;
    char c = imp_lexer_get_char(lexer);
    while ((c > 0) && !p(c))
    {
        imp_lexer_eat_char(lexer);
        count += 1;
        c = imp_lexer_get_char(lexer);
    }
    return count;
}

static int imp_lexer_eat_string(imp_lexer *lexer, const char *s, uint32_t n)
{
    uint32_t count = 0;
    char const *s1 = (char const *) (lexer->data + lexer->cursor);
    while ((n != 0) && (*s && *s1 && (*s == *s1)))
    {
        if (n > 0) n -= 1;
        s += 1;
        s1 += 1;
        count += 1;
    }
    if ((n == 0) || (*s == 0))
    {
        lexer->cursor += count;
        return count;
    }
    return 0;
}

static int32_t imp_lexer_find_keyword(imp_lexer *lexer, imp_span span)
{
    uint32_t keyword_index;
    for (keyword_index = 0;
         keyword_index < lexer->keyword_count;
         keyword_index++)
    {
        if (imp_span_is_equal(lexer->keywords[keyword_index], span))
        {
            return lexer->keyword_tags[keyword_index];
        }
    }
    return IMP_TOKEN_INVALID;
}

static imp_token imp_lexer_get_token(imp_lexer *lexer)
{
    if (lexer->current_token_ok)
    {
        return lexer->current_token;
    }

    imp_lexer_consume_while(lexer, imp_ascii_is_whitespace);

    imp_token t;
    memset(&t, 0, sizeof(t));
    t.tag = IMP_TOKEN_INVALID;
    t.line = lexer->line;
    t.column = lexer->column;

    char c = imp_lexer_get_char(lexer);
    if (c == 0)
    {
        t.tag = IMP_TOKEN_EOF;
    }
    else if (imp_ascii_is_valid_identifier_head(c))
    {
        t.tag = IMP_TOKEN_IDENTIFIER;
        t.span.data = (char const *) (lexer->data + lexer->cursor);
        t.span.size = imp_lexer_consume_while(lexer, imp_ascii_is_valid_identifier_body);

        int32_t keyword_tag = imp_lexer_find_keyword(lexer, t.span);
        if (keyword_tag > IMP_TOKEN_INVALID)
        {
            t.tag = keyword_tag;
        }
    }
    else if (imp_ascii_is_digit(c))
    {
        /* @todo: parse floats here too */
        t.tag = IMP_TOKEN_LITERAL_INTEGER;
        char const *s = (char const *) (lexer->data + lexer->cursor);
        uint32_t parsed_characters = imp_parse_integer(s, lexer->size - lexer->cursor, &t.integer_value);
        t.span.data = s;
        t.span.size = parsed_characters;
        lexer->cursor += parsed_characters;
    }
    else if (c == '"')
    {
        t.tag = IMP_TOKEN_LITERAL_STRING;
        char const *s = (char const *) (lexer->data + lexer->cursor);
        int saved_cursor = lexer->cursor;

        imp_lexer_eat_char(lexer);

        while (1)
        {
            c = imp_lexer_get_char(lexer);
            if (c == 0 || imp_ascii_is_crlf(c))
            {
                /* @todo: report lexer error */
                t.tag = IMP_TOKEN_INVALID;
                break;
            }
            imp_lexer_eat_char(lexer);
            if (c == '"') { break; }
            if (c == '\\') { imp_lexer_eat_char(lexer); }
        }

        t.span.data = s;
        t.span.size = lexer->cursor - saved_cursor;
    }
    else /* @todo: parse string literals */
    {
        t.tag = c;
        t.span.data = (char const *) (lexer->data + lexer->cursor);
        t.span.size = 1;

        imp_lexer_eat_char(lexer);
    }

    lexer->current_token = t;
    lexer->current_token_ok = 1;
    return t;
}

static imp_token imp_lexer_eat_token(imp_lexer *lexer)
{
    imp_token result = imp_lexer_get_token(lexer);
    lexer->current_token_ok = 0;
    return result;
}

void imp_begin(char const *data, uint64_t size)
{
    memset(&imp_lexer_global, 0, sizeof(imp_lexer_global));
    imp_lexer_global.data = data;
    imp_lexer_global.size = size;
    imp_lexer_global.line = 1;
    imp_lexer_global.column = 1;
}

static int imp_test_token(int tag, imp_token *result)
{
    imp_token t = imp_lexer_get_token(&imp_lexer_global);
    if (t.tag == tag)
    {
        imp_lexer_eat_token(&imp_lexer_global);
        if (result) { *result = t; }
        return 1;
    }
    return 0;
}

static int imp_peek_token(int tag, imp_token *result)
{
    imp_token t = imp_lexer_get_token(&imp_lexer_global);
    if (t.tag == tag)
    {
        if (result) { *result = t; }
        return 1;
    }
    return 0;
}

int imp_eof(imp_token *t) { return imp_test_token(IMP_TOKEN_EOF, t); }
int imp_paren_open(imp_token *t) { return imp_test_token('(', t); }
int imp_paren_close(imp_token *t) { return imp_test_token(')', t); }
int imp_bracket_open(imp_token *t) { return imp_test_token('[', t); }
int imp_bracket_close(imp_token *t) { return imp_test_token(']', t); }
int imp_brace_open(imp_token *t) { return imp_test_token('{', t); }
int imp_brace_close(imp_token *t) { return imp_test_token('}', t); }
int imp_colon(imp_token *t) { return imp_test_token(':', t); }
int imp_semicolon(imp_token *t) { return imp_test_token(';', t); }
int imp_plus(imp_token *t) { return imp_test_token('+', t); }
int imp_minus(imp_token *t) { return imp_test_token('-', t); }
int imp_asterisk(imp_token *t) { return imp_test_token('*', t); }
int imp_slash(imp_token *t) { return imp_test_token('/', t); }
int imp_comma(imp_token *t) { return imp_test_token(',', t); }
int imp_period(imp_token *t) { return imp_test_token('.', t); }
int imp_equals(imp_token *t) { return imp_test_token('=', t); }
int imp_more(imp_token *t) { return imp_test_token('>', t); }
int imp_less(imp_token *t) { return imp_test_token('<', t); }
int imp_identifier(imp_token *t) { return imp_test_token(IMP_TOKEN_IDENTIFIER, t); }
int imp_keyword(imp_token *t) { return imp_test_token(IMP_TOKEN_KEYWORD, t); }
int imp_integer(imp_token *t) { return imp_test_token(IMP_TOKEN_LITERAL_INTEGER, t); }
int imp_string(imp_token *t) { return imp_test_token(IMP_TOKEN_LITERAL_STRING, t); }

int imp_eof_peek(imp_token *t) { return imp_peek_token(IMP_TOKEN_EOF, t); }
int imp_paren_open_peek(imp_token *t) { return imp_peek_token('(', t); }
int imp_paren_close_peek(imp_token *t) { return imp_peek_token(')', t); }
int imp_bracket_open_peek(imp_token *t) { return imp_peek_token('[', t); }
int imp_bracket_close_peek(imp_token *t) { return imp_peek_token(']', t); }
int imp_brace_open_peek(imp_token *t) { return imp_peek_token('{', t); }
int imp_brace_close_peek(imp_token *t) { return imp_peek_token('}', t); }
int imp_colon_peek(imp_token *t) { return imp_peek_token(':', t); }
int imp_semicolon_peek(imp_token *t) { return imp_peek_token(';', t); }
int imp_plus_peek(imp_token *t) { return imp_peek_token('+', t); }
int imp_minus_peek(imp_token *t) { return imp_peek_token('-', t); }
int imp_asterisk_peek(imp_token *t) { return imp_peek_token('*', t); }
int imp_slash_peek(imp_token *t) { return imp_peek_token('/', t); }
int imp_comma_peek(imp_token *t) { return imp_peek_token(',', t); }
int imp_period_peek(imp_token *t) { return imp_peek_token('.', t); }
int imp_equals_peek(imp_token *t) { return imp_peek_token('=', t); }
int imp_more_peek(imp_token *t) { return imp_peek_token('>', t); }
int imp_less_peek(imp_token *t) { return imp_peek_token('<', t); }
int imp_identifier_peek(imp_token *t) { return imp_peek_token(IMP_TOKEN_IDENTIFIER, t); }
int imp_keyword_peek(imp_token *t) { return imp_peek_token(IMP_TOKEN_KEYWORD, t); }
int imp_integer_peek(imp_token *t) { return imp_peek_token(IMP_TOKEN_LITERAL_INTEGER, t); }
int imp_string_peek(imp_token *t) { return imp_peek_token(IMP_TOKEN_LITERAL_STRING, t); }

int imp_peek(imp_function *f, imp_token *t)
{
    imp_checkpoint checkpoint = imp_safe();
    int ok = f(t);
    if (ok)
    {
        imp_restore(checkpoint);
    }
    return ok;
}

imp_checkpoint imp_safe(void)
{
    return imp_lexer_global;
}

void imp_restore(imp_checkpoint checkpoint)
{
    imp_lexer_global = checkpoint;
}

#endif /* IMPARSER_IMPLEMENTATION */
