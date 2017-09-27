// Created by John Åkerblom 2015-12-15

#ifndef __H3M_HERO_H_DEF__
#define __H3M_HERO_H_DEF__

enum H3M_HERO {
    /* Knight (Castle) */
    H3M_HERO_ORRIN,          /* 0x00 */
    H3M_HERO_VALESKA,        /* 0x01 */
    H3M_HERO_EDRIC,          /* 0x02 */
    H3M_HERO_SYLVIA,         /* 0x03 */
    H3M_HERO_LORD_HAART,     /* 0x04 */
    H3M_HERO_SORSHA,         /* 0x05 */
    H3M_HERO_CHRISTIAN,      /* 0x06 */
    H3M_HERO_TYRIS,          /* 0x07 */

    /* Cleric (Castle) */
    H3M_HERO_RION,           /* 0x08 */
    H3M_HERO_ADELA,          /* 0x09 */
    H3M_HERO_CUTHBERT,       /* 0x0A */
    H3M_HERO_ADELAIDE,       /* 0x0B */
    H3M_HERO_INGHAM,         /* 0x0C */
    H3M_HERO_SANYA,          /* 0x0D */
    H3M_HERO_LOYNIS,         /* 0x0E */
    H3M_HERO_CAITLIN,        /* 0x0F */

    /* Ranger (Rampart) */
    H3M_HERO_MEPHALA,        /* 0x10 */
    H3M_HERO_UFRETIN,        /* 0x11 */
    H3M_HERO_JENOVA,         /* 0x12 */
    H3M_HERO_RYLAND,         /* 0x13 */
    H3M_HERO_THORGRIM,       /* 0x14 */
    H3M_HERO_IVOR,           /* 0x15 */
    H3M_HERO_CLANCY,         /* 0x16 */
    H3M_HERO_KYRRE,          /* 0x17 */

    /* Druid (Rampart) */
    H3M_HERO_CORONIUS,       /* 0x18 */
    H3M_HERO_ULAND,          /* 0x19 */
    H3M_HERO_ELLESHAR,       /* 0x1A */
    H3M_HERO_GEM,            /* 0x1B */
    H3M_HERO_MALCOM,         /* 0x1C */
    H3M_HERO_MELODIA,        /* 0x1D */
    H3M_HERO_ALAGAR,         /* 0x1E */
    H3M_HERO_AERIS,          /* 0x1F */
    
    /* Alchemist (Tower) */
    H3M_HERO_PIQUEDRAM,      /* 0x20 */
    H3M_HERO_THANE,          /* 0x21 */
    H3M_HERO_JOSEPHINE,      /* 0x22 */
    H3M_HERO_NEELA,          /* 0x23 */
    H3M_HERO_TOROSAR ,       /* 0x24 */
    H3M_HERO_FAFNER,         /* 0x25 */
    H3M_HERO_RISSA,          /* 0x26 */
    H3M_HERO_IONA,           /* 0x27 */
    
    /* Wizard (Tower) */
    H3M_HERO_ASTRAL,         /* 0x28 */
    H3M_HERO_HALON,          /* 0x29 */
    H3M_HERO_SERENA,         /* 0x2A */
    H3M_HERO_DAREMYTH,       /* 0x2B */
    H3M_HERO_THEODORUS,      /* 0x2C */
    H3M_HERO_SOLMYR,         /* 0x2D */
    H3M_HERO_CYRA,           /* 0x2E */
    H3M_HERO_AINE,           /* 0x2F */
    
    /* Demoniac (Inferno) */
    H3M_HERO_FIONA,          /* 0x30 */
    H3M_HERO_RASHKA,         /* 0x31 */
    H3M_HERO_MARIUS,         /* 0x32 */
    H3M_HERO_IGNATIUS,       /* 0x33 */
    H3M_HERO_OCTAVIA,        /* 0x34 */
    H3M_HERO_CALH,           /* 0x35 */
    H3M_HERO_PYRE,           /* 0x36 */
    H3M_HERO_NYMUS,          /* 0x37 */
    
    /* Heretic (Inferno) */
    H3M_HERO_AYDEN,          /* 0x38 */
    H3M_HERO_XYRON,          /* 0x39 */
    H3M_HERO_AXSIS,          /* 0x3A */
    H3M_HERO_OLEMA,          /* 0x3B */
    H3M_HERO_CALID,          /* 0x3C */
    H3M_HERO_ASH,            /* 0x3D */
    H3M_HERO_ZYDAR,          /* 0x3E */
    H3M_HERO_XARFAX,         /* 0x3F */
    
    /* Death Knight (Necropolis) */
    H3M_HERO_STRAKER,        /* 0x40 */
    H3M_HERO_VOKIAL,         /* 0x41 */
    H3M_HERO_MOANDOR,        /* 0x42 */
    H3M_HERO_CHARNA,         /* 0x43 */
    H3M_HERO_TAMIKA,         /* 0x44 */
    H3M_HERO_ISRA,           /* 0x45 */
    H3M_HERO_CLAVIUS,        /* 0x46 */
    H3M_HERO_GALTHRAN,       /* 0x47 */
    
    /* Necromancer (Necropolis) */
    H3M_HERO_SEPTIENNA,      /* 0x48 */
    H3M_HERO_AISLINN,        /* 0x49 */
    H3M_HERO_SANDRO,         /* 0x4A */
    H3M_HERO_NIMBUS,         /* 0x4B */
    H3M_HERO_THANT,          /* 0x4C */
    H3M_HERO_XSI,            /* 0x4D */
    H3M_HERO_VIDOMINA,       /* 0x4E */
    H3M_HERO_NAGASH,         /* 0x4F */
    
    /* Overlord (Dungeon) */
    H3M_HERO_LORELEI,        /* 0x50 */
    H3M_HERO_ARLACH,         /* 0x51 */
    H3M_HERO_DACE,           /* 0x52 */
    H3M_HERO_AJIT,           /* 0x53 */
    H3M_HERO_DAMACON,        /* 0x54 */
    H3M_HERO_GUNNAR,         /* 0x55 */
    H3M_HERO_SYNCA,          /* 0x56 */
    H3M_HERO_SHAKTI,         /* 0x57 */

    /* Warlock (Dungeon) */
    H3M_HERO_ALAMAR,         /* 0x58 */
    H3M_HERO_JAEGAR,         /* 0x59 */
    H3M_HERO_MALEKITH,       /* 0x5A */
    H3M_HERO_JEDDITE,        /* 0x5B */
    H3M_HERO_GEON,           /* 0x5C */
    H3M_HERO_DEEMER,         /* 0x5D */
    H3M_HERO_SEPHINROTH,     /* 0x5E */
    H3M_HERO_DARKSTORM,      /* 0x5F */
    

    /* Barbarian (Stronghold) */
    H3M_HERO_YOG,            /* 0x60 */
    H3M_HERO_GURNISSON,      /* 0x61 */
    H3M_HERO_JABARKAS,       /* 0x62 */
    H3M_HERO_SHIVA,          /* 0x63 */
    H3M_HERO_GRETCHIN,       /* 0x64 */
    H3M_HERO_KRELLION,       /* 0x65 */
    H3M_HERO_CRAG_HACK,      /* 0x66 */
    H3M_HERO_TYRAXOR,        /* 0x67 */
    
    /* Battle Mage (Stronghold) */
    H3M_HERO_GIRD,           /* 0x68 */
    H3M_HERO_VEY,            /* 0x69 */
    H3M_HERO_DESSA,          /* 0x6A */
    H3M_HERO_TEREK,          /* 0x6B */
    H3M_HERO_ZUBIN,          /* 0x6C */
    H3M_HERO_GUNDULA,        /* 0x6D */
    H3M_HERO_ORIS,           /* 0x6E */
    H3M_HERO_SAURUG,         /* 0x6F */
    
    /* Beastmaster (Fortress) */
    H3M_HERO_BRON,           /* 0x70 */
    H3M_HERO_DRAKON,         /* 0x71 */
    H3M_HERO_WYSTAN,         /* 0x72 */
    H3M_HERO_TAZAR,          /* 0x73 */
    H3M_HERO_ALKIN,          /* 0x74 */
    H3M_HERO_KORBAC,         /* 0x75 */
    H3M_HERO_GERWULF,        /* 0x76 */
    H3M_HERO_BROGHILD,       /* 0x77 */
    
    /* Witch (Fortress) */
    H3M_HERO_MIRLANDA,       /* 0x78 */
    H3M_HERO_ROSIC,          /* 0x79 */
    H3M_HERO_VOY,            /* 0x7A */
    H3M_HERO_VERDISH,        /* 0x7B */
    H3M_HERO_MERIST,         /* 0x7C */
    H3M_HERO_STYG,           /* 0x7D */
    H3M_HERO_ANDRA,          /* 0x7E */
    H3M_HERO_TIVA,           /* 0x7F */
    
    /* Planeswalker (Conflux) */
    H3M_HERO_PASIS,          /* 0x80 */
    H3M_HERO_THUNAR,         /* 0x81 */
    H3M_HERO_IGNISSA,        /* 0x82 */
    H3M_HERO_LACUS,          /* 0x83 */
    H3M_HERO_MONERE,         /* 0x84 */
    H3M_HERO_ERDAMON,        /* 0x85 */
    H3M_HERO_FIUR,           /* 0x86 */
    H3M_HERO_KALT,           /* 0x87 */
    
    /* Elementalist (Conflux) */
    H3M_HERO_LUNA,           /* 0x88 */
    H3M_HERO_BRISSA,         /* 0x89 */
    H3M_HERO_CIELE,          /* 0x8A */
    H3M_HERO_LABETHA,        /* 0x8B */
    H3M_HERO_INTEUS,         /* 0x8C */
    H3M_HERO_AENAIN,         /* 0x8D */
    H3M_HERO_GELARE,         /* 0x8E */
    H3M_HERO_GRINDAN,        /* 0x8F */

    /* Special  (Campaign) */
    H3M_HERO_SIR_MULLICH,    /* 0x90 */ // IN CHARGE OF BEING BROKEN
    H3M_HERO_ADRIENNE,       /* 0x91 */
    H3M_HERO_CATHERINE,      /* 0x92 */
    H3M_HERO_DRACON,         /* 0x93 */
    H3M_HERO_GELU,           /* 0x94 */
    H3M_HERO_KILGOR,         /* 0x95 */
    H3M_HERO_LORD_HAART2,    /* 0x96 */ // UNDEAD LORD HAART
    H3M_HERO_MUTARE,         /* 0x97 */
    H3M_HERO_ROLAND,         /* 0x98 */
    H3M_HERO_MUTARE_DRAKE,   /* 0x99 */
    H3M_HERO_BORAGUS,        /* 0x9A */
    H3M_HERO_XERON           /* 0x9B */
};

#endif
