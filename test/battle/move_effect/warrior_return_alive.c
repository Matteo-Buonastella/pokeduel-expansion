#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(GetMoveEffect(MOVE_WARRIOR_RETURN_ALIVE) == EFFECT_WARRIOR_RETURN_ALIVE);
}

SINGLE_BATTLE_TEST("The Warrior Returning Alive heals 50% of max HP and raises Attack by 1 stage at 1/3 HP or less")
{
    s16 healed;

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(100); MaxHP(300); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_WARRIOR_RETURN_ALIVE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WARRIOR_RETURN_ALIVE, player);
        HP_BAR(player, captureDamage: &healed);
        MESSAGE("Wobbuffet had its HP restored.");
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
        MESSAGE("Wobbuffet's Attack rose!");
    } THEN {
        EXPECT_EQ(healed, -150);
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("The Warrior Returning Alive fails above 1/3 HP")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(101); MaxHP(300); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_WARRIOR_RETURN_ALIVE); }
    } SCENE {
        NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_WARRIOR_RETURN_ALIVE, player);
        MESSAGE("But it failed!");
    } THEN {
        EXPECT_EQ(player->hp, 101);
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("The Warrior Returning Alive still heals if Attack can't go any higher")
{
    GIVEN {
        ASSUME(GetMoveEffect(MOVE_BELLY_DRUM) == EFFECT_BELLY_DRUM);
        PLAYER(SPECIES_WOBBUFFET) { HP(250); MaxHP(300); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_BELLY_DRUM); } // 250 - 150 = 100 HP, +6 Attack
        TURN { MOVE(player, MOVE_WARRIOR_RETURN_ALIVE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WARRIOR_RETURN_ALIVE, player);
        HP_BAR(player);
        MESSAGE("Wobbuffet's Attack won't go any higher!");
    } THEN {
        EXPECT_EQ(player->hp, 250);
    }
}

AI_SINGLE_BATTLE_TEST("AI doesn't use The Warrior Returning Alive above 1/3 HP")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(150); MaxHP(300); Moves(MOVE_WARRIOR_RETURN_ALIVE, MOVE_SCRATCH); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); EXPECT_MOVE(opponent, MOVE_SCRATCH); }
    }
}

AI_SINGLE_BATTLE_TEST("AI uses The Warrior Returning Alive at low HP")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_SCRATCH); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(60); MaxHP(300); Moves(MOVE_WARRIOR_RETURN_ALIVE, MOVE_SPLASH); }
    } WHEN {
        TURN { MOVE(player, MOVE_SCRATCH); EXPECT_MOVE(opponent, MOVE_WARRIOR_RETURN_ALIVE); }
    }
}
