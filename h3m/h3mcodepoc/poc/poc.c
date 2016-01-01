#include "../generic/h3api.h"
#include "../generic/h3struct.h"
#include "../generic/trigger.h"

#define COORDS_QUESTGIVER H3API_XYZ(5, 5, 0)

#define COORDS_QUESTTARGET1 H3API_XYZ(6, 20, 0)
#define COORDS_QUESTTARGET2 H3API_XYZ(10, 20, 0)
#define COORDS_QUESTTARGET3 H3API_XYZ(14, 20, 0)

#define COORDS_STABLE H3API_XYZ(3, 4, 0)

static void _visit_questgiver(uint32_t coords, void *edata, int *pass, int *ret)
{
	if (coords == COORDS_QUESTGIVER) {
		h3api_fog_reveal(COORDS_QUESTTARGET1);
		h3api_fog_reveal(COORDS_QUESTTARGET2);
		h3api_fog_reveal(COORDS_QUESTTARGET3);
		*pass = 1;
	}
}

static void _visit_stable(uint32_t coords, void *edata, int *pass, int *ret)
{
	if (coords == COORDS_STABLE) {
		struct H3STRUCT_HERO *hero = (struct H3STRUCT_HERO *)edata;
		hero->movement_pts = 0x13333337;
		*pass = 0;

        h3api_fog_reveal(H3API_XYZ(36, 36, 0));
	}
}

void poc_init(void)
{
	trigger_add(EVENT_SEERS_HUT_VISIT, COORDS_QUESTGIVER, _visit_questgiver);
	trigger_add(EVENT_OBJECT_VISIT, COORDS_STABLE, _visit_stable);
}
