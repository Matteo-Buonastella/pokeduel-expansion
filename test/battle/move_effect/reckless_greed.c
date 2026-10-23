#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(GetMoveEffect(MOVE_RECKLESS_GREED) == EFFECT_RECKLESS_GREED);
}

SINGLE_BATTLE_TEST("Reckless Greed sharply raises the user's Attack and Sp. Atk, then the user must recharge")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_RECKLESS_GREED); }
        TURN { SKIP_TURN(player); }
        TURN { MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_RECKLESS_GREED, player);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
        MESSAGE("Wobbuffet's Attack sharply rose!");
        MESSAGE("Wobbuffet's Sp. Atk sharply rose!");
        MESSAGE("Wobbuffet must recharge!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 2);
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 2);
    }
}

SINGLE_BATTLE_TEST("Reckless Greed fails without a recharge turn if neither stat can rise")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_RECKLESS_GREED); }
        TURN { SKIP_TURN(player); }
        TURN { MOVE(player, MOVE_RECKLESS_GREED); }
        TURN { SKIP_TURN(player); }
        TURN { MOVE(player, MOVE_RECKLESS_GREED); }
        TURN { SKIP_TURN(player); }
        TURN { MOVE(player, MOVE_RECKLESS_GREED); }
        TURN { MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_RECKLESS_GREED, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_RECKLESS_GREED, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_RECKLESS_GREED, player);
        NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_RECKLESS_GREED, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], MAX_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_SPATK], MAX_STAT_STAGE);
    }
}

AI_SINGLE_BATTLE_TEST("AI doesn't use Reckless Greed if its Attack and Sp. Atk can't rise")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_RECKLESS_GREED, MOVE_SCRATCH); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); EXPECT_MOVE(opponent, MOVE_RECKLESS_GREED); }
        TURN { MOVE(player, MOVE_CELEBRATE); SKIP_TURN(opponent); }
        TURN { MOVE(player, MOVE_CELEBRATE); EXPECT_MOVE(opponent, MOVE_RECKLESS_GREED); }
        TURN { MOVE(player, MOVE_CELEBRATE); SKIP_TURN(opponent); }
        TURN { MOVE(player, MOVE_CELEBRATE); EXPECT_MOVE(opponent, MOVE_RECKLESS_GREED); }
        TURN { MOVE(player, MOVE_CELEBRATE); SKIP_TURN(opponent); }
        TURN { MOVE(player, MOVE_CELEBRATE); EXPECT_MOVE(opponent, MOVE_SCRATCH); }
    }
}
