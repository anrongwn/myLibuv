#pragma once
#include <stdlib.h>

//�ڴ������
class CanAllotter {
public:
	static void * an_malloc(size_t size) {
		return malloc(size);
	}

	static void an_free(void *buf) {
		free(buf);
	}
};
