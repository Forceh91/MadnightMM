#include "mm_utils.h"
#include <string.h>

const char *mm_get_file_extension(const char *file, size_t length)
{
	return mm_get_token_from_end(file, '.', length);
}

const char *mm_get_token_from_end(const char *str, char token, size_t length)
{
	if (length == 0)
		length = strlen(str);

	const char *substr = str;

	if (length > 0)
	{
		for (size_t i = length - 1; i >= 0; --i)
		{
			if (str[i] == token)
			{
				substr = &str[i + 1];
				break;
			}
		}
	}

	return substr;
}
