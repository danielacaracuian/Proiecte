// SPDX-License-Identifier: BSD-3-Clause

 #include "osmem.h"

 #include "helpers.h"

 #define ALIGNMENT 8
 #define ALIGN(size) (((size) + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1))
 #define MAX_THRESHOLD 131072
 #define PAGE_SIZE 4096

static struct block_meta *block_list;
static size_t threshold = MAX_THRESHOLD;

static void split(struct block_meta *block, size_t size)
{
	block->status = STATUS_ALLOC;
	long diff_size =
		(long)block->size - (long)size - sizeof(struct block_meta);

	if (diff_size < 1)
		return;

	struct block_meta *new_block =
		(struct block_meta *)((char *)block + sizeof(struct block_meta) +
							  size);
	new_block->size = diff_size;
	new_block->status = STATUS_FREE;
	new_block->next = block->next;

	block->size = size;
	block->next = new_block;
}

static void coalesce(void)
{
	struct block_meta *current = block_list;

	while (current) {
		struct block_meta *next = current->next;

		if (current->status == STATUS_FREE && next &&
			next->status == STATUS_FREE) {
			current->size += next->size + sizeof(struct block_meta);
			current->next = next->next;
		} else {
			current = current->next;
		}
	}
}

void *os_malloc(size_t size)
{
	if (size <= 0)
		return NULL;

	size = ALIGN(size);
	size_t total_size = sizeof(struct block_meta) + size;
	struct block_meta *block;

	if (total_size > threshold) {
		block = mmap(NULL, total_size, PROT_READ | PROT_WRITE,
					 MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
		if (block == MAP_FAILED)
			return NULL;

		block->size = size;
		block->status = STATUS_MAPPED;
		block->next = NULL;

		return block + 1;
	}

	if (block_list == NULL) {
		block = sbrk(MAX_THRESHOLD);
		if (block == (void *)-1)
			return NULL;

		block->size = MAX_THRESHOLD - sizeof(struct block_meta);
		block->status = STATUS_FREE;
		block->next = NULL;

		block_list = block;

		split(block, size);
	} else {
		struct block_meta *current = block_list, *best_fit = NULL,
						  *prev = NULL;

		coalesce();

		while (current) {
			if (current->status == STATUS_FREE && current->size >= size &&
				(best_fit == NULL || current->size < best_fit->size)) {
				best_fit = current;
			}
			prev = current;
			current = current->next;
		}

		if (best_fit) {
			block = best_fit;
			block->status = STATUS_ALLOC;

			split(block, size);
		} else {
			if (prev->status == STATUS_FREE) {
				block = sbrk(size - prev->size);
				if (block == (void *)-1)
					return NULL;

				prev->status = STATUS_ALLOC;
				prev->size = size;

				block = prev;
			} else {
				block = sbrk(total_size);
				if (block == (void *)-1)
					return NULL;

				prev->next = block;

				block->next = NULL;
				block->status = STATUS_ALLOC;
				block->size = size;
			}
		}
	}

	return block + 1;
}

void os_free(void *ptr)
{
	if (ptr == NULL)
		return;

	struct block_meta *block = (struct block_meta *)ptr - 1;

	if (block->status == STATUS_MAPPED)
		munmap(block, block->size + sizeof(struct block_meta));
	else
		block->status = STATUS_FREE;
}

void *os_calloc(size_t nmemb, size_t size)
{
	size_t total_size = nmemb * size;

	threshold = PAGE_SIZE;

	void *ptr = os_malloc(total_size);

	threshold = MAX_THRESHOLD;

	if (ptr != NULL && total_size < PAGE_SIZE)
		memset(ptr, 0, total_size);

	return ptr;
}

void *os_realloc(void *ptr, size_t size)
{
	if (ptr == NULL)
		return os_malloc(size);

	if (size == 0) {
		os_free(ptr);
		return NULL;
	}

	struct block_meta *block = (struct block_meta *)ptr - 1;

	size = ALIGN(size);

	if (block->status == STATUS_FREE)
		return NULL;

	if (block->status == STATUS_ALLOC) {
		if (size <= block->size) {
			size_t diff_size = block->size - size;

			if (diff_size > sizeof(struct block_meta))
				split(block, size);

			return ptr;
		}

		struct block_meta *next_block = block->next;

		if (next_block == NULL) {
			struct block_meta *new_block = sbrk(size - block->size);

			if (new_block == (void *)-1)
				return NULL;

			block->size = size;
			return ptr;
		}

		int old_size = block->size;

		while (next_block != NULL && next_block->status == STATUS_FREE) {
			size_t total_size =
				old_size + next_block->size + sizeof(struct block_meta);
			if (size <= total_size) {
				block->size = total_size;
				block->next = next_block->next;

				if (total_size - size > sizeof(struct block_meta))
					split(block, size);
				else
					block->size = total_size;

				return ptr;
			}

			block->next = next_block->next;
			block->size = total_size;
			next_block = next_block->next;
		}
	}

	void *new_ptr = os_malloc(size);

	if (new_ptr == NULL)
		return NULL;

	if (block->size < size)
		size = block->size;

	memcpy(new_ptr, ptr, size);
	os_free(ptr);

	return new_ptr;
}
