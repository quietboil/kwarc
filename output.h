#ifndef __OUTPUT_H
#define __OUTPUT_H

#include "states.h"
#include <stdio.h>

/// names, derived from the input file name, used to generate output
typedef struct _output {
    char * output_path;         ///< the output path (initially without an extension)
    char * file_name_ext;       ///< pointer to the first character - the period - of the extension
    char * file_name;           ///< pointer to name of the output file
    char * lowercase_prefix;    ///< "namespace" prefix derived from the input file name
    char * uppercase_prefix;    ///< header guard prefix
} output_t;

/**
 * Derives, from the source file name, various names that are used to generate output.
 * \param  input_filename  The spec file name (as specified on the command line)
 * \param  output_names    Pointer to the results structure
 */
void make_output_names(const char * input_filename, output_t * output);

/**
 * Outputs the body of the state machine.
 * \param  tokens        Pointer to the list of tokens recognized by the automaton.
 * \param  start_state   The starting state of the automaton.
 * \param  output_names  Pointer to the initialized output names structure.
 */
void write_automaton(token_list_t * tokens, state_t * start_state, output_t * output);

#endif