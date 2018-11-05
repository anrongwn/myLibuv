#pragma once
#include <stdlib.h>

//ÄÚ´æ·ÖÅäÆ÷
class CanAllotter {
public:
	static void * an_malloc(size_t size) {
		return malloc(size);
	}

	static void an_free(void *buf) {
		free(buf);
	}
};
