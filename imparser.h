#ifndef IMPARSER_H_
#define IMPARSER_H_

#include <stdint.h>

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
int imp_asterics(imp_token *t);
int imp_slash(imp_token *t);
int imp_comma(imp_token *t);
int imp_period(imp_token *t);
int imp_equals(imp_token *t);
int imp_more(imp_token *t);
int imp_less(imp_token *t);

int imp_identifier(imp_token *t);
int imp_keyword(imp_token *t);
int imp_integer(imp_token *t);

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
                                      (('a' <= c) && (c <= 'f')) ? (c - 'a') :
                                      (('A' <= c) && (c <= 'F')) ? (c - 'A') :
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
        return lexer->current_token;

    imp_lexer_consume_while(lexer, imp_ascii_is_whitespace);

    imp_token t = {};
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
        imp_lexer_eat_char(lexer); /* Consume identifier head now, becuse consume_while
                              might have predicate that does not include head character. */
        t.span.size = imp_lexer_consume_while(lexer, imp_ascii_is_valid_identifier_body) + 1;

        int32_t keyword_tag = imp_lexer_find_keyword(lexer, t.span);
        if (keyword_tag > IMP_TOKEN_INVALID)
            t.tag = keyword_tag;
    }
    else if (imp_ascii_is_digit(c))
    {
        t.tag = IMP_TOKEN_LITERAL_INTEGER;
        char const *s = (char const *) (lexer->data + lexer->cursor);
        uint32_t parsed_characters = imp_parse_integer(s, lexer->size - lexer->cursor, &t.integer_value);
        t.span.data = s;
        t.span.size = parsed_characters;
        lexer->cursor += parsed_characters;
    }
    else
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
    imp_lexer_global.data = data;
    imp_lexer_global.size = size;
}

static int imp_test_token(int tag, imp_token *result)
{
    imp_token t = imp_lexer_get_token(&imp_lexer_global);
    if (t.tag != tag) return 0;
    lexer_eat_token(imp_lexer_global);
    if (result) *result = t;
    return 1;
}

int imp_eof(imp_token *t) { return imparser_token(IMP_TOKEN_EOF, t); }
int imp_paren_open(imp_token *t) { return imparser_token('(', t); }
int imp_paren_close(imp_token *t) { return imparser_token(')', t); }
int imp_bracket_open(imp_token *t) { return imparser_token('[', t); }
int imp_bracket_close(imp_token *t) { return imparser_token(']', t); }
int imp_brace_open(imp_token *t) { return imparser_token('{', t); }
int imp_brace_close(imp_token *t) { return imparser_token('}', t); }
int imp_colon(imp_token *t) { return imparser_token(':', t); }
int imp_semicolon(imp_token *t) { return imparser_token(';', t); }
int imp_plus(imp_token *t) { return imparser_token('+', t); }
int imp_minus(imp_token *t) { return imparser_token('-', t); }
int imp_asterics(imp_token *t) { return imparser_token('*', t); }
int imp_slash(imp_token *t) { return imparser_token('/', t); }
int imp_comma(imp_token *t) { return imparser_token(',', t); }
int imp_period(imp_token *t) { return imparser_token('.', t); }
int imp_equals(imp_token *t) { return imparser_token('=', t); }
int imp_more(imp_token *t) { return imparser_token('>', t); }
int imp_less(imp_token *t) { return imparser_token('<', t); }

int imp_identifier(imp_token *t);
int imp_keyword(imp_token *t);
int imp_integer(imp_token *t);

#endif /* IMPARSER_IMPLEMENTATION */
