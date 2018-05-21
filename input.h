#ifndef __INPUT_H
#define __INPUT_H

#include <stdint.h>

/**
 * Utility to read file into memory.
 * \param      file_name  File name/path to read.
 * \param[out] size       Pointer to the variable where size of the filewil be stored.
 * \return Pointer to the buffer with the file context.
 */
const char * read_file(const char * file_name, uint16_t * size);

#endif