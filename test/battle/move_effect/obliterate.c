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
SINGLE_BATTLE_TEST("Obliterate's accuracy drops by 1% for each level the target is higher")
{
    // 40% accuracy - 10 levels = 30%
    PASSES_RANDOMLY(30, 100, RNG_ACCURACY);
    GIVEN {
        ASSUME(GetMoveAccuracy(MOVE_OBLITERATE) == 40);
        PLAYER(SPECIES_EXODIA_THE_FORBIDDEN_ONE) { Level(50); }
        OPPONENT(SPECIES_LATIOS) { Level(60); }
    } WHEN {
        TURN { MOVE(player, MOVE_OBLITERATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_OBLITERATE, player);
        HP_BAR(opponent, hp: 0);
    }
}

SINGLE_BATTLE_TEST("Obliterate can't hit if the level gap brings its accuracy to 0% or below")
{
    GIVEN {
        ASSUME(GetMoveAccuracy(MOVE_OBLITERATE) == 40);
        PLAYER(SPECIES_EXODIA_THE_FORBIDDEN_ONE) { Level(10); }
        OPPONENT(SPECIES_LATIOS) { Level(60); }
    } WHEN {
        TURN { MOVE(player, MOVE_OBLITERATE); }
    } SCENE {
        NONE_OF {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_OBLITERATE, player);
            HP_BAR(opponent, hp: 0);
        }
    }
}
