#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(GetMoveEffect(MOVE_FLUTE_OF_D) == EFFECT_FLUTE_OF_D);
    ASSUME(GetSpeciesType(SPECIES_DRATINI, 0) == TYPE_DRAGON);
}

SINGLE_BATTLE_TEST("Flute of D lowers the target's Defense and switches the user out")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_FLUTE_OF_D); SEND_OUT(player, 1); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FLUTE_OF_D, player);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, opponent);
        MESSAGE("The opposing Wobbuffet's Defense fell!");
        SEND_IN_MESSAGE("Wynaut");
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(player->species, SPECIES_WYNAUT);
    }
}

SINGLE_BATTLE_TEST("Flute of D raises the Attack and Sp. Atk of a Dragon type that switches in")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_DRATINI);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_FLUTE_OF_D); SEND_OUT(player, 1); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FLUTE_OF_D, player);
        SEND_IN_MESSAGE("Dratini");
        MESSAGE("The Flute of D empowered Dratini!");
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
    } THEN {
        EXPECT_EQ(player->species, SPECIES_DRATINI);
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Flute of D doesn't boost a switch-in that isn't a Dragon type")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_FLUTE_OF_D); SEND_OUT(player, 1); }
    } SCENE {
        SEND_IN_MESSAGE("Wynaut");
        NOT MESSAGE("The Flute of D empowered Wynaut!");
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Flute of D's boost doesn't carry over to a later switch-in")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WYNAUT);
        PLAYER(SPECIES_DRATINI);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_FLUTE_OF_D); SEND_OUT(player, 1); }
        TURN { SWITCH(player, 2); }
    } SCENE {
        SEND_IN_MESSAGE("Wynaut");
        NOT MESSAGE("The Flute of D empowered Dratini!");
    } THEN {
        EXPECT_EQ(player->species, SPECIES_DRATINI);
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Flute of D doesn't switch the user out if the Defense drop fails (Gen7+)")
{
    GIVEN {
        WITH_CONFIG(B_PARTING_SHOT_SWITCH, GEN_7);
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_DRATINI);
        OPPONENT(SPECIES_BELDUM) { Ability(ABILITY_CLEAR_BODY); }
    } WHEN {
        TURN { MOVE(player, MOVE_FLUTE_OF_D); }
    } SCENE {
        ABILITY_POPUP(opponent, ABILITY_CLEAR_BODY);
        NOT SEND_IN_MESSAGE("Dratini");
    } THEN {
        EXPECT_EQ(player->species, SPECIES_WOBBUFFET);
    }
}

SINGLE_BATTLE_TEST("Flute of D still lowers the target's Defense if the user has no party member to switch to")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_FLUTE_OF_D); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FLUTE_OF_D, player);
        MESSAGE("The opposing Wobbuffet's Defense fell!");
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(player->species, SPECIES_WOBBUFFET);
    }
}

AI_SINGLE_BATTLE_TEST("AI still uses Flute of D for the Defense drop when it has no party member to switch to")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_FLUTE_OF_D, MOVE_SPLASH); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); EXPECT_MOVE(opponent, MOVE_FLUTE_OF_D); }
    }
}

AI_SINGLE_BATTLE_TEST("AI doesn't use Flute of D if the target's Defense can't be lowered (Gen7+)")
{
    GIVEN {
        WITH_CONFIG(B_PARTING_SHOT_SWITCH, GEN_7);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_BELDUM) { Ability(ABILITY_CLEAR_BODY); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_FLUTE_OF_D, MOVE_SCRATCH); }
        OPPONENT(SPECIES_DRATINI) { Moves(MOVE_SCRATCH); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); EXPECT_MOVE(opponent, MOVE_SCRATCH); }
    }
}
