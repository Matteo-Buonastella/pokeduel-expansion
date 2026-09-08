#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(GetMoveEffect(MOVE_OBLITERATE) == EFFECT_OHKO);
}


SINGLE_BATTLE_TEST("Obliterate works if target's level is higher than yours")
{
    GIVEN {
        PLAYER(SPECIES_EXODIA_THE_FORBIDDEN_ONE) { Level(1); }
        OPPONENT(SPECIES_LATIOS) { Level(2); }
    } WHEN {
        TURN { MOVE(player, MOVE_OBLITERATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_OBLITERATE, player);
    }
}

SINGLE_BATTLE_TEST("Obliterate works if target's level is lower than yours")
{
    GIVEN {
        PLAYER(SPECIES_EXODIA_THE_FORBIDDEN_ONE) { Level(100); }
        OPPONENT(SPECIES_LATIOS) { Level(99); }
    } WHEN {
        TURN { MOVE(player, MOVE_OBLITERATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_OBLITERATE, player);
    }
}