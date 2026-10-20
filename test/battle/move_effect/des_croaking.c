#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(GetMoveEffect(MOVE_DES_CROAKING) == EFFECT_DES_CROAKING);
    ASSUME(gSpeciesInfo[SPECIES_FROG_THE_JAM].isFrog);
    ASSUME(!gSpeciesInfo[SPECIES_WOBBUFFET].isFrog);
}

SINGLE_BATTLE_TEST("Des-Croaking boosts the damage of the user's Frogs by 33%", s16 damage)
{
    enum Move setupMove;

    PARAMETRIZE { setupMove = MOVE_CELEBRATE; }
    PARAMETRIZE { setupMove = MOVE_DES_CROAKING; }

    GIVEN {
        PLAYER(SPECIES_FROG_THE_JAM);
        OPPONENT(SPECIES_WOBBUFFET) { MaxHP(500); HP(500); }
    } WHEN {
        TURN { MOVE(player, setupMove); }
        TURN { MOVE(player, MOVE_SCRATCH); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(1.33), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Des-Croaking boosts Frogs that switch in after it was used", s16 damage)
{
    enum Move setupMove;

    PARAMETRIZE { setupMove = MOVE_CELEBRATE; }
    PARAMETRIZE { setupMove = MOVE_DES_CROAKING; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_FROG_THE_JAM);
        OPPONENT(SPECIES_WOBBUFFET) { MaxHP(500); HP(500); }
    } WHEN {
        TURN { MOVE(player, setupMove); }
        TURN { SWITCH(player, 1); }
        TURN { MOVE(player, MOVE_SCRATCH); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(1.33), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Des-Croaking doesn't boost monsters that aren't Frogs", s16 damage)
{
    enum Move setupMove;

    PARAMETRIZE { setupMove = MOVE_CELEBRATE; }
    PARAMETRIZE { setupMove = MOVE_DES_CROAKING; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { MaxHP(500); HP(500); }
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

SINGLE_BATTLE_TEST("Des-Croaking doesn't boost the opposing side's Frogs", s16 damage)
{
    enum Move setupMove;

    PARAMETRIZE { setupMove = MOVE_CELEBRATE; }
    PARAMETRIZE { setupMove = MOVE_DES_CROAKING; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { MaxHP(500); HP(500); Speed(1); }
        OPPONENT(SPECIES_FROG_THE_JAM) { Speed(2); }
    } WHEN {
        TURN { MOVE(player, setupMove); }
        TURN { MOVE(opponent, MOVE_SCRATCH); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, opponent);
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Des-Croaking lasts for 5 turns")
{
    GIVEN {
        PLAYER(SPECIES_FROG_THE_JAM);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_DES_CROAKING); }
        TURN {}
        TURN {}
        TURN {}
        TURN {}
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DES_CROAKING, player);
        MESSAGE("The croaking powered up the Frogs on your team!");
        MESSAGE("Your side's Des-Croaking wore off!");
    } THEN {
        EXPECT(!(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_DES_CROAKING));
    }
}

SINGLE_BATTLE_TEST("Des-Croaking lasts for 8 turns if the user is holding Light Clay")
{
    GIVEN {
        ASSUME(GetItemHoldEffect(ITEM_LIGHT_CLAY) == HOLD_EFFECT_LIGHT_CLAY);
        PLAYER(SPECIES_FROG_THE_JAM) { Item(ITEM_LIGHT_CLAY); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_DES_CROAKING); }
        TURN {}
        TURN {}
        TURN {}
        TURN {}
        TURN {}
        TURN {}
        TURN {}
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DES_CROAKING, player);
        MESSAGE("Your side's Des-Croaking wore off!");
    } THEN {
        EXPECT(!(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_DES_CROAKING));
    }
}

SINGLE_BATTLE_TEST("Des-Croaking is still active after 5 turns if the user is holding Light Clay")
{
    GIVEN {
        PLAYER(SPECIES_FROG_THE_JAM) { Item(ITEM_LIGHT_CLAY); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_DES_CROAKING); }
        TURN {}
        TURN {}
        TURN {}
        TURN {}
    } SCENE {
        NOT MESSAGE("Your side's Des-Croaking wore off!");
    } THEN {
        EXPECT(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_DES_CROAKING);
    }
}

SINGLE_BATTLE_TEST("Des-Croaking fails if it is already active on the user's side")
{
    GIVEN {
        PLAYER(SPECIES_FROG_THE_JAM);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_DES_CROAKING); }
        TURN { MOVE(player, MOVE_DES_CROAKING); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DES_CROAKING, player);
        MESSAGE("But it failed!");
    }
}

AI_SINGLE_BATTLE_TEST("AI uses Des-Croaking when it has Frogs in its party")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_DES_CROAKING, MOVE_SPLASH); }
        OPPONENT(SPECIES_FROG_THE_JAM) { Moves(MOVE_SCRATCH); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); EXPECT_MOVE(opponent, MOVE_DES_CROAKING); }
    }
}

AI_SINGLE_BATTLE_TEST("AI doesn't use Des-Croaking when it has no Frogs")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_DES_CROAKING, MOVE_SCRATCH); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SCRATCH); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); EXPECT_MOVE(opponent, MOVE_SCRATCH); }
    }
}

AI_SINGLE_BATTLE_TEST("AI doesn't use Des-Croaking when it is already active")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_FROG_THE_JAM) { Moves(MOVE_DES_CROAKING, MOVE_SCRATCH); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_DES_CROAKING); }
        TURN { MOVE(player, MOVE_CELEBRATE); EXPECT_MOVE(opponent, MOVE_SCRATCH); }
    }
}
