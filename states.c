#include "states.h"
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

state_t * state_create(uint16_t state_number)
{
    state_t * state = calloc(1, sizeof(state_t));
    state->no = state_number;
    return state;
}

/**
 * Adds a new character match transition to the state where transition destination is an existing state.
 * \param  state       State to add a new match transition to.
 * \param  match       Character to match.
 * \param  next_state  State to go to when matched.
 * \return next_state
 */
static state_t * state_add_goto_on_match(state_t * state, char match, state_t * next_state)
{
    uint8_t i = state->num_matches;
    if (++state->num_matches > state->max_matches) {
        state->max_matches += 4;
        state->matches     = realloc(state->matches,     sizeof(char)     * state->max_matches);
        state->goto_states = realloc(state->goto_states, sizeof(state_t*) * state->max_matches);
    }
    state->matches[i] = match;
    return state->goto_states[i] = next_state;
}

/**
 * Adds a new character match transition to the specified state.
 * \param          state         State to add a new match transition to.
 * \param          match         Character to match.
 * \param[in,out]  state_no_gen  Pointer to the generator of state numbers.
 * \return The go-to state of the added transition.
 */
static state_t * state_add_match(state_t * state, char match, uint16_t * state_no_gen)
{
    state_t * next_state = state_create(++*state_no_gen);
    return state_add_goto_on_match(state, match, next_state);
}

/// Constant that is returned by the `state_get_transition` to indicate that the state does not match
/// the specified character.
#define NOT_FOUND -1

/**
 * Scans the state transitions for a specified match value.
 * \param  state    State to examine.
 * \param  match    Match character to locate.
 * \param  no_case  If `match` is a letter, ignore the case during search
 * \return Index of the match transition if found. NOT_FOUND otherwise.
 */
static int state_get_transition(state_t * state, char match, bool no_case)
{
    const char * mptr = state->matches;
    const char * mend = state->matches + state->num_matches;
    // look for the exact match first
    for (; mptr < mend; ++mptr) {
        if (*mptr == match) {
            return mptr - state->matches;
        }
    }
    if (no_case) {
        // look for a case-insensitive match then
        for (mptr = state->matches; mptr < mend; ++mptr) {
            if (toupper(*mptr) == toupper(match)) {
                return mptr - state->matches;
            }
        }
    }
    return NOT_FOUND;
}

state_t * build_string_matcher(state_t * state, uint16_t * state_no_gen, uint16_t * token_id_gen, const char * text, uint8_t text_len, bool no_case)
{
    const char * text_end = text + text_len;
    const char * last_chr = text_end - 1;
    while (text != text_end) {
        state_t * next_state;
        int transition_idx = state_get_transition(state, *text, no_case);
        if (transition_idx < 0) {
            next_state = state_add_match(state, *text, text == last_chr ? token_id_gen : state_no_gen);
        } else if (state->matches[transition_idx] != *text) {
            next_state = state_add_goto_on_match(state, *text, state->goto_states[transition_idx]);
        } else {
            next_state = state->goto_states[transition_idx];
        }
        state = next_state;
        ++text;
    }
    if (state->no > MAX_TOKEN_ID) {
        // this text is a substring of a longer one that is already being matched
        state->no = ++*token_id_gen;
    }
    return state;
}
