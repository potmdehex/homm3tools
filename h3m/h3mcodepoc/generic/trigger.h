#ifndef __TRIGGER_H_DEF__
#define __TRIGGER_H_DEF__

#include <stdint.h>

enum EVENT
{
	EVENT_OBJECT_VISIT,
	EVENT_OBJECT_REMOVE,
	EVENT_SEERS_HUT_VISIT,
	EVENT_QUEST_IS_MONSTER_DEFEATED
};

typedef void (*trigger_cb_t)(uint32_t coords, void *edata, int *pass, int *ret);

int trigger_add(enum EVENT e, uint32_t coords, trigger_cb_t cb);
int trigger_dispatch(enum EVENT e, uint32_t coords, void *edata, int *pass, int *ret);

#endif
