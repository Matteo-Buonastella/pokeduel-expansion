#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(MoveHasAdditionalEffectSelf(MOVE_DIAMOND_BLADE_STRIKE, MOVE_EFFECT_STAT_PLUS_OR_MINUS) == TRUE);
}

SINGLE_BATTLE_TEST("Diamond Blade Strike has a 10% chance to change all of the user's stats")
{
    PASSES_RANDOMLY(10, 100, RNG_SECONDARY_EFFECT);
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_DIAMOND_BLADE_STRIKE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DIAMOND_BLADE_STRIKE, player);
        HP_BAR(opponent);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
    }
}

SINGLE_BATTLE_TEST("Diamond Blade Strike raises all of the user's stats by 1 stage half the time it activates")
{
    PASSES_RANDOMLY(50, 100, RNG_STAT_PLUS_OR_MINUS);
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_DIAMOND_BLADE_STRIKE, WITH_RNG(RNG_SECONDARY_EFFECT, TRUE)); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Diamond Blade Strike lowers all of the user's stats by 1 stage half the time it activates")
{
    PASSES_RANDOMLY(50, 100, RNG_STAT_PLUS_OR_MINUS);
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_DIAMOND_BLADE_STRIKE, WITH_RNG(RNG_SECONDARY_EFFECT, TRUE)); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Diamond Blade Strike doesn't change the user's stats if the user has Sheer Force")
{
    GIVEN {
        PLAYER(SPECIES_TAUROS) { Ability(ABILITY_SHEER_FORCE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_DIAMOND_BLADE_STRIKE, WITH_RNG(RNG_SECONDARY_EFFECT, TRUE)); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DIAMOND_BLADE_STRIKE, player);
        NOT ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
    }
}
