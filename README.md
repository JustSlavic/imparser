# Imparser

This is a library is for buliding a parser.

The goal - easy to setup and use.

### Build

This is a single-header library, just copy the file into your source tree, and include into your build process.
You have to define macro `IMPARSER_IMPLEMENTATION` to enable implementation of the library.

### Using

To start parsing, just use the function `imp_begin(char const *string, uint64_t size)`, then you can use functions, corresponding to the tokens you expect.

Functions you might call:
```c
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
int imp_integer(imp_token *t);
int imp_string(imp_token *t);
```

The `imp_token *t` parameter in those functions is optional, but you might want to use it if you need it in parser logic or for better error messages.

There are `*_peek` versions of those functions, if you need to peek one token ahead.

Some parsers might want a backtracking mechanism, for that there is a pair of functions:

```c
imp_checkpoint imp_save(void);
void imp_restore(imp_checkpoint checkpoint);
```

You can save the state of the parser at some point, and then restore it when you need to backtrack your parsing process.

### Extensions

You can extend the parsing session by using the following functions:

```c
int imp_extend(char const *s, int tag);
int imp_extension(int tag, imp_token *t);
```

You can define some user defined constants, and associate them with the c-strings, so parser will recognize those strings during the parsing with the `imp_extension` function.

### Internals

This library uses single global variable for the lexer, so this library is not safe to use in a multithreaded program.

The tokenizing happens one token at a time, without building the token stream upfront. It allows the library to be allocation-free.
