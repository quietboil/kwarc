#ifndef __STATES_H
#define __STATES_H

#include <stdint.h>
#include <stdbool.h>

/// The maximum number that will be used to identify the tokens.
#define MAX_TOKEN_ID  UINT8_MAX

/// States of the strings recognition machine
typedef struct _state state_t;
struct _state {
    uint16_t    no;             ///< State number
    uint8_t     num_matches;    ///< Number of transitions
    uint8_t     max_matches;    ///< Capacity of the transitions storage
    char *      matches;        ///< Array of characters to match in this state
    state_t **  goto_states;    ///< Array of pointers to states to transtion to
};

/**
 * Allocates and initializes the new state.
 * \param  number  State number.
 * \return Pointer to the state struct.
 */
state_t * state_create(uint16_t state_number);

/**
 * Adds states to the string recognition automaton required to recognize the new string.
 * \param  state         Initial state of the automaton.
 * \param  state_no_gen  Pointer to the state number "generator".
 * \param  token_id_gen  Pointer to the token ID "generator".
 * \param  text          Pointer to the text of the text to recognize.
 * \param  text_len      Length of the text.
 * \param  no_case       Ignore the case when building matching states
 * \return Final state of the text recognition. This in this state the automaton returns the ID of the recognized text.
 *
 * The `no_case` argument when set to `true` forces the reuse of the existing transition when the letter case is
 * the only difference. This reduces the size of the automaton, but it has a side-effect. The following specification:
 * ```
 * Content-Length: CONTENT_LENGTH
 * content-length: CONTENT_LENGTH
 * ```
 * will create an automaton that will also accept `Content-length` and `content-Length`.
 */
state_t * build_string_matcher(state_t * state, uint16_t * state_no_gen, uint16_t * token_id_gen, const char * text, uint8_t text_len, bool no_case);

/// Token is a symbol used to represent a recognized keyword.
typedef struct _token token_t;

/// Element of the token-name -> token-ID collection
struct _token {
    const char * name;      ///< Ponter to the first character of the token name
    const char * name_end;  ///< End of token name (points to the character next to the last in the token name)
    uint8_t      id;        ///< Token ID. State No that recognizes the keyword.
    token_t    * next;      ///< Next element in a linked list
};

/// A list of tokens.
typedef struct _token_list {
    token_t * first;
    token_t * last;
} token_list_t;


#endif