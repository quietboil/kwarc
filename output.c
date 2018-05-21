#include "output.h"
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void make_output_names(const char * input_filename, output_t * output)
{
    // "Remove" the extension from the file name (if any)
    const char * spec_rootname_end = strrchr(input_filename, '.');
    if (!spec_rootname_end) {
        spec_rootname_end = input_filename + strlen(input_filename);
    }
    size_t spec_rootname_len = spec_rootname_end - input_filename;

    // Generate .h file name (we start with .h)
    output->output_path = malloc(spec_rootname_len + 4);
    strncpy(output->output_path, input_filename, spec_rootname_len);
    output->file_name_ext = output->output_path + spec_rootname_len;

    // Find the "name" of the specification in the file name.
    // It will be used as a "namespace" of sorts.
    const char * spec_name = strrchr(input_filename, '/');
    if (spec_name) {
        ++spec_name;
    } else if ((spec_name = strrchr(input_filename, '\\')) != NULL) {
        ++spec_name;
    } else {
        spec_name = input_filename;
    }
    size_t spec_name_len = spec_rootname_len - (spec_name - input_filename);

    output->file_name = output->output_path + (spec_name - input_filename);

    // Create lower and upper case versions of the specification name
    // to be used as "namespace" prefix and .h guard respectively.
    output->lowercase_prefix = malloc(spec_name_len + 1);
    output->uppercase_prefix = malloc(spec_name_len + 1);
    for (int i = 0; i < spec_name_len; i++) {
        // Mask characters that cannot be used in the generated code.
        char c = isalnum(spec_name[i]) ? spec_name[i] : '_';
        output->lowercase_prefix[i] = tolower(c);
        output->uppercase_prefix[i] = toupper(c);
    }
    output->lowercase_prefix[spec_name_len] = '\0';
    output->uppercase_prefix[spec_name_len] = '\0';
}

/**
 * Writes a single state case block.
 * \param  state  State of the name recognition automaton.
 */
static void write_state(state_t * state, FILE * out)
{
    if (state->num_matches > 0 && state->no < 0x8000) {
        fprintf(out, "\t\tcase %u: {\n", state->no);
        if (state->num_matches == 1) {
            const char * fmt = isprint(state->matches[0])
                             ? "\t\t\tif (next_char == '%c') return %u;\n"
                             : "\t\t\tif (next_char == %u) return %u;\n";
            fprintf(out, fmt, state->matches[0], state->goto_states[0]->no);
        } else {
            fprintf(out, "\t\t\tswitch (next_char) {\n");
            for (int i = 0; i < state->num_matches; i++) {
                const char * fmt = isprint(state->matches[i])
                                 ? "\t\t\t\tcase '%c': return %u;\n"
                                 : "\t\t\t\tcase %u: return %u;\n";
                fprintf(out, fmt, state->matches[i], state->goto_states[i]->no);
            }
            fprintf(out, "\t\t\t}\n");
        }
        fprintf(out, "\t\t\tbreak;\n");
        fprintf(out, "\t\t}\n");

        // mark state as written
        state->no |= 0x8000;

        for (int i = 0; i < state->num_matches; i++) {
            write_state(state->goto_states[i], out);
        }
    }
}

void write_automaton(token_list_t * tokens, state_t * start_state, output_t * output)
{
    // Generate sources, starting with .h
    strcpy(output->file_name_ext, ".h");
    FILE * out = fopen(output->output_path, "w");
    if (out) {
        fprintf(out, "#ifndef __%s_H\n", output->uppercase_prefix);
        fprintf(out, "#define __%s_H\n\n", output->uppercase_prefix);
        fprintf(out, "#include <stdint.h>\n\n");
        // Prototypes of the state machines' entries
        fprintf(out, "uint16_t %s_next_state(uint16_t current_state, char next_char);\n\n", output->lowercase_prefix);
        for (token_t * t = tokens->first; t != NULL; t = t->next) {
            fprintf(out, "#define %.*s %d\n", t->name_end - t->name, t->name, t->id);
        }
        fprintf(out, "\n#endif\n");
        fclose(out);
    }

    strcpy(output->file_name_ext, ".c");
    out = fopen(output->output_path, "w");
    if (out) {
        strcpy(output->file_name_ext, ".h");
        fprintf(out, "#include \"%s\"\n\n", output->file_name);
        fprintf(out, "uint16_t %s_next_state(uint16_t state, char next_char)\n"
                     "{\n"
                     "\tswitch (state) {\n", output->lowercase_prefix);
        write_state(start_state, out);
        fprintf(out, "\t}\n"
                     "\treturn 0;\n"
                     "}\n\n");
    }
}
