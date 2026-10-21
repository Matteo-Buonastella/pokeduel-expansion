#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(GetMoveEffect(MOVE_WINDSTORM) == EFFECT_WINDSTORM);
}

SINGLE_BATTLE_TEST("Windstorm drags out a random party member and lowers its Defense")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_BULBASAUR);
    } WHEN {
        TURN { MOVE(player, MOVE_WINDSTORM); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WINDSTORM, player);
        MESSAGE("The opposing Bulbasaur was dragged out!");
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, opponent);
        MESSAGE("The opposing Bulbasaur's Defense fell!");
    } THEN {
        EXPECT_EQ(opponent->species, SPECIES_BULBASAUR);
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Windstorm fails if the target has no party member to switch to")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_WINDSTORM); }
    } SCENE {
        MESSAGE("But it failed!");
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Windstorm's Defense drop is blocked by Clear Body on the dragged out monster")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_BELDUM) { Ability(ABILITY_CLEAR_BODY); }
    } WHEN {
        TURN { MOVE(player, MOVE_WINDSTORM); }
    } SCENE {
        MESSAGE("The opposing Beldum was dragged out!");
        ABILITY_POPUP(opponent, ABILITY_CLEAR_BODY);
        NOT ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, opponent);
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Windstorm doesn't lower the Defense of a monster that faints to hazards on entry")
{
    GIVEN {
        ASSUME(GetMoveEffect(MOVE_STEALTH_ROCK) == EFFECT_STEALTH_ROCK);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_CHARIZARD) { HP(1); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_STEALTH_ROCK); }
        TURN { MOVE(player, MOVE_WINDSTORM, WITH_RNG(RNG_FORCE_RANDOM_SWITCH, 0)); SEND_OUT(opponent, 2); }
    } SCENE {
        MESSAGE("The opposing Charizard was dragged out!");
        NOT MESSAGE("The opposing Charizard's Defense fell!");
    }
}
