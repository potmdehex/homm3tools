// Created by John Åkerblom 2015-01-24

#include "../h3mlib.h"
#include "meta_push.h"
#include "../h3m_structures/object_details/h3m_od_body_dynamic.h"

#define RESET_PTR(a) if (NULL != a) { free(a); a = NULL; }

static int _meta_push_od_artifact(uint32_t fm,
    struct H3M_OD_BODY_DYNAMIC_ARTIFACT *body,
    struct META_OD_ENTRY *meta_od_entry)
{
    size_t n = 0;

    if (0 != body->has_guardians) {
        META_OBJECT_PUSH_PTR(meta_od_entry->dyn_pointers, body, body->guardians.mesg,
            body->guardians.mesg_size, 0)
        n = (H3M_FORMAT_ROE == fm) ? sizeof(body->guardians.creatures->roe)
            : sizeof(body->guardians.creatures->absod);
        META_OBJECT_PUSH_PTR(meta_od_entry->dyn_pointers, body,
            body->guardians.creatures, n, 0)
    } else {
        META_OBJECT_PUSH_SKIP(meta_od_entry->dyn_pointers, body, body->guardians,
            sizeof(body->guardians))
    }

    return 0;
}

static int _meta_push_od_event(uint32_t fm,
    struct H3M_OD_BODY_DYNAMIC_EVENT *body, struct META_OD_ENTRY *meta_od_entry)
{
    size_t n = 0;

    _meta_push_od_artifact(fm, (struct H3M_OD_BODY_DYNAMIC_ARTIFACT *)body,
        meta_od_entry);

    n = body->contents.secondary_skills_count *
        sizeof(*(body->contents.secondary_skills));
    META_OBJECT_PUSH_PTR(meta_od_entry->dyn_pointers, body,
        body->contents.secondary_skills, n, 0)

    n = body->contents.artifacts_count
        * ((H3M_FORMAT_ROE == fm) ? sizeof(body->contents.artifacts->roe)
        : sizeof(body->contents.artifacts->absod));
    META_OBJECT_PUSH_PTR(meta_od_entry->dyn_pointers, body, body->contents.artifacts,
        n, 0)

    n = body->contents.spells_count * sizeof(*(body->contents.spells));
    META_OBJECT_PUSH_PTR(meta_od_entry->dyn_pointers, body, body->contents.spells, n,
        0)

    n = body->contents.creatures_count
        * ((H3M_FORMAT_ROE ==
            fm) ? sizeof(body->contents.creatures->roe.slots[0])
        : sizeof(body->contents.creatures->absod.slots[0]));
    META_OBJECT_PUSH_PTR(meta_od_entry->dyn_pointers, body, body->contents.creatures,
        n, 0)

    return 0;
}

static int _meta_push_od_hero(uint32_t fm,
    struct H3M_OD_BODY_DYNAMIC_HERO *body, struct META_OD_ENTRY *meta_od_entry)
{
    size_t n = 0;
    size_t count = 0;

    if (H3M_FORMAT_ROE == fm) {
        RESET_PTR(body->biography)
            RESET_PTR(body->spells)
            RESET_PTR(body->primary_skills)
    }

    n = (body->name) ? sizeof(body->name) + body->name->size : 0;
    META_OBJECT_PUSH_PTR(meta_od_entry->dyn_pointers, body, body->name, n, 0)

    if (fm < H3M_FORMAT_SOD) {
        META_OBJECT_PUSH_SKIP(meta_od_entry->dyn_pointers, body, body->has_experience,
            sizeof(body->has_experience))
    } else if (fm >= H3M_FORMAT_SOD && 0 == body->has_experience) {
        META_OBJECT_PUSH_SKIP(meta_od_entry->dyn_pointers, body, body->experience,
            sizeof(body->experience))
    }

    if (0 == body->has_face) {
        META_OBJECT_PUSH_SKIP(meta_od_entry->dyn_pointers, body, body->has_face,
            sizeof(body->has_face))
    }

    n = (NULL != body->secondary_skills && 0 != body->has_secondary_skills) ?
        (sizeof(body->secondary_skills->count) +
        (body->secondary_skills->count *
            sizeof(body->secondary_skills->skills[0])))
        : 0;
    META_OBJECT_PUSH_PTR(meta_od_entry->dyn_pointers, body, body->secondary_skills, n,
        0)

    n =(H3M_FORMAT_ROE == fm) ? 
        sizeof(body->creatures->roe) : sizeof(body->creatures->absod);
    META_OBJECT_PUSH_PTR(meta_od_entry->dyn_pointers, body, body->creatures, n, 0)

    if (NULL != body->artifacts) {
        // Get artifacts size, a bit tricky
        n = (H3M_FORMAT_ROE == fm) ? sizeof(body->artifacts->roe)
            : ((H3M_FORMAT_AB == fm) ? sizeof(body->artifacts->ab)
            : sizeof(body->artifacts->sod));
        count =
            *(uint16_t *)((uint8_t *)body->artifacts + n - sizeof(uint16_t));
        if (0 != count) {
            n += count * ((H3M_FORMAT_ROE == fm) ?
                sizeof(body->artifacts->roe.backpack.artifacts[0])
                : sizeof(body->artifacts->absod.backpack.artifacts[0]));
        }
        META_OBJECT_PUSH_PTR(meta_od_entry->dyn_pointers, body, body->artifacts, n, 0)
    } else {
        META_OBJECT_PUSH_PTR(meta_od_entry->dyn_pointers, body, body->artifacts, 0, 0)
    }

    if (fm >= H3M_FORMAT_AB) {
        n = (body->biography) ? sizeof(body->biography) +
            body->biography->size : 0;
        META_OBJECT_PUSH_PTR(meta_od_entry->dyn_pointers, body, body->biography, n, 0)

        if (H3M_FORMAT_AB == fm) {
            META_OBJECT_PUSH_SKIP(meta_od_entry->dyn_pointers, body, body->has_spells,
                sizeof(body->has_spells))
        }
    } else {
        n = sizeof(body->has_biography) + sizeof(body->biography)
            + sizeof(body->gender) + sizeof(body->has_spells) +
            sizeof(body->spells)
            + sizeof(body->has_primary_skills) + sizeof(body->primary_skills);
        META_OBJECT_PUSH_SKIP(meta_od_entry->dyn_pointers, body, body->has_biography,
            n)
    }

    if (fm >= H3M_FORMAT_AB) {
        n = (H3M_FORMAT_AB ==
            fm) ? sizeof(body->spells->ab) : sizeof(body->spells->sod);
        META_OBJECT_PUSH_PTR(meta_od_entry->dyn_pointers, body, body->spells, n, 0)
    }

    if (H3M_FORMAT_AB == fm) {
        n = sizeof(body->has_primary_skills) + sizeof(body->primary_skills);
        META_OBJECT_PUSH_SKIP(meta_od_entry->dyn_pointers, body,
            body->has_primary_skills, n)
    } else if (fm >= H3M_FORMAT_SOD) {
        META_OBJECT_PUSH_PTR(meta_od_entry->dyn_pointers, body, body->primary_skills,
            sizeof(*(body->primary_skills)), 0)
    }

    return 0;
}

static int _meta_push_od_message_bearer(uint32_t fm,
    struct H3M_OD_BODY_DYNAMIC_MESSAGE_BEARER *body,
    struct META_OD_ENTRY *meta_od_entry)
{
    META_OBJECT_PUSH_PTR(meta_od_entry->dyn_pointers, body, body->mesg,
        body->mesg_size, 0)

    return 0;
}

static int _meta_push_od_monster(uint32_t fm,
    struct H3M_OD_BODY_DYNAMIC_MONSTER *body,
    struct META_OD_ENTRY *meta_od_entry)
{
    size_t n = 0;

    if (NULL == body) {
        return 1;
    }

    if (0 != body->has_mesg_and_treasure) {
        META_OBJECT_PUSH_PTR(meta_od_entry->dyn_pointers, body,
            body->mesg_and_treasure.mesg, body->mesg_and_treasure.mesg_size, 0)

        n = ((H3M_FORMAT_ROE == fm) ? 
            sizeof(body->mesg_and_treasure.treasure->roe)
            : sizeof(body->mesg_and_treasure.treasure->absod));
        META_OBJECT_PUSH_PTR(meta_od_entry->dyn_pointers, body,
            body->mesg_and_treasure.treasure, n, 0)
    } else {
        META_OBJECT_PUSH_SKIP(meta_od_entry->dyn_pointers, body,
            body->mesg_and_treasure, sizeof(body->mesg_and_treasure))
    }

    return 0;
}

static int _meta_push_od_random_dwelling(uint32_t fm,
    struct H3M_OD_BODY_DYNAMIC_RANDOM_DWELLING *body,
    struct META_OD_ENTRY *meta_od_entry)
{
    if (META_OBJECT_RANDOM_DWELLING_PRESET_ALIGNMENT_ABSOD ==
        meta_od_entry->oa_type) {
        META_OBJECT_PUSH_SKIP(meta_od_entry->dyn_pointers, body, body->castle_absod_id,
            sizeof(body->castle_absod_id))
    }

    if (META_OBJECT_RANDOM_DWELLING_PRESET_ALIGNMENT_ABSOD ==
        meta_od_entry->oa_type || 0 != body->castle_absod_id) {
        META_OBJECT_PUSH_SKIP(meta_od_entry->dyn_pointers, body, body->alignment,
            sizeof(body->alignment))
    }

    if (META_OBJECT_RANDOM_DWELLING_PRESET_LEVEL_ABSOD == meta_od_entry->oa_type) {
        META_OBJECT_PUSH_SKIP(meta_od_entry->dyn_pointers, body, body->min_level,
            sizeof(body->min_level) + sizeof(body->max_level))
    }

    return 0;
}

static int _meta_push_od_pandoras_box(uint32_t fm,
    struct H3M_OD_BODY_DYNAMIC_PANDORAS_BOX *body,
    struct META_OD_ENTRY *meta_od_entry)
{
    // Hack for now, these two are structured the same for guardians and contents
    _meta_push_od_event(fm, (struct H3M_OD_BODY_DYNAMIC_EVENT *)body,
        meta_od_entry);

    return 0;
}

static int _meta_push_od_quest_guard(uint32_t fm,
    struct H3M_OD_BODY_DYNAMIC_QUEST_GUARD *body,
    struct META_OD_ENTRY *meta_od_entry)
{
    size_t n = 0;

    if (H3M_FORMAT_ROE == fm) {
        RESET_PTR(body->quest.objective);
    }
    // TODO remove this extern hack once this func has a proper place
    extern size_t sizeof_quest(uint8_t *quest, uint8_t quest_type);

    if (fm >= H3M_FORMAT_AB && 0 != body->quest_type) {
        n = sizeof_quest((uint8_t *)body->quest.objective, body->quest_type);
        META_OBJECT_PUSH_PTR(meta_od_entry->dyn_pointers, body, body->quest.objective,
            n, 0)
        META_OBJECT_PUSH_PTR(meta_od_entry->dyn_pointers, body,
            body->quest.deadline_and_mesg.proposal_mesg,
            body->quest.deadline_and_mesg.proposal_mesg_size, 0)
        META_OBJECT_PUSH_PTR(meta_od_entry->dyn_pointers, body,
            body->quest.deadline_and_mesg.progress_mesg,
            body->quest.deadline_and_mesg.progress_mesg_size, 0)
        META_OBJECT_PUSH_PTR(meta_od_entry->dyn_pointers, body,
            body->quest.deadline_and_mesg.completion_mesg,
            body->quest.deadline_and_mesg.completion_mesg_size, 0)
    } else {
        META_OBJECT_PUSH_SKIP(meta_od_entry->dyn_pointers, body, body->quest,
            sizeof(body->quest))
    }

    return 0;
}

static int _meta_push_od_seers_hut(uint32_t fm,
    struct H3M_OD_BODY_DYNAMIC_SEERS_HUT *body,
    struct META_OD_ENTRY *meta_od_entry)
{
    size_t n = 0;

    // TODO remove this extern hack once this func has a proper place
    extern size_t sizeof_reward(uint32_t fm, uint8_t reward_type);

    // Hack for now, these two are structured the same for their quest
    _meta_push_od_quest_guard(fm,
        (struct H3M_OD_BODY_DYNAMIC_QUEST_GUARD *)body, meta_od_entry);

    n = sizeof_reward(fm, body->reward_type);
	// Convert AB/SoD 2-byte amounts to RoE 1-byte 
	if (body->reward_type == R_CREATURES && fm == H3M_FORMAT_ROE)
	{
		body->reward->r_creatures.roe.quantity = (H3M_COMMON_CREATURE_TYPE_ROE)body->reward->r_creatures.absod.quantity;
	}
	if (body->reward_type == R_ARTIFACT && fm == H3M_FORMAT_ROE)
	{
		body->reward->r_artifact.roe = (H3M_COMMON_ARTIFACT_TYPE_ROE)body->reward->r_artifact.absod;
	}
	META_OBJECT_PUSH_PTR(meta_od_entry->dyn_pointers, body, body->reward, n, 0)
    return 0;
}

static int _meta_push_od_town(uint32_t fm,
    struct H3M_OD_BODY_DYNAMIC_TOWN *body, struct META_OD_ENTRY *meta_od_entry)
{
    struct H3M_OD_BODY_EXT_TOWN_EVENT *event = NULL;
    struct META_DYN_ARRAY_ENTRY *entry = NULL;
    UT_array *p_array = NULL;
    size_t n = 0;

    meta_od_entry->has_absod_id = (fm >= H3M_FORMAT_AB);

    n = (body->name) ? sizeof(body->name) + body->name->size : 0;
    META_OBJECT_PUSH_PTR(meta_od_entry->dyn_pointers, body, body->name, n, 0)
    n = (H3M_FORMAT_ROE == fm) ? 
        sizeof(body->creatures->roe) : sizeof(body->creatures->absod);
    META_OBJECT_PUSH_PTR(meta_od_entry->dyn_pointers, body, body->creatures, n, 0)
    META_OBJECT_PUSH_PTR(meta_od_entry->dyn_pointers, body, body->buildings,
        sizeof(*body->buildings), sizeof(body->has_fort))

    if (H3M_FORMAT_ROE == fm) {
        META_OBJECT_PUSH_SKIP(meta_od_entry->dyn_pointers, body,
            body->must_have_spells, sizeof(body->must_have_spells))
    }

    if (NULL != body->events) {
        utarray_new(p_array, &ut_ptr_icd);
        utarray_reserve(p_array, body->event_count);

        for (unsigned int i = 0; i < body->event_count; ++i) {
            struct META_DYN_ARRAY_ENTRY *entry = calloc(1, sizeof(*entry));
            event = &body->events[i];
            entry->body = (uint8_t *)event;
            entry->body_size = sizeof(*(body->events));
            utarray_new(entry->dyn_pointers, &ut_ptr_icd);

            META_OBJECT_PUSH_PTR(entry->dyn_pointers, event, event->name,
                event->name_size, 0)
            META_OBJECT_PUSH_PTR(entry->dyn_pointers, event, event->mesg,
                event->mesg_size, 0)
            if (fm < H3M_FORMAT_SOD) {
            META_OBJECT_PUSH_SKIP(entry->dyn_pointers, event,
                    event->applies_to_human, sizeof(event->applies_to_human))
            }
            utarray_push_back(p_array, &entry);
        }

        META_OBJECT_PUSH_ARRAY(meta_od_entry->dyn_pointers, body, body->events,
            p_array)
    } else {
        META_OBJECT_PUSH_PTR(meta_od_entry->dyn_pointers, body, body->events, 0, 0)
    }

    if (fm < H3M_FORMAT_SOD) {
        META_OBJECT_PUSH_SKIP(meta_od_entry->dyn_pointers, body, body->alignment,
            sizeof(body->alignment))
    }

    return 0;
}

int meta_push_od(uint32_t fm, uint8_t *body,
    struct META_OD_ENTRY *meta_od_entry)
{
    switch (meta_od_entry->oa_type) {
    case META_OBJECT_ARTIFACT:
    case META_OBJECT_ARTIFACT_AB:
    case META_OBJECT_ARTIFACT_SOD:
    case META_OBJECT_RESOURCE:
    case META_OBJECT_SPELL_SCROLL:
        return _meta_push_od_artifact(fm,
            (struct H3M_OD_BODY_DYNAMIC_ARTIFACT *)body, meta_od_entry);
    case META_OBJECT_EVENT:
        return _meta_push_od_event(fm, (struct H3M_OD_BODY_DYNAMIC_EVENT *)body,
            meta_od_entry);
    case META_OBJECT_HERO:
    case META_OBJECT_PRISON:
    case META_OBJECT_RANDOM_HERO:
        return _meta_push_od_hero(fm, (struct H3M_OD_BODY_DYNAMIC_HERO *)body,
            meta_od_entry);
    case META_OBJECT_MONSTER:
    case META_OBJECT_MONSTER_ABSOD:
        return _meta_push_od_monster(fm,
            (struct H3M_OD_BODY_DYNAMIC_MONSTER *)body, meta_od_entry);
    case META_OBJECT_OCEAN_BOTTLE:
    case META_OBJECT_SIGN:
        return _meta_push_od_message_bearer(fm,
            (struct H3M_OD_BODY_DYNAMIC_MESSAGE_BEARER *)body, meta_od_entry);
    case META_OBJECT_PANDORAS_BOX:
        return _meta_push_od_pandoras_box(fm,
            (struct H3M_OD_BODY_DYNAMIC_PANDORAS_BOX *)body, meta_od_entry);
    case META_OBJECT_RANDOM_DWELLING_ABSOD:
    case META_OBJECT_RANDOM_DWELLING_PRESET_ALIGNMENT_ABSOD:
    case META_OBJECT_RANDOM_DWELLING_PRESET_LEVEL_ABSOD:
        return _meta_push_od_random_dwelling(fm,
            (struct H3M_OD_BODY_DYNAMIC_RANDOM_DWELLING *)body, meta_od_entry);
    case META_OBJECT_QUEST_GUARD:
        return _meta_push_od_quest_guard(fm,
            (struct H3M_OD_BODY_DYNAMIC_QUEST_GUARD *)body, meta_od_entry);
    case META_OBJECT_SEERS_HUT:
        return _meta_push_od_seers_hut(fm,
            (struct H3M_OD_BODY_DYNAMIC_SEERS_HUT *)body, meta_od_entry);
    case META_OBJECT_TOWN:
    case META_OBJECT_TOWN_ABSOD:
        return _meta_push_od_town(fm, (struct H3M_OD_BODY_DYNAMIC_TOWN *)body,
            meta_od_entry);
    default:
        *(int *)0xD47B17C5 = 0xDEADDEAD;
        break;
    }

    return 1;
}
