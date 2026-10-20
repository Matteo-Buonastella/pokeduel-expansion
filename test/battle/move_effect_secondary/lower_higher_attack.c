#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(MoveHasAdditionalEffect(MOVE_HONEST_CRYING, MOVE_EFFECT_LOWER_HIGHER_ATTACK) == TRUE);
}

SINGLE_BATTLE_TEST("Honest Crying lowers the target's Attack if it is higher than its Sp. Atk")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Attack(200); SpAttack(100); }
    } WHEN {
        TURN { MOVE(player, MOVE_HONEST_CRYING); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HONEST_CRYING, player);
        HP_BAR(opponent);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, opponent);
        MESSAGE("The opposing Wobbuffet's Attack fell!");
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(opponent->statStages[STAT_SPATK], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Honest Crying lowers the target's Sp. Atk if it is higher than its Attack")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Attack(100); SpAttack(200); }
    } WHEN {
        TURN { MOVE(player, MOVE_HONEST_CRYING); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HONEST_CRYING, player);
        HP_BAR(opponent);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, opponent);
        MESSAGE("The opposing Wobbuffet's Sp. Atk fell!");
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(opponent->statStages[STAT_SPATK], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Honest Crying lowers the target's Sp. Atk if its Attack and Sp. Atk are equal")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Attack(100); SpAttack(100); }
    } WHEN {
        TURN { MOVE(player, MOVE_HONEST_CRYING); }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(opponent->statStages[STAT_SPATK], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Honest Crying takes stat stages into account when comparing Attack and Sp. Atk")
{
    GIVEN {
        ASSUME(GetMoveEffect(MOVE_SWORDS_DANCE) == EFFECT_STAT_CHANGE);
        PLAYER(SPECIES_WOBBUFFET) { Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Attack(100); SpAttack(150); Speed(2); }
    } WHEN {
        // +2 Attack: 100 * 2 = 200 > 150 Sp. Atk
        TURN { MOVE(opponent, MOVE_SWORDS_DANCE); MOVE(player, MOVE_HONEST_CRYING); }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(opponent->statStages[STAT_SPATK], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Honest Crying doesn't lower stats if the user has Sheer Force")
{
    GIVEN {
        PLAYER(SPECIES_TAUROS) { Ability(ABILITY_SHEER_FORCE); }
        OPPONENT(SPECIES_WOBBUFFET) { Attack(200); SpAttack(100); }
    } WHEN {
        TURN { MOVE(player, MOVE_HONEST_CRYING); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HONEST_CRYING, player);
        NOT ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, opponent);
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Honest Crying raises the higher attacking stat of a target with Contrary")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_SNIVY) { Ability(ABILITY_CONTRARY); Attack(200); SpAttack(100); }
    } WHEN {
        TURN { MOVE(player, MOVE_HONEST_CRYING); }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
    }
}
