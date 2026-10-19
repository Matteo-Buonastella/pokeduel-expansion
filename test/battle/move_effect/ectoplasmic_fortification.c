#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(GetMoveEffect(MOVE_ECTOPLASMIC_FORTIFICATION) == EFFECT_ECTOPLASMIC_FORTIFICATION);
}

SINGLE_BATTLE_TEST("Ectoplasmic Fortification raises Sp. Atk and Sp. Def by 1 stage and restores 1/6 of the user's max HP")
{
    s16 healed;

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(1); MaxHP(600); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_ECTOPLASMIC_FORTIFICATION); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ECTOPLASMIC_FORTIFICATION, player);
        HP_BAR(player, captureDamage: &healed);
        MESSAGE("Wobbuffet had its HP restored.");
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
    } THEN {
        EXPECT_EQ(healed, -100);
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Ectoplasmic Fortification still raises stats when the user is at full HP")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_ECTOPLASMIC_FORTIFICATION); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ECTOPLASMIC_FORTIFICATION, player);
        NOT HP_BAR(player);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Ectoplasmic Fortification still restores HP when Sp. Atk and Sp. Def are maxed")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(1); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_CALM_MIND); }
        TURN { MOVE(player, MOVE_CALM_MIND); }
        TURN { MOVE(player, MOVE_CALM_MIND); }
        TURN { MOVE(player, MOVE_CALM_MIND); }
        TURN { MOVE(player, MOVE_CALM_MIND); }
        TURN { MOVE(player, MOVE_CALM_MIND); }
        TURN { MOVE(player, MOVE_ECTOPLASMIC_FORTIFICATION); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ECTOPLASMIC_FORTIFICATION, player);
        HP_BAR(player);
        NOT ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
    } THEN {
        EXPECT_GT(player->hp, 1);
    }
}

SINGLE_BATTLE_TEST("Ectoplasmic Fortification fails when the user is at full HP and Sp. Atk and Sp. Def are maxed")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_CALM_MIND); }
        TURN { MOVE(player, MOVE_CALM_MIND); }
        TURN { MOVE(player, MOVE_CALM_MIND); }
        TURN { MOVE(player, MOVE_CALM_MIND); }
        TURN { MOVE(player, MOVE_CALM_MIND); }
        TURN { MOVE(player, MOVE_CALM_MIND); }
        TURN { MOVE(player, MOVE_ECTOPLASMIC_FORTIFICATION); }
    } SCENE {
        NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_ECTOPLASMIC_FORTIFICATION, player);
    }
}

SINGLE_BATTLE_TEST("Ectoplasmic Fortification still raises stats but doesn't heal under Heal Block")
{
    GIVEN {
        ASSUME(GetMoveEffect(MOVE_HEAL_BLOCK) == EFFECT_HEAL_BLOCK);
        PLAYER(SPECIES_WOBBUFFET) { HP(1); Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(2); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_HEAL_BLOCK); MOVE(player, MOVE_ECTOPLASMIC_FORTIFICATION); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HEAL_BLOCK, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ECTOPLASMIC_FORTIFICATION, player);
        NOT HP_BAR(player);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
    } THEN {
        EXPECT_EQ(player->hp, 1);
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE + 1);
    }
}

AI_SINGLE_BATTLE_TEST("AI uses Ectoplasmic Fortification to heal and boost")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(100); MaxHP(300); Moves(MOVE_ECTOPLASMIC_FORTIFICATION, MOVE_SPLASH); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); EXPECT_MOVE(opponent, MOVE_ECTOPLASMIC_FORTIFICATION); }
    }
}
