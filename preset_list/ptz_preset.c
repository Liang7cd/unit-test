#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "ptz_preset.h"
#include "list.h"

#define PRESET_FILE "./preset.data"

typedef struct Ptz_preset_T *Ptz_preset_T;

struct Ptz_preset_T {
    int index;
    int x;
    int y;
};

static ListD_T list = NULL;

ListD_T ptz_preset_get_list(void)
{
	return list;
}


int ptz_preset_parse_file(char *pathname, ListD_T list)
{
	FILE *fp = NULL;
	int count = 0;
	Ptz_preset_T preset = NULL;
	ListDNode_T node = NULL;

	assert(pathname);
	assert(list);

	fp = fopen(pathname, "r");
	if (fp == NULL)
	{
		return -1;
	}

	fscanf(fp, "%d", &count);

	for (int i = 0; i < count; i++)
	{
		preset = (Ptz_preset_T)calloc(1, sizeof(*preset));
		if (preset == NULL)
		{
		}
		fscanf(fp, "%d,%d,%d", &(preset->index), &(preset->x), &(preset->y));
		node = ListDNode_new((void *)preset);
		ListD_insert(list, node);
	}

	fclose(fp);

	return 0;
}

int ptz_preset_save_file(char *pathname, ListD_T list)
{
	int count = 0;
	FILE *fp = NULL;
	Ptz_preset_T preset = NULL;
	ListDNode_T node = NULL;
	char tmp[64] = { 0 };

	assert(pathname);
	assert(list);

	fp = fopen(pathname, "w");
	if (fp == NULL)
	{
		return -1;
	}

	count = ListD_count(list);
	snprintf(tmp, 64, "%d\n", count);
	fwrite(tmp, 1, strlen(tmp), fp);

	for (int i = 0; i < count; i++)
	{
		node = ListD_get(list, i);
		preset = (Ptz_preset_T)(node->priv);
		snprintf(tmp, 64, "%d,%d,%d\n", preset->index, preset->x, preset->y);
		fwrite(tmp, 1, strlen(tmp), fp);
	}

	fflush(fp);

	fclose(fp);

	return 0;
}

static int cmp(void *priv, void *arg)
{
	Ptz_preset_T pre = (Ptz_preset_T)priv;
	int index = (int )arg;

	if (pre->index == index)
		return 0;
	else
		return 1;
}

int ptz_preset_add(int index, int x, int y)
{
	ListDNode_T node = NULL;
	Ptz_preset_T new = NULL;

	node = ListD_search(list, cmp, (void *)index);
	if (node == NULL)
	{
		new = (Ptz_preset_T)calloc(1, sizeof(*new));
		if (new != NULL)
		{
			new->index = index;
			new->x = x;
			new->y = y;

			node = ListDNode_new((void *)new);
			ListD_insert(list, node);
		}else{
			return -1;
		}
	}else{
		new = (Ptz_preset_T)(node->priv);
		printf("index:[%d], x:[%d], y:[%d]\n", new->index, new->x, new->y);
		new->index = index;
		new->x = x;
		new->y = y;
	}
	ptz_preset_save_file(PRESET_FILE, list);

	return 0;
}

int ptz_preset_del(int old)
{
	ListDNode_T node = NULL;

	node = ListD_search(list, cmp, (void *)old);
	if (node)
	{
		free(node->priv);
		ListD_remove(list, node);
		ListDNode_free(&node);

		ptz_preset_save_file(PRESET_FILE, list);

		return 0;
	}
	else
	{
		return -1;
	}
}

int ptz_preset_search(int index, int *x, int *y)
{
	assert(x && y);

	ListDNode_T node = NULL;
	Ptz_preset_T preset = NULL;

	node = ListD_search(list, cmp, (void *)index);
	if (node)
	{
		preset = (Ptz_preset_T)(node->priv);
		*x = preset->x;
		*y = preset->y;

		return 0;
	}
	else
	{
		return -1;
	}
}

int ptz_preset_init(void)
{
	list = ListD_new();

	return ptz_preset_parse_file(PRESET_FILE, list);
}

int ptz_preset_deinit(void)
{
	int ret = 0;
	int count = 0;
	ListDNode_T node = NULL;

	ret = ptz_preset_save_file(PRESET_FILE, list);
	if (ret == -1)
	{
		return -1;
	}

	for (int i = 0; i < count; i++)
	{
		node = ListD_get(list, i);
		free(node->priv);
		ListDNode_free(&node);
	}

	ListD_free(&list);

	return 0;
}


