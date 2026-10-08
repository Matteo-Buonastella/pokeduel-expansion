#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(MoveHasAdditionalEffect(MOVE_THUNDER_FORCE, MOVE_EFFECT_PARALYSIS));
    ASSUME(MoveAlwaysHitsInRain(MOVE_THUNDER_FORCE));
    ASSUME(GetMoveAccuracy(MOVE_THUNDER_FORCE) < 100);
}

SINGLE_BATTLE_TEST("Thunder Force has a 10% chance to paralyze the target")
{
    PASSES_RANDOMLY(10, 100, RNG_SECONDARY_EFFECT);
    GIVEN {
        PLAYER(SPECIES_SLIFER_THE_SKY_DRAGON);
        OPPONENT(SPECIES_SWAP_FROG) { Ability(ABILITY_REGENERATOR); MaxHP(999); HP(999); }
    } WHEN {
        TURN { MOVE(player, MOVE_THUNDER_FORCE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_THUNDER_FORCE, player);
        HP_BAR(opponent);
        ANIMATION(ANIM_TYPE_STATUS, B_ANIM_STATUS_PRZ, opponent);
        STATUS_ICON(opponent, paralysis: TRUE);
    }
}

SINGLE_BATTLE_TEST("Thunder Force has a 20% chance to paralyze the target in rain")
{
    PASSES_RANDOMLY(20, 100, RNG_SECONDARY_EFFECT);
    GIVEN {
        PLAYER(SPECIES_SLIFER_THE_SKY_DRAGON);
        OPPONENT(SPECIES_SWAP_FROG) { Ability(ABILITY_DRIZZLE); MaxHP(999); HP(999); }
    } WHEN {
        TURN { MOVE(player, MOVE_THUNDER_FORCE); }
    } SCENE {
        ABILITY_POPUP(opponent, ABILITY_DRIZZLE);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_THUNDER_FORCE, player);
        HP_BAR(opponent);
        ANIMATION(ANIM_TYPE_STATUS, B_ANIM_STATUS_PRZ, opponent);
        STATUS_ICON(opponent, paralysis: TRUE);
    }
}

SINGLE_BATTLE_TEST("Thunder Force can miss outside of rain")
{
    GIVEN {
        PLAYER(SPECIES_SLIFER_THE_SKY_DRAGON);
        OPPONENT(SPECIES_SWAP_FROG) { Ability(ABILITY_REGENERATOR); MaxHP(999); HP(999); }
    } WHEN {
        TURN { MOVE(player, MOVE_THUNDER_FORCE, hit: FALSE); }
    } SCENE {
        NONE_OF {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_THUNDER_FORCE, player);
            HP_BAR(opponent);
        }
    }
}

SINGLE_BATTLE_TEST("Thunder Force can't miss in rain")
{
    GIVEN {
        PLAYER(SPECIES_SLIFER_THE_SKY_DRAGON);
        OPPONENT(SPECIES_SWAP_FROG) { Ability(ABILITY_DRIZZLE); MaxHP(999); HP(999); }
    } WHEN {
        TURN { MOVE(player, MOVE_THUNDER_FORCE, hit: FALSE); }
    } SCENE {
        ABILITY_POPUP(opponent, ABILITY_DRIZZLE);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_THUNDER_FORCE, player);
        HP_BAR(opponent);
    }
}
