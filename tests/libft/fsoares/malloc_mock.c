#define _GNU_SOURCE
#include <dlfcn.h>
#include <execinfo.h>

#include "utils.h"
#include "color.h"

void *results[100];
int res_pos = 0;
int cur_res_pos = 0;

int bt_fd = -1;


#define MALLOC_LIMIT 1000000
t_node allocations[MALLOC_LIMIT];
int alloc_pos = 0;

static void _add_malloc(void *ptr, size_t size, void *to_return)
{
	void	*buffer[1000];
	int		nptrs;
	char	**strings;

	t_node new_node = allocations[alloc_pos];
	new_node.freed = false;
	new_node.ptr = ptr;
	new_node.returned = to_return;
	new_node.size = size;

	nptrs = backtrace(buffer, 1000);
	strings = backtrace_symbols(buffer, nptrs);

	new_node.strings = strings;
	new_node.nptrs = nptrs;
	allocations[alloc_pos] = new_node;
	alloc_pos++;
}

static void _mark_as_free(void *ptr)
{
	for (int pos = 0; pos < alloc_pos && alloc_pos < MALLOC_LIMIT; pos++)
	{
		t_node temp = allocations[pos];
		if (temp.ptr == ptr && !temp.freed) {
			allocations[pos].freed = true;
			return;
		}
	}
}

void *malloc(size_t size)
{
	void *(*libc_malloc)(size_t) = (void *(*)(size_t))dlsym(RTLD_NEXT, "malloc");
	void *p = libc_malloc(size);
	void *to_return = p;
	if (res_pos > cur_res_pos && (long)(results[cur_res_pos]) != 1)
		to_return = results[cur_res_pos];
	else if (size < MALLOC_LIMIT)
	{
		char *s = (char *)p;
		size_t i = 0;
		while (i < size) {
			s[i] = (char)(i + 1);
			i++;
		}
		if (i > 2)
			s[1] = 0;
		if (size > 3)
			strncpy(s + 2, "calloc should set the memory to zeros!", size - 2);
	}
	cur_res_pos++;
	_add_malloc(p, size, to_return);
	return (to_return);
}

void free(void *p)
{
	void (*libc_free)(void *) = (void (*)(void *))dlsym(RTLD_NEXT, "free");
	_mark_as_free(p);
	libc_free(p);
}

int reset_malloc_mock()
{
	int temp = cur_res_pos;
	for (int i = 0; i < cur_res_pos; i++)
	{
		free(allocations[i].strings);
	}
	res_pos = 0;
	cur_res_pos = 0;
	alloc_pos = 0;
	return temp;
}

void malloc_set_result(void *res)
{
	results[res_pos++] = res;
}

void malloc_set_null(int nth)
{
	reset_malloc_mock();
	for (int i = 0; i < nth; i++)
		malloc_set_result((void *)1);
	malloc_set_result(NULL);
}

size_t get_malloc_size(void *ptr)
{
	size_t size = 0;
	for (int pos = 0; pos < alloc_pos; pos++)
	{
		t_node temp = allocations[pos];
		if (temp.ptr == ptr)
		{
			if (temp.freed)
				size = temp.size;
			else
				return temp.size;
		}
	}
	return size;
}

void print_mallocs()
{
	int temp = alloc_pos;
	for (int pos = 0; pos < temp; pos++)
	{
		t_node tmp = allocations[pos];
		fprintf(errors_file, "%i: %p - %zu - freed: %i - %p\n", pos, tmp.ptr, tmp.size, tmp.freed, tmp.returned);
	}
}

void save_traces(char **strings, int nptrs)
{
	for (int i = 0; i < nptrs; i++)
	{
		fprintf(errors_file, "%s\n", strings[i]);
	}
}

int check_leaks(void *result)
{
	if (result)
		free(result);
	int temp = alloc_pos;
	int res = 1;
	for (int pos = 0; pos < temp; pos++)
	{
		t_node tmp = allocations[pos];
		if (!tmp.freed && tmp.returned)
		{
			if (res)
				error("\n");
			fprintf(errors_file, "Memory leak: %p - %zu bytes\n", tmp.returned, tmp.size);
			fprintf(errors_file, "You failed to free the memory allocated at:\n");
			save_traces(tmp.strings, tmp.nptrs);
			fprintf(errors_file, "\n");
			res = 0;
		}
	}
	if (!res)
		fprintf(errors_file, "\n");
	return res;
}

t_node *get_all_allocs()
{
	t_node *result = calloc(alloc_pos, sizeof(t_node));
	for (int i = 0; i < alloc_pos; i++)
	{
		t_node temp;
		temp.ptr = allocations[i].ptr;
		temp.returned = allocations[i].returned;
		temp.size = allocations[i].size;
		temp.freed = allocations[i].freed;
		temp.nptrs = allocations[i].nptrs;
		temp.strings = calloc(temp.nptrs, sizeof(char *));
		for (int j = 0; j < temp.nptrs; j++)
			temp.strings[j] = strdup(allocations[i].strings[j]);
		result[i] = temp;
	}
	return result;
}

void add_trace_to_signature(int offset, t_node *allocs, int n)
{
	for (int i = 0; i < allocs[n].nptrs; i++)
	{
		offset += sprintf(signature + offset, "%s\n", allocs[n].strings[i]);
	}
}