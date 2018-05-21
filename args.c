#include "args.h"
#include <stdio.h>
#include <string.h>

void parse_args(int argc, char * argv[], opts_t * opts)
{
    int i = 0;
    bool read_term = false;
    while (++i < argc) {
        if (read_term) {
            int arg_len = strlen(argv[i]);
            switch (arg_len) {
                case 0: {
                    fprintf(stderr, "** Terminator value is not provided: using '%c'.\n", opts->term);
                    break;
                }
                case 1: {
                    opts->term = argv[i][0];
                    break;
                }
                default: {
                    opts->term = argv[i][0];
                    fprintf(stderr, "** Terminator value is %d characters long: using '%c'.\n", arg_len, opts->term);
                }
            }
            read_term = false;
        } else if (argv[i][0] == '-') {
            const char * arg = &argv[i][1];
            while (*arg) {
                switch (*arg) {
                    case 'i': {
                        opts->no_case = true;
                        break;
                    }
                    case 'd': {
                        read_term = true;
                        break;
                    }
                }
                ++arg;
            }
        } else {
            opts->input_filename = argv[i];
        }
    }
}
