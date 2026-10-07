#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(GetMoveEffect(MOVE_SHRINK) == EFFECT_SHRINK);
    ASSUME(MoveHasAdditionalEffect(MOVE_SHRINK, STAT_CHANGE_EFFECT_MINUS));
}

SINGLE_BATTLE_TEST("Shrink raises the user's Evasion and lowers the target's Attack by 1 stage")
{
    GIVEN {
        PLAYER(SPECIES_ROCKET_WARRIOR);
        OPPONENT(SPECIES_NEOS);
    } WHEN {
        TURN { MOVE(player, MOVE_SHRINK); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SHRINK, player);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, opponent);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_EVASION], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Shrink still raises the user's Evasion if the target's Attack can't be lowered")
{
    GIVEN {
        PLAYER(SPECIES_ROCKET_WARRIOR);
        OPPONENT(SPECIES_CELTIC_GUARDIAN_OF_NOBLE_ARMS) { Ability(ABILITY_HYPER_CUTTER); }
    } WHEN {
        TURN { MOVE(player, MOVE_SHRINK); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SHRINK, player);
        ABILITY_POPUP(opponent, ABILITY_HYPER_CUTTER);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_EVASION], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
    }
}
