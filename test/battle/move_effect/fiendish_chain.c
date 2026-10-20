#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(GetMoveEffect(MOVE_FIENDISH_CHAIN) == EFFECT_FIENDISH_CHAIN);
}

SINGLE_BATTLE_TEST("Fiendish Chain suppresses the target's Ability and prevents it from escaping")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_FIENDISH_CHAIN); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FIENDISH_CHAIN, player);
        MESSAGE("The opposing Wobbuffet's Ability was suppressed!");
        MESSAGE("The opposing Wobbuffet can no longer escape!");
    } THEN {
        EXPECT(opponent->volatiles.gastroAcid);
        EXPECT(opponent->volatiles.escapePrevention);
    }
}

SINGLE_BATTLE_TEST("Fiendish Chain still traps the target if its Ability can't be suppressed")
{
    GIVEN {
        ASSUME(gAbilitiesInfo[ABILITY_MULTITYPE].cantBeSuppressed);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_ARCEUS) { Ability(ABILITY_MULTITYPE); }
    } WHEN {
        TURN { MOVE(player, MOVE_FIENDISH_CHAIN); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FIENDISH_CHAIN, player);
        NOT MESSAGE("The opposing Arceus's Ability was suppressed!");
        MESSAGE("The opposing Arceus can no longer escape!");
    } THEN {
        EXPECT(!opponent->volatiles.gastroAcid);
        EXPECT(opponent->volatiles.escapePrevention);
    }
}

SINGLE_BATTLE_TEST("Fiendish Chain still suppresses the Ability of a Ghost type that can't be trapped")
{
    GIVEN {
        WITH_CONFIG(B_GHOSTS_ESCAPE, GEN_6);
        ASSUME(GetSpeciesType(SPECIES_GENGAR, 0) == TYPE_GHOST);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_GENGAR) { Ability(ABILITY_CURSED_BODY); }
    } WHEN {
        TURN { MOVE(player, MOVE_FIENDISH_CHAIN); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FIENDISH_CHAIN, player);
        MESSAGE("The opposing Gengar's Ability was suppressed!");
        NOT MESSAGE("The opposing Gengar can no longer escape!");
    } THEN {
        EXPECT(opponent->volatiles.gastroAcid);
        EXPECT(!opponent->volatiles.escapePrevention);
    }
}

SINGLE_BATTLE_TEST("Fiendish Chain fails if the target's Ability can't be suppressed and it can't be trapped")
{
    GIVEN {
        ASSUME(gAbilitiesInfo[ABILITY_MULTITYPE].cantBeSuppressed);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_ARCEUS) { Ability(ABILITY_MULTITYPE); }
    } WHEN {
        TURN { MOVE(player, MOVE_FIENDISH_CHAIN); }
        TURN { MOVE(player, MOVE_FIENDISH_CHAIN); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FIENDISH_CHAIN, player);
        MESSAGE("But it failed!");
    }
}

SINGLE_BATTLE_TEST("Fiendish Chain fails if the target is already suppressed and trapped")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_FIENDISH_CHAIN); }
        TURN { MOVE(player, MOVE_FIENDISH_CHAIN); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FIENDISH_CHAIN, player);
        MESSAGE("But it failed!");
    }
}

SINGLE_BATTLE_TEST("Fiendish Chain is blocked by Ability Shield only for the suppression")
{
    GIVEN {
        ASSUME(GetItemHoldEffect(ITEM_ABILITY_SHIELD) == HOLD_EFFECT_ABILITY_SHIELD);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Item(ITEM_ABILITY_SHIELD); }
    } WHEN {
        TURN { MOVE(player, MOVE_FIENDISH_CHAIN); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FIENDISH_CHAIN, player);
        NOT MESSAGE("The opposing Wobbuffet's Ability was suppressed!");
        MESSAGE("The opposing Wobbuffet can no longer escape!");
    } THEN {
        EXPECT(!opponent->volatiles.gastroAcid);
        EXPECT(opponent->volatiles.escapePrevention);
    }
}

AI_SINGLE_BATTLE_TEST("AI doesn't use Fiendish Chain if the target is already suppressed and trapped")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_FIENDISH_CHAIN, MOVE_SCRATCH); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_FIENDISH_CHAIN); }
        TURN { MOVE(player, MOVE_CELEBRATE); EXPECT_MOVE(opponent, MOVE_SCRATCH); }
    }
}
