#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(MoveHasAdditionalEffectSelf(MOVE_CELESTIAL_SWORD, MOVE_EFFECT_STAT_PLUS));
    // Wing Attack is used to faint the foes, so it mustn't roll a secondary effect
    ASSUME(GetMoveAdditionalEffectCount(MOVE_WING_ATTACK) == 0);
}

SINGLE_BATTLE_TEST("Celestial Sword has a 10% chance to raise the user's Attack if none of the foe's mons have fainted")
{
    PASSES_RANDOMLY(10, 100, RNG_SECONDARY_EFFECT);
    GIVEN {
        PLAYER(SPECIES_GUARDIAN_EATOS);
        OPPONENT(SPECIES_NEOS) { MaxHP(999); HP(999); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELESTIAL_SWORD); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELESTIAL_SWORD, player);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
    }
}

SINGLE_BATTLE_TEST("Celestial Sword's chance to raise the user's Attack is 10% for each of the foe's fainted mons")
{
    // 2 fainted foes = 20%
    PASSES_RANDOMLY(20, 100, RNG_SECONDARY_EFFECT);
    GIVEN {
        PLAYER(SPECIES_GUARDIAN_EATOS) { Speed(100); }
        OPPONENT(SPECIES_LITTLE_WINGUARD) { HP(1); Speed(1); }
        OPPONENT(SPECIES_SANGAN) { HP(1); Speed(1); }
        OPPONENT(SPECIES_NEOS) { MaxHP(999); HP(999); Speed(1); }
    } WHEN {
        TURN { MOVE(player, MOVE_WING_ATTACK); SEND_OUT(opponent, 1); }
        TURN { MOVE(player, MOVE_WING_ATTACK); SEND_OUT(opponent, 2); }
        TURN { MOVE(player, MOVE_CELESTIAL_SWORD); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WING_ATTACK, player);
        HP_BAR(opponent, hp: 0);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WING_ATTACK, player);
        HP_BAR(opponent, hp: 0);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELESTIAL_SWORD, player);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
    }
}
