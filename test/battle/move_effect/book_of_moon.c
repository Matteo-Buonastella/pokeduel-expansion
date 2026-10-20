#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(GetMoveEffect(MOVE_BOOK_OF_MOON) == EFFECT_BOOK_OF_MOON);
}

SINGLE_BATTLE_TEST("Book of Moon raises the user's Defense by 1 stage")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_BOOK_OF_MOON); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BOOK_OF_MOON, player);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
        MESSAGE("Wobbuffet's Defense rose!");
    } THEN {
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Book of Moon cures the user's status and raises its Defense by 2 stages")
{
    u32 status1;

    PARAMETRIZE { status1 = STATUS1_BURN; }
    PARAMETRIZE { status1 = STATUS1_POISON; }
    PARAMETRIZE { status1 = STATUS1_TOXIC_POISON; }
    PARAMETRIZE { status1 = STATUS1_PARALYSIS; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Status1(status1); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_BOOK_OF_MOON, WITH_RNG(RNG_PARALYSIS, FALSE)); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BOOK_OF_MOON, player);
        STATUS_ICON(player, none: TRUE);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
        MESSAGE("Wobbuffet's Defense rose sharply!");
    } THEN {
        EXPECT_EQ(player->status1, STATUS1_NONE);
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE + 2);
    }
}

SINGLE_BATTLE_TEST("Book of Moon still cures the user's status if its Defense can't go any higher")
{
    GIVEN {
        ASSUME(GetMoveEffect(MOVE_IRON_DEFENSE) == EFFECT_STAT_CHANGE);
        PLAYER(SPECIES_WOBBUFFET) { Status1(STATUS1_BURN); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_IRON_DEFENSE); }
        TURN { MOVE(player, MOVE_IRON_DEFENSE); }
        TURN { MOVE(player, MOVE_IRON_DEFENSE); }
        TURN { MOVE(player, MOVE_BOOK_OF_MOON); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BOOK_OF_MOON, player);
        STATUS_ICON(player, none: TRUE);
        MESSAGE("Wobbuffet's Defense won't go any higher!");
    } THEN {
        EXPECT_EQ(player->status1, STATUS1_NONE);
        EXPECT_EQ(player->statStages[STAT_DEF], MAX_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Book of Moon fails if the user has no status and its Defense can't go any higher")
{
    GIVEN {
        ASSUME(GetMoveEffect(MOVE_IRON_DEFENSE) == EFFECT_STAT_CHANGE);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_IRON_DEFENSE); }
        TURN { MOVE(player, MOVE_IRON_DEFENSE); }
        TURN { MOVE(player, MOVE_IRON_DEFENSE); }
        TURN { MOVE(player, MOVE_BOOK_OF_MOON); }
    } SCENE {
        NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_BOOK_OF_MOON, player);
        MESSAGE("Wobbuffet's Defense won't go any higher!");
    }
}

SINGLE_BATTLE_TEST("Book of Moon's boost is doubled by Simple")
{
    GIVEN {
        PLAYER(SPECIES_BIDOOF) { Ability(ABILITY_SIMPLE); Status1(STATUS1_BURN); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_BOOK_OF_MOON); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE + 4);
    }
}

AI_SINGLE_BATTLE_TEST("AI doesn't use Book of Moon if it isn't statused and its Defense is maxed")
{
    GIVEN {
        ASSUME(GetMoveEffect(MOVE_IRON_DEFENSE) == EFFECT_STAT_CHANGE);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_BOOK_OF_MOON, MOVE_IRON_DEFENSE, MOVE_SCRATCH); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_IRON_DEFENSE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_IRON_DEFENSE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_IRON_DEFENSE); }
        TURN { MOVE(player, MOVE_CELEBRATE); NOT_EXPECT_MOVE(opponent, MOVE_BOOK_OF_MOON); }
    }
}
