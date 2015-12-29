// Created by John Åkerblom 2014-10-12

#ifndef __H3M_AI_HERO_SETTINGS_H_DEF__
#define __H3M_AI_HERO_SETTINGS_H_DEF__

// Included by h3m_ai.h

#include "../utils/msvc_comp_stdint.h"
#include "../common/h3m_common.h"

#pragma pack(push, 1)

// TODO: split this struct, use the one from h3m_common.h for worn artifacts
// NOT BINARY COMPATIBLE - LAST ELEMENT IS DYNAMIC
struct H3M_AI_HERO_SETTINGS_ARTIFACTS {
    // Untested, taken from Russian doc with unknown member added
    uint16_t headwear;
    uint16_t shoulders;
    uint16_t right_hand;
    uint16_t left_hand;
    uint16_t torso;
    uint16_t right_ring;
    uint16_t left_ring;
    uint16_t feet;
    uint16_t misc1;
    uint16_t misc2;
    uint16_t misc3;
    uint16_t misc4;
    uint16_t device1;
    uint16_t device2;
    uint16_t device3;
    uint16_t device4;
    uint16_t unknown;
    uint16_t spellbook;
    uint16_t misc5;
    uint16_t backpack_count;

    struct H3M_COMMON_BACKPACK_ENTRY *backpack_entries; // DYNAMIC MEMBER
};                              // Size 40 + ?, ? = <backpack_amount> * 2

// NOT BINARY COMPATIBLE - LAST ELEMENT IS DYNAMIC
struct H3M_AI_HERO_SETTINGS_BIOGRAPHY {
    uint32_t desc_size;
    uint8_t *desc;
};                              // Size 4+?, ? = <desc_size>

// TODO: split this struct, use the one from h3m_common.h
// NOT BINARY COMPATIBLE - LAST ELEMENT IS DYNAMIC
struct H3M_AI_HERO_SETTINGS_SECONDARY_SKILLS {
    uint32_t count;
    struct H3M_COMMON_SECONDARY_SKILL *entries;
};                              // Size 4+?, ? = <skill_count> * 2

// NOT BINARY COMPATIBLE - CONTAINS DYNAMIC ELEMENTS
struct H3M_AI_HERO_SETTINGS {
    uint8_t has_settings;
    // if has_settings is 0, struct ends here

    uint8_t has_experience;
    // experience here, 0/4 bytes
    uint8_t has_secondary_skills;
    // secondary_skills here, 0/(4+#*2), # = number of secondary skills
    uint8_t has_artifacts;
    // artifacts here, 0/(40+#*2), # = number of artifacts in backpack
    uint8_t has_biography;
    // biography here, 0/(4+#), # = biography size
    uint8_t gender;             // 0xFF-default, 0x00-male, 0x01-female
    uint8_t has_spells;
    // spells here, 0/9 bytes
    uint8_t has_primary_skills;
    // primary_skills here 0/4 bytes

    uint32_t experience;
    struct H3M_AI_HERO_SETTINGS_SECONDARY_SKILLS secondary_skills;
    struct H3M_AI_HERO_SETTINGS_ARTIFACTS artifacts;
    struct H3M_AI_HERO_SETTINGS_BIOGRAPHY biography;
    uint8_t spells[9];
    struct H3M_COMMON_PRIMARY_SKILLS primary_skills;
};

#pragma pack(pop)

#endif
