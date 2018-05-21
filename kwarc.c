#include "args.h"
#include "input.h"
#include "states.h"
#include "output.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE_LEN 127

/// Allocates and initializes a new token.
static token_t * token_create(const char * name, const char * name_end, uint8_t id)
{
    token_t * token = malloc(sizeof(token_t));
    token->name = name;
    token->name_end = name_end;
    token->id = id;
    token->next = NULL;
    return token;
}

/// Appends a token to the list
static void token_list_append(token_list_t * tokens, token_t * token)
{
    if (tokens->last) {
        tokens->last = tokens->last->next = token;
    } else {
        tokens->last = tokens->first = token;
    }
}

/// Representation of the compiled state machine - tokens and states
typedef struct _sm {
    state_t *     start_state;
    token_list_t  tokens;
} sm_t;

/**
 * Compiles the recognition automaton specification.
 * \param  text      Pointer to the text of the spec.
 * \param  text_end  Pointer to the character jsut after the last character of the spec
 * \param  kw_term   Character that is used to terminate keywords.
 * \param  no_case   Option to merge keywords that differ only in their caseness.
 * \return state machine states and the list of tokens
 */
static sm_t spec_compile(const char * text, uint16_t text_len, char kw_term, bool no_case)
{
    sm_t sm;
    sm.start_state = state_create(0);
    sm.tokens.first = NULL;
    sm.tokens.last = NULL;

    uint16_t last_state_no = MAX_TOKEN_ID;
    uint16_t last_token_id = 0;

    const char * text_end = text + text_len;

    while (text < text_end) {
        // line format = keyword: TOKEN
        const char * keyword = text;
        while (text < text_end && *text != '\n' && *text != kw_term) {
            ++text;
        }
        const char * keyword_end = text;

        if (text < text_end && *text == kw_term) {
            // skip whitespace
            while (++text < text_end && *text != '\n' && !isgraph(*text)) {}
            if (text < text_end && isgraph(*text)) {
                const char * token = text;
                // find the end of the token
                while (++text < text_end && isgraph(*text)) {}
                const char * token_end = text;

                uint16_t last_listed_token_id = last_token_id;
                const state_t * final_state = 
                    build_string_matcher( sm.start_state, &last_state_no, &last_token_id
                                        , keyword, keyword_end - keyword, no_case );
                if (final_state->no > last_listed_token_id) {
                    token_list_append(&sm.tokens, token_create(token, token_end, final_state->no));
                }
            }
        }
        // skip to the next line
        while (text < text_end && *text++ != '\n') {}
    }
    return sm;
}

int main(int argc, char * argv[])
{
    opts_t opts;

    // Set up the default values
    opts.input_filename = NULL;
    opts.no_case = false;   // keywords are case sensitive
    opts.term = ':';        // default keyword-value separator

    parse_args(argc, argv, &opts);

    if (!opts.input_filename) {
        fprintf(stderr, "Usage: %s [-i] [-d 'term'] <spec_file_name>\n", argv[0]);
        return 1;
    }

    output_t output;
    make_output_names(opts.input_filename, &output);

    uint16_t text_length;
    const char * text = read_file(opts.input_filename, &text_length);

    if (text) {
        sm_t sm = spec_compile(text, text_length, opts.term, opts.no_case);
                
        write_automaton(&sm.tokens, sm.start_state, &output);
    }

    return 0;
}