#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(GetMoveEffect(MOVE_BAD_REACTION_TO_SIMOCHI) == EFFECT_BAD_REACTION_TO_SIMOCHI);
    ASSUME(MoveHasAdditionalEffect(MOVE_ABSORB, MOVE_EFFECT_ABSORB));
}

SINGLE_BATTLE_TEST("Bad Reaction to Simochi makes draining moves hurt the user instead of healing it")
{
    s16 damage;
    s16 hurt;
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); }
    } WHEN {
        TURN { MOVE(player, MOVE_BAD_REACTION_TO_SIMOCHI); }
        TURN { MOVE(player, MOVE_ABSORB); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BAD_REACTION_TO_SIMOCHI, player);
        MESSAGE("Everyone is having a bad reaction to Simochi!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ABSORB, player);
        HP_BAR(opponent, captureDamage: &damage);
        HP_BAR(player, captureDamage: &hurt);
        MESSAGE("Wobbuffet had a bad reaction to Simochi!");
    } THEN {
        EXPECT_MUL_EQ(damage, Q_4_12(0.5), hurt);
    }
}

SINGLE_BATTLE_TEST("Bad Reaction to Simochi affects the user's side too")
{
    s16 damage;
    s16 hurt;
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); }
    } WHEN {
        TURN { MOVE(player, MOVE_BAD_REACTION_TO_SIMOCHI); MOVE(opponent, MOVE_ABSORB); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BAD_REACTION_TO_SIMOCHI, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ABSORB, opponent);
        HP_BAR(player, captureDamage: &damage);
        HP_BAR(opponent, captureDamage: &hurt);
        MESSAGE("The opposing Wobbuffet had a bad reaction to Simochi!");
    } THEN {
        EXPECT_MUL_EQ(damage, Q_4_12(0.5), hurt);
    }
}

SINGLE_BATTLE_TEST("Bad Reaction to Simochi makes Leech Seed hurt the seeder instead of healing it")
{
    s16 drained;
    s16 hurt;
    GIVEN {
        PLAYER(SPECIES_WYNAUT) { Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); }
    } WHEN {
        TURN { MOVE(player, MOVE_BAD_REACTION_TO_SIMOCHI); }
        TURN { MOVE(player, MOVE_LEECH_SEED); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_LEECH_SEED, player);
        HP_BAR(opponent, captureDamage: &drained);
        HP_BAR(player, captureDamage: &hurt);
        MESSAGE("Wynaut had a bad reaction to Simochi!");
    } THEN {
        EXPECT_EQ(drained, hurt);
    }
}

SINGLE_BATTLE_TEST("Bad Reaction to Simochi makes Strength Sap hurt the user instead of healing it")
{
    GIVEN {
        ASSUME(GetMoveEffect(MOVE_STRENGTH_SAP) == EFFECT_STRENGTH_SAP);
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); }
    } WHEN {
        TURN { MOVE(player, MOVE_BAD_REACTION_TO_SIMOCHI); }
        TURN { MOVE(player, MOVE_STRENGTH_SAP); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_STRENGTH_SAP, player);
        HP_BAR(player);
        MESSAGE("Wobbuffet had a bad reaction to Simochi!");
    } THEN {
        EXPECT_LT(player->hp, player->maxHP);
    }
}

SINGLE_BATTLE_TEST("Bad Reaction to Simochi stacks with Liquid Ooze")
{
    s16 damage;
    s16 hurt;
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); }
        OPPONENT(SPECIES_TENTACOOL) { Ability(ABILITY_LIQUID_OOZE); Speed(50); }
    } WHEN {
        TURN { MOVE(player, MOVE_BAD_REACTION_TO_SIMOCHI); }
        TURN { MOVE(player, MOVE_ABSORB); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ABSORB, player);
        HP_BAR(opponent, captureDamage: &damage);
        ABILITY_POPUP(opponent, ABILITY_LIQUID_OOZE);
        HP_BAR(player, captureDamage: &hurt);
        MESSAGE("Wobbuffet sucked up the liquid ooze!");
    } THEN {
        // Each effect reverses half the damage dealt, so together the user loses the full amount
        EXPECT_EQ(damage, hurt);
    }
}

SINGLE_BATTLE_TEST("Bad Reaction to Simochi doesn't hurt a user with Magic Guard")
{
    GIVEN {
        PLAYER(SPECIES_CLEFABLE) { Ability(ABILITY_MAGIC_GUARD); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); }
    } WHEN {
        TURN { MOVE(player, MOVE_BAD_REACTION_TO_SIMOCHI); }
        TURN { MOVE(player, MOVE_ABSORB); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ABSORB, player);
        HP_BAR(opponent);
        NONE_OF {
            HP_BAR(player);
            MESSAGE("Clefable had a bad reaction to Simochi!");
        }
    } THEN {
        EXPECT_EQ(player->hp, player->maxHP);
    }
}

SINGLE_BATTLE_TEST("Bad Reaction to Simochi lasts for 5 turns")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_BAD_REACTION_TO_SIMOCHI); }
        TURN { }
        TURN { }
        TURN { }
        TURN { }
    } SCENE {
        MESSAGE("Everyone is having a bad reaction to Simochi!");
        MESSAGE("The bad reaction to Simochi wore off!");
    }
}

SINGLE_BATTLE_TEST("Bad Reaction to Simochi fails if it's already active")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_BAD_REACTION_TO_SIMOCHI); }
        TURN { MOVE(player, MOVE_BAD_REACTION_TO_SIMOCHI); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BAD_REACTION_TO_SIMOCHI, player);
        MESSAGE("Everyone is having a bad reaction to Simochi!");
        MESSAGE("But it failed!");
    }
}
