#include "trigger.h"
#include <stdint.h>

struct trigger_t
{
	int active;
	enum EVENT e;
	uint32_t coords;
	trigger_cb_t cb;
};

static struct trigger_t triggers[1000];
static unsigned int trigger_count;

int trigger_add(enum EVENT e, uint32_t coords, trigger_cb_t cb)
{
	triggers[trigger_count].active = 1;
	triggers[trigger_count].e = e;
	triggers[trigger_count].coords = coords;
	triggers[trigger_count].cb = cb;

	++trigger_count;

    return 0;
}

static int _get_trigger_idx(enum EVENT e, uint32_t coords)
{
    for (unsigned int i = 0; i < trigger_count; ++i) {
		if (triggers[i].e == e && triggers[i].coords == coords) {
			return i;
		}
	}

	return -1;
}

int trigger_dispatch(enum EVENT e, uint32_t coords, void *edata, int *pass, int *ret)
{
	int idx = _get_trigger_idx(e, coords);

	if (idx == -1) {
		return -1;
	}

    triggers[idx].cb(coords, edata, pass, ret);

	return 0;
}
