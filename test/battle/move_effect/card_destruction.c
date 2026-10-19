#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(GetMoveEffect(MOVE_CARD_DESTRUCTION) == EFFECT_CARD_DESTRUCTION);
}

SINGLE_BATTLE_TEST("Card Destruction switches out both the user and the target")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_BULBASAUR);
    } WHEN {
        TURN { MOVE(player, MOVE_CARD_DESTRUCTION); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CARD_DESTRUCTION, player);
        MESSAGE("Wynaut was dragged out!");
        MESSAGE("The opposing Bulbasaur was dragged out!");
    } THEN {
        EXPECT_EQ(player->species, SPECIES_WYNAUT);
        EXPECT_EQ(opponent->species, SPECIES_BULBASAUR);
    }
}

SINGLE_BATTLE_TEST("Card Destruction replaces each battler with a random non-fainted party member")
{
    PASSES_RANDOMLY(1, 2, RNG_CARD_DESTRUCTION);
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_BULBASAUR);
        OPPONENT(SPECIES_CHARMANDER);
        OPPONENT(SPECIES_SQUIRTLE) { HP(0); }
    } WHEN {
        TURN { MOVE(player, MOVE_CARD_DESTRUCTION); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CARD_DESTRUCTION, player);
        MESSAGE("The opposing Bulbasaur was dragged out!");
    }
}

SINGLE_BATTLE_TEST("Card Destruction only switches out battlers that have a party member to switch to")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_BULBASAUR);
    } WHEN {
        TURN { MOVE(player, MOVE_CARD_DESTRUCTION); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CARD_DESTRUCTION, player);
        NOT MESSAGE("Wobbuffet was dragged out!");
        MESSAGE("The opposing Bulbasaur was dragged out!");
    } THEN {
        EXPECT_EQ(player->species, SPECIES_WOBBUFFET);
        EXPECT_EQ(opponent->species, SPECIES_BULBASAUR);
    }
}

SINGLE_BATTLE_TEST("Card Destruction resets the stat stages of switched out battlers")
{
    GIVEN {
        ASSUME(GetMoveEffect(MOVE_SWORDS_DANCE) == EFFECT_STAT_CHANGE);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_SWORDS_DANCE); MOVE(player, MOVE_CARD_DESTRUCTION); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SWORDS_DANCE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CARD_DESTRUCTION, player);
        MESSAGE("The opposing Wobbuffet was dragged out!");
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Card Destruction fails if no battler has a party member to switch to")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WYNAUT) { HP(0); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_CARD_DESTRUCTION); }
    } SCENE {
        NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_CARD_DESTRUCTION, player);
        MESSAGE("But it failed!");
    }
}

WILD_BATTLE_TEST("Card Destruction fails in wild battles")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_CARD_DESTRUCTION); }
    } SCENE {
        NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_CARD_DESTRUCTION, player);
        MESSAGE("But it failed!");
    }
}

DOUBLE_BATTLE_TEST("Card Destruction can send back in a monster its partner was just switched out for")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WYNAUT);
        PLAYER(SPECIES_BULBASAUR);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WYNAUT);
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_CARD_DESTRUCTION); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CARD_DESTRUCTION, playerLeft);
        MESSAGE("Bulbasaur was dragged out!");
        MESSAGE("Wobbuffet was dragged out!");
    } THEN {
        EXPECT_EQ(playerLeft->species, SPECIES_BULBASAUR);
        EXPECT_EQ(playerRight->species, SPECIES_WOBBUFFET);
    }
}

AI_SINGLE_BATTLE_TEST("AI uses Card Destruction when the player has boosted several stats")
{
    GIVEN {
        ASSUME(GetMoveEffect(MOVE_COIL) == EFFECT_STAT_CHANGE);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_COIL, MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CARD_DESTRUCTION, MOVE_SPLASH); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SPLASH); }
    } WHEN {
        TURN { MOVE(player, MOVE_COIL); MOVE(opponent, MOVE_SPLASH); }
        TURN { MOVE(player, MOVE_CELEBRATE); EXPECT_MOVE(opponent, MOVE_CARD_DESTRUCTION); }
    }
}

AI_SINGLE_BATTLE_TEST("AI doesn't use Card Destruction for a small advantage")
{
    GIVEN {
        ASSUME(GetMoveEffect(MOVE_SWORDS_DANCE) == EFFECT_STAT_CHANGE);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_SWORDS_DANCE, MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CARD_DESTRUCTION, MOVE_TACKLE, MOVE_SPLASH); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SPLASH); }
    } WHEN {
        TURN { MOVE(player, MOVE_SWORDS_DANCE); MOVE(opponent, MOVE_SPLASH); }
        TURN { MOVE(player, MOVE_CELEBRATE); EXPECT_MOVE(opponent, MOVE_TACKLE); }
    }
}

AI_SINGLE_BATTLE_TEST("AI doesn't use Card Destruction when it can KO the target")
{
    GIVEN {
        ASSUME(GetMoveEffect(MOVE_COIL) == EFFECT_STAT_CHANGE);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_WOBBUFFET) { HP(1); Moves(MOVE_COIL, MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CARD_DESTRUCTION, MOVE_TACKLE, MOVE_SPLASH); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SPLASH); }
    } WHEN {
        TURN { MOVE(player, MOVE_COIL); MOVE(opponent, MOVE_SPLASH); }
        TURN { MOVE(player, MOVE_CELEBRATE); EXPECT_MOVE(opponent, MOVE_TACKLE); }
    }
}

AI_SINGLE_BATTLE_TEST("AI doesn't use Card Destruction when it would lose its own stat boosts")
{
    GIVEN {
        ASSUME(GetMoveEffect(MOVE_SWORDS_DANCE) == EFFECT_STAT_CHANGE);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CARD_DESTRUCTION, MOVE_SWORDS_DANCE, MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SPLASH); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_SWORDS_DANCE); }
        TURN { MOVE(player, MOVE_CELEBRATE); NOT_EXPECT_MOVE(opponent, MOVE_CARD_DESTRUCTION); }
    }
}
