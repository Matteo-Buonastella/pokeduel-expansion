#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(GetMoveEffect(MOVE_NAP_TIME) == EFFECT_NAP_TIME);
}

SINGLE_BATTLE_TEST("Nap Time fails if target is immune to sleep")
{
    GIVEN {
        PLAYER(SPECIES_MOKEY_KING);
        OPPONENT(SPECIES_ZUBAT) { Ability(ABILITY_INSOMNIA); }
    } WHEN {
        TURN { MOVE(player, MOVE_NAP_TIME);}
    } SCENE {
        ABILITY_POPUP(opponent, ABILITY_INSOMNIA);
        MESSAGE("The opposing Zubat made it ineffective!");
        NONE_OF {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_NAP_TIME, player);
            ANIMATION(ANIM_TYPE_STATUS, B_ANIM_STATUS_SLP, player);
            ANIMATION(ANIM_TYPE_STATUS, B_ANIM_STATUS_SLP, opponent);
            STATUS_ICON(player, sleep: TRUE);
            STATUS_ICON(opponent, sleep: TRUE);
        }
    }
}

SINGLE_BATTLE_TEST("Nap Time fails if the user is already asleep")
{
    GIVEN {
        ASSUME(GetMoveEffect(MOVE_SLEEP_TALK) == EFFECT_SLEEP_TALK);
        PLAYER(SPECIES_MOKEY_KING) { Status1(STATUS1_SLEEP); Moves(MOVE_SLEEP_TALK, MOVE_NAP_TIME); }
        OPPONENT(SPECIES_ZUBAT) { Ability(ABILITY_MOXIE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SLEEP_TALK);}
    } SCENE {
        MESSAGE("Mokey King used Sleep Talk!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SLEEP_TALK, player);
        MESSAGE("Mokey King used Nap Time!");
        MESSAGE("Mokey King is already asleep!");
        NONE_OF {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_NAP_TIME, player);
            ANIMATION(ANIM_TYPE_STATUS, B_ANIM_STATUS_SLP, player);
            ANIMATION(ANIM_TYPE_STATUS, B_ANIM_STATUS_SLP, opponent);
            STATUS_ICON(opponent, sleep: TRUE);
        }
    }
}

SINGLE_BATTLE_TEST("Nap Time puts the user and target to sleep")
{
    GIVEN {
        PLAYER(SPECIES_MOKEY_KING) HP(10); 
        OPPONENT(SPECIES_ZUBAT) { Ability(ABILITY_MOXIE); }
    } WHEN {
        TURN { MOVE(player, MOVE_NAP_TIME);}
    } SCENE {
        MESSAGE("Mokey King used Nap Time!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_NAP_TIME, player);
        MESSAGE("The opposing Zubat fell asleep!");
        MESSAGE("Mokey King slept and restored its HP!");
        MESSAGE("Mokey King's HP was restored.");
    }
}

SINGLE_BATTLE_TEST("Nap Time fails in Electric Terrain")
{
    GIVEN {
        PLAYER(SPECIES_MOKEY_KING); 
        OPPONENT(SPECIES_ZUBAT) { Ability(ABILITY_MOXIE); }
    } WHEN {
        TURN { MOVE(player, MOVE_NAP_TIME);}
    } SCENE {
        MESSAGE("Mokey King used Nap Time!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_NAP_TIME, player);
        MESSAGE("The opposing Zubat fell asleep!");
        MESSAGE("Mokey King slept and restored its HP!");
        MESSAGE("Mokey King's HP was restored.");
    }
}

SINGLE_BATTLE_TEST("Nap Time fails if grounded in Electric/Misty Terrain")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_ELECTRIC_TERRAIN; }
    PARAMETRIZE { move = MOVE_MISTY_TERRAIN; }
    GIVEN {
        ASSUME(GetMoveEffect(move) == EFFECT_TERRAIN);

        ASSUME(GetMoveTerrainType(MOVE_ELECTRIC_TERRAIN) == B_TERRAIN_ELECTRIC);
        ASSUME(GetMoveTerrainType(MOVE_MISTY_TERRAIN) == B_TERRAIN_MISTY);

        PLAYER(SPECIES_PETIT_MOTH);
        OPPONENT(SPECIES_LATIOS);
    } WHEN {
        TURN { MOVE(opponent, move); MOVE(player, MOVE_NAP_TIME); }
    } SCENE {
        NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_NAP_TIME, player);
    } THEN {
        EXPECT(!(player->status1 & STATUS1_SLEEP));
        EXPECT(!(opponent->status1 & STATUS1_SLEEP));
    }
}