#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(GetMoveEffect(MOVE_POWER_BOND) == EFFECT_POWER_BOND);
}

SINGLE_BATTLE_TEST("Power Bond sharply raises the user's Attack before hitting")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { MaxHP(500); HP(500); Attack(100); }
        OPPONENT(SPECIES_WOBBUFFET) { MaxHP(999); HP(999); }
    } WHEN {
        TURN { MOVE(player, MOVE_POWER_BOND); }
    } SCENE {
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
        MESSAGE("Wobbuffet's Attack rose sharply!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_POWER_BOND, player);
        HP_BAR(opponent);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 2);
    }
}

SINGLE_BATTLE_TEST("Power Bond's damage benefits from its own Attack boost", s16 damage)
{
    bool32 preBoosted;

    PARAMETRIZE { preBoosted = FALSE; }
    PARAMETRIZE { preBoosted = TRUE; }

    GIVEN {
        ASSUME(GetMoveEffect(MOVE_SWORDS_DANCE) == EFFECT_STAT_CHANGE);
        ASSUME(GetMoveEffect(MOVE_CELEBRATE) == EFFECT_CELEBRATE);
        PLAYER(SPECIES_WOBBUFFET) { MaxHP(999); HP(999); Attack(100); }
        OPPONENT(SPECIES_WOBBUFFET) { MaxHP(999); HP(999); }
    } WHEN {
        // +2 then Power Bond's own +2 gives +4 (3x); without the pre-boost it's +2 (2x)
        TURN { MOVE(player, preBoosted ? MOVE_SWORDS_DANCE : MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_POWER_BOND); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_POWER_BOND, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(1.5), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Power Bond's recoil is equal to the user's Attack stat")
{
    s16 recoil;

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { MaxHP(500); HP(500); Attack(150); }
        OPPONENT(SPECIES_WOBBUFFET) { MaxHP(999); HP(999); }
    } WHEN {
        TURN { MOVE(player, MOVE_POWER_BOND); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_POWER_BOND, player);
        HP_BAR(opponent);
        HP_BAR(player, captureDamage: &recoil);
    } THEN {
        EXPECT_EQ(recoil, 150);
    }
}

SINGLE_BATTLE_TEST("Power Bond doesn't raise Attack or deal recoil if it misses")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { MaxHP(500); HP(500); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_POWER_BOND, hit: FALSE); }
    } SCENE {
        NOT ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
        NOT HP_BAR(player);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(player->hp, 500);
    }
}

SINGLE_BATTLE_TEST("Power Bond's recoil is prevented by Rock Head")
{
    GIVEN {
        PLAYER(SPECIES_AERODACTYL) { Ability(ABILITY_ROCK_HEAD); MaxHP(500); HP(500); }
        OPPONENT(SPECIES_WOBBUFFET) { MaxHP(999); HP(999); }
    } WHEN {
        TURN { MOVE(player, MOVE_POWER_BOND); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_POWER_BOND, player);
        HP_BAR(opponent);
        NOT HP_BAR(player);
    } THEN {
        EXPECT_EQ(player->hp, 500);
    }
}

AI_SINGLE_BATTLE_TEST("AI doesn't use Power Bond if the recoil would KO it without KOing the target")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_WOBBUFFET) { MaxHP(999); HP(999); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(50); Attack(100); Moves(MOVE_POWER_BOND, MOVE_SCRATCH); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); EXPECT_MOVE(opponent, MOVE_SCRATCH); }
    }
}
