#ifndef __ARGS_H
#define __ARGS_H

#include <stdbool.h>

/// Program execution options
typedef struct _opts {
    const char * input_filename;
    bool         no_case;
    char         term;
} opts_t;

/**
 * Scans program's arguments and updates the options.
 * \param  argc  `argc` from `main`
 * \param  argv  `argv` from `main`
 * \param  opts  Pointer to the program options structure.
 */
void parse_args(int argc, char * argv[], opts_t * opts);

#endif