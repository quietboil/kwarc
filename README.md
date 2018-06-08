# Keyword Recognition Automaton Compiler

**kwarc** generates automaton implemented in C to recognize a set of keywords.

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
- The keywords have common prefix. For example, when matching `Content-Encoding` and `Content-Length` `strncmp` have to redo some of the work. This can be easily addressed, but the resulting source code most likely will be a but less readable.
- The payload arrives in fragments and the keyword can be split in the middle at the fragment boundary.

**kwarc** addresses these shortcomings by generating an automaton to recognize a set of specified keywords.

## Features

**kwarc** generates automaton that help creating the recognition machine for the specified set of keywords. **kwarc** also generates the recognition machine itself. However in some cases, for example when the scanned text arrives in fragments - the full recognition will require a few additional steps that are environment and implementation dependent and need to be implemented by the user of the generated scanner.

Given this specification:
```yaml
Accept:             ACCEPT
Accept-Charset:     ACCEPT_CHARSET
Accept-Encoding:    ACCEPT_ENCODING
Accept-Language:    ACCEPT_LANGUAGE
Accept-Datetime:    ACCEPT_DATETIME
```
**kwarc** generates the the header file and the implementation of the following two functions:
```h
uint16_t spec_next_state(uint16_t current_state, char next_char);

typedef struct {
    uint16_t state;
    uint16_t length;
} spec_scan_result_t;

spec_scan_result_t spec_scan(uint16_t current_state, char next_char);
```
Where `spec` is a namespace prefix which **kwarc** derives from the keyword specification file name.

**kwarc** also supports use cases where keywords might differ in their case, but otherwise should be recognized as the same keyword. For example, when the following keywords are specified:
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

**kwarc** specification is a text file where each non-empty line represents a keyword terminated by default by `:` and a symbol returned by the automaton when the keyword is recognized.

> :pushpin: **Note** that `:` keyword terminator can be changed by the `-d` command line option to any character in cases when, for example, `:` is a part of a keyword and thus cannot be used as a keyword terminator.

## Compilation

**kwarc** only needs the name of the specification file. However it also accept command line 2 options:
- `-d` - specifies the keyword terminating character to use by the compiler. For example, to use `!` as a terminator one would use: `kwarc -d ! kw.spec`
- `-i` - directs the compiler to merge keywords that differ only by the case of some of their letters into a single keyword.

> :pushpin: **Note** that `-i` option does not create a case-insensitive automaton. For example, when specification lists `Accept-Charset` and `accept-charset`, the resulting automaton will still reject `ACCEPT-CHARSET` or `AcCePt-ChArSeT` :smiley: even if compiled with `-i` option.

The name of the specification file itself is also important. Besides being used to name the generated `.h` and `.c` files it is also used as a name-space of sorts to name the automaton function itself. For example, given the specification file `http_headers.spec` **kwarc** will generate:
```h
typedef struct _http_headers_scan_result {
    uint16_t state;
    uint16_t length;
} http_headers_scan_result_t;
uint16_t http_headers_next_state(uint16_t state, char next);
http_headers_scan_result_t http_headers_scan(uint16_t state, const char * text, const char * end);
```

## Usage

Generated scanner requires that you provide a storage for the current scanner state and feed it to the recognition funtion together with the pointers to the current position in the text and the end of the text buffer.

Scanner will return the next state and the length of the scanned part of the text. The returned state can be:
- 0 when the scanner has not matched the beginning of the buffer to any of the known keywords. The `length` in this case represents the number of characters scanned before scanner determined that it could not continue.
- A number >= 256 when when scanner reaches the end of the buffer before recognizing a keyword. Appllication should continue scanning when more data become available and provide this internal state to the scanner as the starting state.
- A number between 1 and 255 when the scanner has recognized one of the keywords. the returned state is the ID of that keyword

If the generated scanner for some reason cannot be used directly, you can build your own variant by using generated `next_state` function. It needs the current automaton state (the starting state is 0) and the next character in the stream. It'll return the next automaton state. The returned state can be:
- 0 when the next character was not expected in this state, i.e. whatever has been scanned up to this point does represent a beginning of a known keyword.
- A number >= 256 when the next character has been accepted, but the automaton is in the middle of recognizing a keyword and needs more characters to finish.
- A number between 1 and 255 when a keyword has been matched. The returned state is the ID of that keyword.

> :pushpin: **Note** that when there is a keyword that is a substring of one or more other keywords, for example `Accept` and `Accept-Charset`, care must be taken to ensure that, when the scanner returns the ID a keyword, it is indeed fully recognized. And if it is not, when for example the next character is not `:`, scanning must continue and the returned state be treated as an intermediate state.