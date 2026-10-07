#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(GetMoveEffect(MOVE_SWORDS_OF_REVEALING_LIGHT) == EFFECT_NON_VOLATILE_STATUS);
    ASSUME(GetMoveNonVolatileStatus(MOVE_SWORDS_OF_REVEALING_LIGHT) == MOVE_EFFECT_FREEZE);
}

SINGLE_BATTLE_TEST("Swords of Revealing Light freezes the target")
{
    GIVEN {
        ASSUME(GetSpeciesType(SPECIES_NEOS, 0) != TYPE_ICE);
        ASSUME(GetSpeciesType(SPECIES_NEOS, 1) != TYPE_ICE);
        PLAYER(SPECIES_BEAVER_WARRIOR) { Speed(1); }
        OPPONENT(SPECIES_NEOS) { Speed(100); }
    } WHEN {
        // Neos moves first so it doesn't get a thaw roll after being frozen
        TURN { MOVE(opponent, MOVE_TACKLE); MOVE(player, MOVE_SWORDS_OF_REVEALING_LIGHT); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SWORDS_OF_REVEALING_LIGHT, player);
        STATUS_ICON(opponent, freeze: TRUE);
    } THEN {
        EXPECT(opponent->status1 & STATUS1_FREEZE);
    }
}

SINGLE_BATTLE_TEST("Swords of Revealing Light fails if the target already has a status condition")
{
    GIVEN {
        PLAYER(SPECIES_BEAVER_WARRIOR);
        OPPONENT(SPECIES_NEOS) { Status1(STATUS1_POISON); }
    } WHEN {
        TURN { MOVE(player, MOVE_SWORDS_OF_REVEALING_LIGHT); }
    } SCENE {
        NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_SWORDS_OF_REVEALING_LIGHT, player);
        MESSAGE("But it failed!");
    } THEN {
        EXPECT_EQ(opponent->status1, STATUS1_POISON);
    }
}
