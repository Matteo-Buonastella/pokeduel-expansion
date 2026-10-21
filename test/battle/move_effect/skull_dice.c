#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(GetMoveEffect(MOVE_SKULL_DICE) == EFFECT_SKULL_DICE);
}

SINGLE_BATTLE_TEST("Skull Dice lowers a random stat of the target by 1 to 6 stages")
{
    u32 stat, stages;

    PARAMETRIZE { stat = STAT_ATK;   stages = 1; }
    PARAMETRIZE { stat = STAT_DEF;   stages = 2; }
    PARAMETRIZE { stat = STAT_SPEED; stages = 3; }
    PARAMETRIZE { stat = STAT_SPATK; stages = 4; }
    PARAMETRIZE { stat = STAT_SPDEF; stages = 6; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_SKULL_DICE,
                    WITH_RNG(RNG_SKULL_DICE_JACKPOT, FALSE),
                    WITH_RNG(RNG_SKULL_DICE_STAT, stat),
                    WITH_RNG(RNG_SKULL_DICE_STAGES, stages)); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SKULL_DICE, player);
        NOT MESSAGE("Jackpot hit!");
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, opponent);
    } THEN {
        for (u32 i = STAT_ATK; i <= STAT_SPDEF; i++)
            EXPECT_EQ(opponent->statStages[i], i == stat ? DEFAULT_STAT_STAGE - stages : DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Skull Dice has a 1% chance to lower all of the target's stats by 6 stages")
{
    PASSES_RANDOMLY(1, 100, RNG_SKULL_DICE_JACKPOT);
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_SKULL_DICE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SKULL_DICE, player);
        MESSAGE("Jackpot hit!");
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, opponent);
    } THEN {
        for (u32 i = STAT_ATK; i <= STAT_SPDEF; i++)
            EXPECT_EQ(opponent->statStages[i], MIN_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Skull Dice is blocked by Clear Body")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_BELDUM) { Ability(ABILITY_CLEAR_BODY); }
    } WHEN {
        TURN { MOVE(player, MOVE_SKULL_DICE, WITH_RNG(RNG_SKULL_DICE_JACKPOT, FALSE), WITH_RNG(RNG_SKULL_DICE_STAT, STAT_ATK)); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SKULL_DICE, player);
        ABILITY_POPUP(opponent, ABILITY_CLEAR_BODY);
        NOT ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, opponent);
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
    }
}

DOUBLE_BATTLE_TEST("Skull Dice rolls separately for each target")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WYNAUT);
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_SKULL_DICE, WITH_RNG(RNG_SKULL_DICE_JACKPOT, FALSE)); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SKULL_DICE, playerLeft);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, opponentLeft);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, opponentRight);
    }
}

AI_SINGLE_BATTLE_TEST("AI doesn't use Skull Dice if the target's stats can't be lowered")
{
    u32 species, ability;

    PARAMETRIZE { species = SPECIES_BELDUM; ability = ABILITY_CLEAR_BODY; }
    PARAMETRIZE { species = SPECIES_SNIVY;  ability = ABILITY_CONTRARY; }

    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(species) { Ability(ability); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SKULL_DICE, MOVE_SCRATCH); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); EXPECT_MOVE(opponent, MOVE_SCRATCH); }
    }
}

AI_SINGLE_BATTLE_TEST("AI doesn't use Skull Dice if all of the target's stats are already at -6")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SKULL_DICE, MOVE_SCRATCH); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_SKULL_DICE, WITH_RNG(RNG_SKULL_DICE_JACKPOT, TRUE)); }
        TURN { MOVE(player, MOVE_CELEBRATE); EXPECT_MOVE(opponent, MOVE_SCRATCH); }
    }
}
