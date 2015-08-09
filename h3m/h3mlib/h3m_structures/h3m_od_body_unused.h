#if 0

// This file contains currently unused structure definitions
// of binary compatible structs. Currently corresponding
// dynamic structures are always used instead.

#pragma pack(push, 1)

// BINARY COMPATIBLE
struct BODY_TOWN_ROE {
    uint8_t owner;
    uint8_t has_name;           // MUST be 0x00 for this struct
    uint8_t has_creatures;      // MUST be 0x00 for this struct
    uint8_t formation;
    uint8_t has_buildings;      // MUST be 0x00 for this struct
    uint8_t has_fort;
    uint8_t unknown4[16];       // default 0x00
};

// BINARY COMPATIBLE
struct BODY_HERO_ROE {
    // ROE - no unknown1 here
    uint8_t owner;
    uint8_t type;
    uint8_t has_name;           // MUST be 0x00 for this struct
    // ROE - no has_experience here
    uint32_t experience;
    uint8_t has_face;           // MUST be 0x00 for this struct
    uint8_t has_secondary_skills;       // MUST be 0x00 for this struct
    uint8_t has_creatures;      // MUST be 0x00 for this struct
    uint8_t formation;          // 0 - Spread, 1 - Grouped
    uint8_t has_artifacts;      // MUST be 0x00 for this struct
    uint8_t patrol_radius;      // default 0xFF
    // ROE - no has_biography here
    // ROE - no gender here
    // ROE - no has_spells here
    // ROE - no has_primary_skills here
    uint8_t unknown2[16];       // default 0x00
};

// BINARY COMPATIBLE
struct BODY_HERO_AB {
    uint32_t unknown1;
    uint8_t owner;
    uint8_t type;
    uint8_t has_name;           // MUST be 0x00 for this struct
    // AB - no has_experience here
    uint32_t experience;
    uint8_t has_face;           // MUST be 0x00 for this struct
    uint8_t has_secondary_skills;       // MUST be 0x00 for this struct
    uint8_t has_creatures;      // MUST be 0x00 for this struct
    uint8_t formation;          // 0 - Spread, 1 - Grouped
    uint8_t has_artifacts;      // MUST be 0x00 for this struct
    uint8_t patrol_radius;      // default 0xFF
    uint8_t has_biography;      // AB/SOD feature, MUST be 0x00 for this struct
    uint8_t gender;             // AB/SOD feature
    // AB - no has_spells here
    // AB - no has_primary_skills here
    uint8_t unknown2[16];       // default 0x00
};

// BINARY COMPATIBLE
struct BODY_HERO_SOD {
    uint32_t unknown1;
    uint8_t owner;
    uint8_t type;
    uint8_t has_name;           // MUST be 0x00 for this struct
    uint8_t has_experience;     // SOD feature, MUST be 0x00 for this struct
    uint8_t has_face;           // MUST be 0x00 for this struct
    uint8_t has_secondary_skills;       // MUST be 0x00 for this struct
    uint8_t has_creatures;      // MUST be 0x00 for this struct
    uint8_t formation;          // 0 - Spread, 1 - Grouped
    uint8_t has_artifacts;      // MUST be 0x00 for this struct
    uint8_t patrol_radius;      // default 0xFF
    uint8_t has_biography;      // AB/SOD feature, MUST be 0x00 for this struct
    uint8_t gender;             // AB/SOD feature
    uint8_t has_spells;         // SOD feature, MUST be 0x00 for this struct
    uint8_t has_primary_skills; // SOD feature, MUST be 0x00 for this struct
    uint8_t unknown2[16];       // default 0x00
};

// BINARY COMPATIBLE
struct BODY_MONSTER_WITH_AI_EXCL_MESG {
    uint16_t quantity;
    uint8_t disposition;        // likelyhood of joining hero
    uint8_t has_ai;             // CANNOT be 0x00 for this struct
    struct {
        uint32_t mesg_size;     // MUST be 0x00 for this struct
        // uint8_t *mesg;
        uint32_t wood;
        uint32_t mercury;
        uint32_t ore;
        uint32_t sulfur;
        uint32_t crystal;
        uint32_t gems;
        uint32_t gold;
        uint16_t artifact;      // default 0xFFFF
    } ai;
    uint8_t never_flees;
    uint8_t does_not_grow;
    uint8_t unknown1[2];
};

// NOT BINARY COMPATIBLE, contains dynamic elements
struct BODY_MONSTER_WITH_AI_INCL_MESG {
    uint16_t quantity;
    uint8_t disposition;        // likelyhood of joining hero
    uint8_t has_ai;             // CANNOT be 0x00 for this struct
    struct {
        uint32_t mesg_size;     // MUST be 0x00 for this struct
        uint8_t *mesg;
        uint32_t wood;
        uint32_t mercury;
        uint32_t ore;
        uint32_t sulfur;
        uint32_t crystal;
        uint32_t gems;
        uint32_t gold;
        uint16_t artifact;      // default 0xFFFF
    } ai;
    uint8_t never_flees;
    uint8_t does_not_grow;
    uint8_t unknown2[2];
};

#pragma pack(pop)

#endif
