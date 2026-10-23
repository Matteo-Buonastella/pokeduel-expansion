#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(GetMoveEffect(MOVE_CALL_OF_THE_HAUNTED) == EFFECT_CALL_OF_THE_HAUNTED);
    ASSUME(GetSpeciesType(SPECIES_GASTLY, 0) == TYPE_GHOST);
    ASSUME(GetSpeciesType(SPECIES_WYNAUT, 0) != TYPE_GHOST && GetSpeciesType(SPECIES_WYNAUT, 1) != TYPE_GHOST);
}

SINGLE_BATTLE_TEST("Call of the Haunted switches the user out and makes the switch-in a pure Ghost type")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_CALL_OF_THE_HAUNTED); SEND_OUT(player, 1); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CALL_OF_THE_HAUNTED, player);
        SEND_IN_MESSAGE("Wynaut");
        MESSAGE("Wynaut transformed into the Ghost type!");
    } THEN {
        EXPECT_EQ(player->species, SPECIES_WYNAUT);
        EXPECT_EQ(player->types[0], TYPE_GHOST);
        EXPECT_EQ(player->types[1], TYPE_GHOST);
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Call of the Haunted raises the Defense of a switch-in that is already part Ghost")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_GASTLY);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_CALL_OF_THE_HAUNTED); SEND_OUT(player, 1); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CALL_OF_THE_HAUNTED, player);
        SEND_IN_MESSAGE("Gastly");
        NOT MESSAGE("Gastly transformed into the Ghost type!");
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
        MESSAGE("Gastly's Defense rose!");
    } THEN {
        EXPECT_EQ(player->species, SPECIES_GASTLY);
        EXPECT_EQ(player->types[0], GetSpeciesType(SPECIES_GASTLY, 0));
        EXPECT_EQ(player->types[1], GetSpeciesType(SPECIES_GASTLY, 1));
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Call of the Haunted doesn't pass the user's stat changes")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_SWORDS_DANCE); }
        TURN { MOVE(player, MOVE_CALL_OF_THE_HAUNTED); SEND_OUT(player, 1); }
    } THEN {
        EXPECT_EQ(player->species, SPECIES_WYNAUT);
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Call of the Haunted fails if the user has no party member to switch to")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_CALL_OF_THE_HAUNTED); }
    } SCENE {
        NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_CALL_OF_THE_HAUNTED, player);
        MESSAGE("But it failed!");
    } THEN {
        EXPECT_EQ(player->species, SPECIES_WOBBUFFET);
        EXPECT_EQ(player->types[0], GetSpeciesType(SPECIES_WOBBUFFET, 0));
    }
}

AI_SINGLE_BATTLE_TEST("AI doesn't use Call of the Haunted if it has no party member to switch to")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CALL_OF_THE_HAUNTED, MOVE_SCRATCH); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); EXPECT_MOVE(opponent, MOVE_SCRATCH); }
    }
}
