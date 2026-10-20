#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(GetMoveEffect(MOVE_FROGGY_FORCEFIELD) == EFFECT_FROGGY_FORCEFIELD);
    ASSUME(gSpeciesInfo[SPECIES_FROG_THE_JAM].isFrog);
    ASSUME(!gSpeciesInfo[SPECIES_WOBBUFFET].isFrog);
    ASSUME(GetMoveCategory(MOVE_SCRATCH) == DAMAGE_CATEGORY_PHYSICAL);
    ASSUME(GetMoveCategory(MOVE_WATER_GUN) == DAMAGE_CATEGORY_SPECIAL);
}

SINGLE_BATTLE_TEST("Froggy Forcefield boosts the Def and Sp. Def of the user's Frogs by 33%", s16 damage)
{
    enum Move move, setupMove;

    PARAMETRIZE { move = MOVE_SCRATCH;   setupMove = MOVE_FROGGY_FORCEFIELD; }
    PARAMETRIZE { move = MOVE_SCRATCH;   setupMove = MOVE_CELEBRATE; }
    PARAMETRIZE { move = MOVE_WATER_GUN; setupMove = MOVE_FROGGY_FORCEFIELD; }
    PARAMETRIZE { move = MOVE_WATER_GUN; setupMove = MOVE_CELEBRATE; }

    GIVEN {
        PLAYER(SPECIES_FROG_THE_JAM) { MaxHP(500); HP(500); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SCRATCH, MOVE_WATER_GUN, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, setupMove); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(opponent, move); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, opponent);
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(1.33), results[1].damage);
        EXPECT_MUL_EQ(results[2].damage, Q_4_12(1.33), results[3].damage);
    }
}

SINGLE_BATTLE_TEST("Froggy Forcefield doesn't boost the defenses of monsters that aren't Frogs", s16 damage)
{
    enum Move setupMove;

    PARAMETRIZE { setupMove = MOVE_FROGGY_FORCEFIELD; }
    PARAMETRIZE { setupMove = MOVE_CELEBRATE; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { MaxHP(500); HP(500); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SCRATCH, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, setupMove); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(opponent, MOVE_SCRATCH); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, opponent);
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Froggy Forcefield doesn't boost the defenses of the opposing side's Frogs", s16 damage)
{
    enum Move setupMove;

    PARAMETRIZE { setupMove = MOVE_FROGGY_FORCEFIELD; }
    PARAMETRIZE { setupMove = MOVE_CELEBRATE; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_FROG_THE_JAM) { MaxHP(500); HP(500); }
    } WHEN {
        TURN { MOVE(player, setupMove); }
        TURN { MOVE(player, MOVE_SCRATCH); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Froggy Forcefield lasts for 5 turns")
{
    GIVEN {
        PLAYER(SPECIES_FROG_THE_JAM);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_FROGGY_FORCEFIELD); }
        TURN {}
        TURN {}
        TURN {}
        TURN {}
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FROGGY_FORCEFIELD, player);
        MESSAGE("A Froggy Forcefield is protecting the Frogs on your team!");
        MESSAGE("Your side's Froggy Forcefield wore off!");
    } THEN {
        EXPECT(!(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_FROGGY_FORCEFIELD));
    }
}

SINGLE_BATTLE_TEST("Froggy Forcefield lasts for 8 turns if the user is holding Light Clay")
{
    GIVEN {
        ASSUME(GetItemHoldEffect(ITEM_LIGHT_CLAY) == HOLD_EFFECT_LIGHT_CLAY);
        PLAYER(SPECIES_FROG_THE_JAM) { Item(ITEM_LIGHT_CLAY); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_FROGGY_FORCEFIELD); }
        TURN {}
        TURN {}
        TURN {}
        TURN {}
    } SCENE {
        NOT MESSAGE("Your side's Froggy Forcefield wore off!");
    } THEN {
        EXPECT(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_FROGGY_FORCEFIELD);
        EXPECT_EQ(gSideTimers[B_SIDE_PLAYER].froggyForcefieldTimer, 3);
    }
}

SINGLE_BATTLE_TEST("Froggy Forcefield fails if it is already active on the user's side")
{
    GIVEN {
        PLAYER(SPECIES_FROG_THE_JAM);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_FROGGY_FORCEFIELD); }
        TURN { MOVE(player, MOVE_FROGGY_FORCEFIELD); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FROGGY_FORCEFIELD, player);
        MESSAGE("But it failed!");
    }
}

SINGLE_BATTLE_TEST("Froggy Forcefield and Des-Croaking can both be active at once")
{
    GIVEN {
        ASSUME(GetMoveEffect(MOVE_DES_CROAKING) == EFFECT_DES_CROAKING);
        PLAYER(SPECIES_FROG_THE_JAM);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_DES_CROAKING); }
        TURN { MOVE(player, MOVE_FROGGY_FORCEFIELD); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DES_CROAKING, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FROGGY_FORCEFIELD, player);
        NOT MESSAGE("But it failed!");
    } THEN {
        EXPECT(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_DES_CROAKING);
        EXPECT(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_FROGGY_FORCEFIELD);
    }
}

AI_SINGLE_BATTLE_TEST("AI uses Froggy Forcefield when it has Frogs in its party")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_FROGGY_FORCEFIELD, MOVE_SPLASH); }
        OPPONENT(SPECIES_FROG_THE_JAM) { Moves(MOVE_SCRATCH); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); EXPECT_MOVE(opponent, MOVE_FROGGY_FORCEFIELD); }
    }
}

AI_SINGLE_BATTLE_TEST("AI doesn't use Froggy Forcefield when it has no Frogs")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_FROGGY_FORCEFIELD, MOVE_SCRATCH); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SCRATCH); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); EXPECT_MOVE(opponent, MOVE_SCRATCH); }
    }
}

AI_SINGLE_BATTLE_TEST("AI doesn't use Froggy Forcefield when it is already active")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_SCRATCH); }
        OPPONENT(SPECIES_FROG_THE_JAM) { Moves(MOVE_FROGGY_FORCEFIELD, MOVE_SCRATCH); }
    } WHEN {
        TURN { MOVE(player, MOVE_SCRATCH); MOVE(opponent, MOVE_FROGGY_FORCEFIELD); }
        TURN { MOVE(player, MOVE_SCRATCH); EXPECT_MOVE(opponent, MOVE_SCRATCH); }
    }
}
