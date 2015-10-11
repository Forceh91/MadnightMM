#pragma once
#ifndef _mm_extractor_h
#define _mm_extractor_h

#include "mm_mod_item.h"

bool mm_extractor_initialize(const char *pathToLibrary);
void mm_extractor_shutdown();

bool mm_extractor_scan(mm_mod_item *mod);

#endif
