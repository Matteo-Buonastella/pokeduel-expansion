#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(GetMoveEffect(MOVE_WRATH_OF_NEOS) == EFFECT_WRATH_OF_NEOS);
}

SINGLE_BATTLE_TEST("Wrath of Neos switches the user out after hitting")
{
    GIVEN {
        PLAYER(SPECIES_NEOS);
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_WRATH_OF_NEOS); SEND_OUT(player, 1); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WRATH_OF_NEOS, player);
        HP_BAR(opponent);
        SEND_IN_MESSAGE("Wynaut");
    }
}

SINGLE_BATTLE_TEST("Wrath of Neos doesn't switch the user out if there are no replacements")
{
    GIVEN {
        PLAYER(SPECIES_NEOS);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_WRATH_OF_NEOS); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WRATH_OF_NEOS, player);
        HP_BAR(opponent);
    } THEN {
        EXPECT_EQ(player->species, SPECIES_NEOS);
    }
}

SINGLE_BATTLE_TEST("Wrath of Neos fails if the user isn't Neos")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_WRATH_OF_NEOS); }
    } SCENE {
        NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_WRATH_OF_NEOS, player);
        MESSAGE("But Wobbuffet can't use the move!");
    } THEN {
        EXPECT_EQ(player->species, SPECIES_WOBBUFFET);
    }
}

// Each pair only differs in which of the target's defenses is low, so the move hits harder
// against the low one only if it uses the matching attacking stat
SINGLE_BATTLE_TEST("Wrath of Neos is physical if the user's Attack is higher than its Sp. Atk", s16 damage)
{
    u32 def, spDef;

    PARAMETRIZE { def = 100; spDef = 300; }
    PARAMETRIZE { def = 300; spDef = 100; }

    GIVEN {
        PLAYER(SPECIES_NEOS) { Attack(300); SpAttack(100); }
        OPPONENT(SPECIES_WOBBUFFET) { MaxHP(999); HP(999); Defense(def); SpDefense(spDef); }
    } WHEN {
        TURN { MOVE(player, MOVE_WRATH_OF_NEOS); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WRATH_OF_NEOS, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_GT(results[0].damage, results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Wrath of Neos is special if the user's Sp. Atk is higher than its Attack", s16 damage)
{
    u32 def, spDef;

    PARAMETRIZE { def = 100; spDef = 300; }
    PARAMETRIZE { def = 300; spDef = 100; }

    GIVEN {
        PLAYER(SPECIES_NEOS) { Attack(100); SpAttack(300); }
        OPPONENT(SPECIES_WOBBUFFET) { MaxHP(999); HP(999); Defense(def); SpDefense(spDef); }
    } WHEN {
        TURN { MOVE(player, MOVE_WRATH_OF_NEOS); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WRATH_OF_NEOS, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_LT(results[0].damage, results[1].damage);
    }
}

AI_SINGLE_BATTLE_TEST("AI doesn't use Wrath of Neos if it isn't Neos")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_WRATH_OF_NEOS, MOVE_SCRATCH); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SCRATCH); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); EXPECT_MOVE(opponent, MOVE_SCRATCH); }
    }
}
