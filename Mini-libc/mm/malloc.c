// SPDX-License-Identifier: BSD-3-Clause

#include <internal/mm/mem_list.h>
#include <internal/types.h>
#include <internal/essentials.h>
#include <sys/mman.h>
#include <string.h>
#include <stdlib.h>

// Define a structure to represent memory blocks
typedef struct Block
{
	size_t size;
	struct Block *next;
} Block;

Block *freeList = NULL;
void *malloc(size_t size)
{
	if (size == 0)
	{
		return NULL;
	}

	Block *prev = NULL;
	Block *curr = freeList;
	while (curr)
	{
		if (curr->size >= size)
		{
			if (curr->size > size + sizeof(Block))
			{
				Block *newFreeBlock = (Block *)((char *)curr + size + sizeof(Block));
				newFreeBlock->size = curr->size - size - sizeof(Block);
				newFreeBlock->next = curr->next;

				curr->size = size;
				curr->next = NULL;

				if (prev)
				{
					prev->next = newFreeBlock;
				} else{
					freeList = newFreeBlock;
				}
			} else{
				if (prev)
				{
					prev->next = curr->next;
				} else{
					freeList = curr->next;
				}
			}
			return (void *)(curr + 1);
		}

		prev = curr;
		curr = curr->next;
	}

	size_t totalSize = size + sizeof(Block);
	curr = (Block *)mmap(NULL, totalSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

	if (curr == MAP_FAILED)
	{
		return NULL;
	}

	curr->size = size;
	curr->next = NULL;

	return (void *)(curr + 1);
}

void *calloc(size_t nmemb, size_t size)
{
	/* TODO: Implement calloc(). */
	size_t totalSize = nmemb * size;
	void *ptr = malloc(totalSize);

	if (ptr)
	{
		memset(ptr, 0, totalSize);
	}

	return ptr;
}

void free(void *ptr)
{
	/* TODO: Implement free(). */
	if (!ptr)
	{
		return;
	}
	Block *block = (Block *)((char *)ptr - sizeof(Block));

	int size = block->size;
	munmap((char *)ptr - sizeof(Block), size + sizeof(Block));
}

void *realloc(void *ptr, size_t size)
{
	if (ptr == NULL)
	{
		return malloc(size);
	}

	Block *block = (Block *)((char *)ptr - sizeof(Block));
	if (block->size >= size)
	{
		return ptr;
	}

	void *newPtr = malloc(size);
	if (newPtr)
	{
		memcpy(newPtr, ptr, block->size);
		free(ptr);
	}

	return newPtr;
}

void *reallocarray(void *ptr, size_t nmemb, size_t size)
{
	/* TODO: Implement reallocarray(). */
	size_t totalSize = nmemb * size;
	return realloc(ptr, totalSize);
}
