// Created by John Åkerblom 2015-12-15

#ifndef __H3M_DISPOSITION_H_DEF__
#define __H3M_DISPOSITION_H_DEF__

enum H3M_DISPOSITION {
    H3M_DISPOSITION_COMPLIANT,      // will always join hero
    H3M_DISPOSITION_FRIENDLY,       // likely to join hero
    H3M_DISPOSITION_AGGRESSIVE,     // may join hero
    H3M_DISPOSITION_HOSTILE,        // unlikely to join hero
    H3M_DISPOSITION_SAVAGE          // will never join hero
};

#endif
