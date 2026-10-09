#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(GetMoveEffect(MOVE_MIRROR_WALL) == EFFECT_MIRROR_WALL);
    ASSUME(GetMoveEffect(MOVE_CELEBRATE) == EFFECT_CELEBRATE);
}

SINGLE_BATTLE_TEST("Mirror Wall cuts the user's HP in half and maximizes its Defense")
{
    GIVEN {
        PLAYER(SPECIES_SHUNOROS) { Ability(ABILITY_MIRROR_ARMOR); }
        OPPONENT(SPECIES_NEOS);
    } WHEN {
        TURN { MOVE(player, MOVE_MIRROR_WALL); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        s32 maxHP = GetMonData(&PLAYER_PARTY[0], MON_DATA_MAX_HP);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_MIRROR_WALL, player);
        HP_BAR(player, hp: maxHP - maxHP / 2);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
        MESSAGE("Shunoros cut its own HP and maximized its Defense!");
    } THEN {
        EXPECT_EQ(player->statStages[STAT_DEF], MAX_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Mirror Wall fails if the user's current HP is half or less than half its maximum")
{
    GIVEN {
        PLAYER(SPECIES_SHUNOROS) { Ability(ABILITY_MIRROR_ARMOR); MaxHP(100); HP(50); }
        OPPONENT(SPECIES_NEOS);
    } WHEN {
        TURN { MOVE(player, MOVE_MIRROR_WALL); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        MESSAGE("But it failed!");
        NONE_OF {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_MIRROR_WALL, player);
            HP_BAR(player);
            ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
        }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Mirror Wall fails if the user's Defense is already at +6")
{
    GIVEN {
        ASSUME_STAT_CHANGE(MOVE_IRON_DEFENSE, defense: +2);
        PLAYER(SPECIES_SHUNOROS) { Ability(ABILITY_MIRROR_ARMOR); }
        OPPONENT(SPECIES_NEOS);
    } WHEN {
        TURN { MOVE(player, MOVE_IRON_DEFENSE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_IRON_DEFENSE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_IRON_DEFENSE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_MIRROR_WALL); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_IRON_DEFENSE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_IRON_DEFENSE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_IRON_DEFENSE, player);
        MESSAGE("But it failed!");
        NONE_OF {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_MIRROR_WALL, player);
            HP_BAR(player);
        }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_DEF], MAX_STAT_STAGE);
    }
}
