#pragma once
#ifndef _mm_utils_h
#define _mm_utils_h

#include <stddef.h>

const char *mm_get_file_extension(const char *file, size_t length = 0);
const char *mm_get_token_from_end(const char *str, char token, size_t length = 0);

#endif
