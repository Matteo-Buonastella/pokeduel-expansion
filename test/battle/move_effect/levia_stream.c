#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(GetMoveEffect(MOVE_LEVIA_STREAM) == EFFECT_LEVIA_STREAM);
    ASSUME(GetMoveWeatherType(MOVE_RAIN_DANCE) == BATTLE_WEATHER_RAIN);
}

SINGLE_BATTLE_TEST("Levia Stream fails if it isn't raining")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_LEVIA_STREAM); }
    } SCENE {
        MESSAGE("Wobbuffet used Levia Stream!");
        MESSAGE("But it failed!");
        NONE_OF {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_LEVIA_STREAM, player);
        }
    }
}

SINGLE_BATTLE_TEST("Levia Stream hits in the rain and then stops the rain")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_RAIN_DANCE); }
        TURN { MOVE(player, MOVE_LEVIA_STREAM); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_RAIN_DANCE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_LEVIA_STREAM, player);
        HP_BAR(opponent);
        MESSAGE("The rain stopped.");
    } THEN {
        EXPECT_EQ(gBattleWeather, B_WEATHER_NONE);
    }
}

SINGLE_BATTLE_TEST("Levia Stream fails in the rain while Cloud Nine is suppressing the weather")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_GOLDUCK) { Ability(ABILITY_CLOUD_NINE); }
    } WHEN {
        TURN { MOVE(player, MOVE_RAIN_DANCE); }
        TURN { MOVE(player, MOVE_LEVIA_STREAM); }
    } SCENE {
        MESSAGE("Wobbuffet used Levia Stream!");
        MESSAGE("But it failed!");
    }
}

SINGLE_BATTLE_TEST("Levia Stream doesn't stop the heavy rain from Primordial Sea")
{
    GIVEN {
        PLAYER(SPECIES_KYOGRE_PRIMAL) { Ability(ABILITY_PRIMORDIAL_SEA); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_LEVIA_STREAM); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_LEVIA_STREAM, player);
        HP_BAR(opponent);
        NONE_OF {
            MESSAGE("The rain stopped.");
        }
    } THEN {
        EXPECT(gBattleWeather & B_WEATHER_RAIN_PRIMAL);
    }
}

AI_SINGLE_BATTLE_TEST("AI uses a different move to keep the rain if it also KOs, otherwise Levia Stream")
{
    u16 hp;
    enum Move expectedMove;
    PARAMETRIZE { hp = 1;   expectedMove = MOVE_SCRATCH; }
    PARAMETRIZE { hp = 200; expectedMove = MOVE_LEVIA_STREAM; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_WOBBUFFET) { HP(hp); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_PELIPPER) { Ability(ABILITY_DRIZZLE); Moves(MOVE_LEVIA_STREAM, MOVE_SCRATCH); }
    } WHEN {
        TURN { EXPECT_MOVE(opponent, expectedMove); SEND_OUT(player, 1); }
    } SCENE {
        MESSAGE("Wobbuffet fainted!");
    }
}

AI_SINGLE_BATTLE_TEST("AI uses Rain Dance to enable Levia Stream")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_PELIPPER) { Ability(ABILITY_KEEN_EYE); Moves(MOVE_RAIN_DANCE, MOVE_LEVIA_STREAM, MOVE_SCRATCH); }
    } WHEN {
        TURN { EXPECT_MOVE(opponent, MOVE_RAIN_DANCE); }
        TURN { EXPECT_MOVE(opponent, MOVE_LEVIA_STREAM); }
    }
}
