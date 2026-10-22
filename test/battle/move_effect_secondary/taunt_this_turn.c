#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(MoveHasAdditionalEffect(MOVE_COLD_WAVE, MOVE_EFFECT_TAUNT_THIS_TURN) == TRUE);
    ASSUME(IsBattleMoveStatus(MOVE_SWORDS_DANCE));
}

SINGLE_BATTLE_TEST("Cold Wave taunts the target for the rest of the turn")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); }
    } WHEN {
        TURN { MOVE(player, MOVE_COLD_WAVE); MOVE(opponent, MOVE_SWORDS_DANCE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_COLD_WAVE, player);
        HP_BAR(opponent);
        MESSAGE("The opposing Wobbuffet fell for the taunt!");
        MESSAGE("The opposing Wobbuffet can't use Swords Dance after the taunt!");
        NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_SWORDS_DANCE, opponent);
    }
}

SINGLE_BATTLE_TEST("Cold Wave's taunt wears off at the end of the turn")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); }
    } WHEN {
        TURN { MOVE(player, MOVE_COLD_WAVE); MOVE(opponent, MOVE_SWORDS_DANCE); }
        TURN { MOVE(opponent, MOVE_SWORDS_DANCE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_COLD_WAVE, player);
        NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_SWORDS_DANCE, opponent);
        MESSAGE("The opposing Wobbuffet's Taunt wore off!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SWORDS_DANCE, opponent);
    }
}

SINGLE_BATTLE_TEST("Cold Wave doesn't prevent damaging moves")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); }
    } WHEN {
        TURN { MOVE(player, MOVE_COLD_WAVE); MOVE(opponent, MOVE_SCRATCH); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_COLD_WAVE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, opponent);
    }
}

SINGLE_BATTLE_TEST("Cold Wave doesn't taunt Pokémon with Oblivious")
{
    GIVEN {
        ASSUME(GetConfig(B_OBLIVIOUS_TAUNT) >= GEN_6);
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); }
        OPPONENT(SPECIES_SLOWPOKE) { Ability(ABILITY_OBLIVIOUS); Speed(50); }
    } WHEN {
        TURN { MOVE(player, MOVE_COLD_WAVE); MOVE(opponent, MOVE_SWORDS_DANCE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_COLD_WAVE, player);
        NOT MESSAGE("The opposing Slowpoke fell for the taunt!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SWORDS_DANCE, opponent);
    }
}

SINGLE_BATTLE_TEST("Cold Wave doesn't taunt Pokémon protected by Aroma Veil")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); }
        OPPONENT(SPECIES_AROMATISSE) { Ability(ABILITY_AROMA_VEIL); Speed(50); }
    } WHEN {
        TURN { MOVE(player, MOVE_COLD_WAVE); MOVE(opponent, MOVE_SWORDS_DANCE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_COLD_WAVE, player);
        NOT MESSAGE("The opposing Aromatisse fell for the taunt!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SWORDS_DANCE, opponent);
    }
}
