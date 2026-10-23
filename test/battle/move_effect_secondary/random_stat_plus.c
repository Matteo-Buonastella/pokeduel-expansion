#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(MoveHasAdditionalEffectSelf(MOVE_MAJESTIC_SLASH, MOVE_EFFECT_RANDOM_STAT_PLUS) == TRUE);
}

SINGLE_BATTLE_TEST("Majestic Slash raises one of the user's Attack, Defense or Speed at random")
{
    u32 roll;
    enum Stat stat;

    PARAMETRIZE { roll = 0; stat = STAT_ATK; }
    PARAMETRIZE { roll = 1; stat = STAT_DEF; }
    PARAMETRIZE { roll = 2; stat = STAT_SPEED; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_MAJESTIC_SLASH, WITH_RNG(RNG_RANDOM_STAT_PLUS, roll)); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_MAJESTIC_SLASH, player);
        HP_BAR(opponent);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + (stat == STAT_ATK));
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE + (stat == STAT_DEF));
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + (stat == STAT_SPEED));
    }
}

SINGLE_BATTLE_TEST("Majestic Slash skips stats that can't go any higher")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_SWORDS_DANCE); }
        TURN { MOVE(player, MOVE_SWORDS_DANCE); }
        TURN { MOVE(player, MOVE_SWORDS_DANCE); }
        TURN { MOVE(player, MOVE_MAJESTIC_SLASH, WITH_RNG(RNG_RANDOM_STAT_PLUS, 0)); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_MAJESTIC_SLASH, player);
        MESSAGE("Wobbuffet's Defense rose!");
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], MAX_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Majestic Slash doesn't raise a stat if the user has Sheer Force")
{
    GIVEN {
        PLAYER(SPECIES_TAUROS) { Ability(ABILITY_SHEER_FORCE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_MAJESTIC_SLASH); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_MAJESTIC_SLASH, player);
        NOT ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
    }
}
