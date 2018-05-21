# Keyword Recognition Automaton Compiler

**kwarc** generates, in C, automata to recognize a set of keywords.

## Motivation

Every now and then you might face a task to process something selectively based on a keyword that identifies the information. The often used pattern to select appropriate action looks somewhat like this:
```c
if (strncmp(ptr, "keyword-A", 9) == 0 && ptr[9] == ':') {
    process_keyword_A(ptr, ...);
} else if (strncmp(ptr, "keyword-B", 9) == 0 && ptr[9] == ':') {
    process_keyword_A(ptr, ...);
} else if (...) {
    ...
}
```
While this approach is quite adequate, in some cases it reveals a number of deficiencies that makes it less than ideal:
- The keywords have common prefix. For example, when mathing `Content-Encoding` and `Content-Length` `strncmp` have to redo some of the work. This can be easily addressed, but the resulting source code most likelt will be a but less readable.
- The payload arrives in fragments and the keyword can be split in the middle at the fragment boundary.

**kwarc** addresses these shortcomings by generating an automaton to recognize a set of specified keywords.

## Features

**kwarc** generates automata that help creating the recognition machine for the specified set of keywords. **kwarc** does not actually generate the recognition machine itself - those are not dificult to implement and there are quite a few possible variants that can be used depending on the use case and the environment. **kwarc** solves the most tedious (well, from the author's point of view anyway :smiley: ) problem of creating a recognition machine - does the next character in the input stream belong to one of the keywords we recognize and, if it does, to which one?

In the end given this specification:
```yaml
Accept:             ACCEPT
Accept-Charset:     ACCEPT_CHARSET
Accept-Encoding:    ACCEPT_ENCODING
Accept-Language:    ACCEPT_LANGUAGE
Accept-Datetime:    ACCEPT_DATETIME
```
**kwarc** generates the the header file and the implementation of the following function:
```h
uint16_t next_state(uint16_t current_state, char next_char);
```

**kwarc** also supports use cases where keywords might differ in their caseness, but otheriwse should be recognized as the same keyword. For example, when the following keywords are specified:
```yaml
Accept-Charset:     ACCEPT_CHARSET
accept-charset:     ACCEPT_CHARSET
```
**kwarc** will build an automaton that will accept both keywords and return the same token ID for both of them.

## Installation

Start by cloning or downloading and extracting the ZIP of this repository. For example:
```sh
$ git clone https://github.com/quietboil/kwarc
```

Then execute `make` to build the compiler.

## Tests

To build **kwarc** unit tests `make` them in the `tests` directory:
```sh
$ cd tests
$ make
```
Tests are built as a single executable `tests`. That's what you run to execute unit tests.

> :pushpin: **Note** that unit tests can be referenced as examples of how **kwarc** generated automata can be used.

## Specification

**kwarc** specification is a text file where each non-empty line represents a keyword terminated bydefault by `:` and a symbol returned by the automaton when the keyword is recognized.

> :pushpin: **Note** that `:` keyword terminator can be changed by the `-d` command line option to any character in cases when, for example, `:` is a part of a keyword and thus cannot be used as a keyword terminator.

## Compilation

**kwarc** only needs the name of the specification file. However it also accept command line 2 options:
- `-d` - specifies the keyword terminating character to use by the compiler. For example, to use `!` as a terminator one would use: `kwarc -d ! kw.spec`
- `-i` - directs the compiler to merge keywords that differ only by the caseness of some of their letters into a single keyword.

> :pushpin: **Note** that `-i` option does not create a case-incensitive automaton. For example, when specification lists `Accept-Charset` and `accept-charset`, the resulting automaton will still reject `ACCEPT-CHARSET` or `AcCePt-ChArSeT` :smiley: even if compiled with `-i` option.

The name of the specification file itself is also important. Besides being used to name the generated `.h` and `.c` files it is also used as a name-space of sorts to name the automaton function inself. For example, given the specification file `http_headers.spec` **kwarc** will generate function named:
```h
uint16_t http_headers_next_state(uint16_t current_state, char next_char);
```

## Usage

Automaton requires that you provide a storage for the current automaton state and feed it to the recognition funtion together with the next character from the input stream.

Automaton will return the next state which can be:
- 0 when the next character cannot be accepted as a part of the recognizable set of keywords,
- a number >= 256 when the next character has been accepted, but the keyword has not been recognized yet (this is an internal automaton state),
- a number between 1 and 255 when the next character has been accepted and it the last character in one of the specified keywords. 

> :pushpin: **Note** that the last case only indicates that the last "next" character was accepted and together with the previously accepted characters forms one of the specified keywords. However, depending on the use case it might not mean that the accepted characters actually form the expected keyword. You might have to use a lookahead character to ensure that the recognized string is indeed a keyword. See `test_http_headers.c` for an example of the recognition machine - `scan_http_header` - that uses `:` as a lookahead to determine whether the substrig that has been matched is a keyword.
