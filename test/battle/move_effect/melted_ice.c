#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(GetMoveEffect(MOVE_MELTED_ICE) == EFFECT_SUPER_EFFECTIVE_ON_ARG);
    ASSUME(GetMoveArgType(MOVE_MELTED_ICE) == TYPE_FIRE);
    ASSUME(GetMoveType(MOVE_MELTED_ICE) == TYPE_ICE);
    ASSUME(MoveHasAdditionalEffect(MOVE_MELTED_ICE, MOVE_EFFECT_FREEZE_OR_FROSTBITE) == TRUE);
}

SINGLE_BATTLE_TEST("Melted Ice is super effective on Fire types")
{
    GIVEN {
        ASSUME(GetSpeciesType(SPECIES_CHARMANDER, 0) == TYPE_FIRE);
        ASSUME(GetSpeciesType(SPECIES_CHARMANDER, 1) == TYPE_FIRE);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_CHARMANDER);
    } WHEN {
        TURN { MOVE(player, MOVE_MELTED_ICE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_MELTED_ICE, player);
        HP_BAR(opponent);
        MESSAGE("It's super effective!");
    }
}

SINGLE_BATTLE_TEST("Melted Ice deals 2x damage to Fire types instead of 0.5x", s16 damage)
{
    u32 species;

    PARAMETRIZE { species = SPECIES_WOBBUFFET; }
    PARAMETRIZE { species = SPECIES_CHARMANDER; }

    GIVEN {
        ASSUME(GetSpeciesType(SPECIES_WOBBUFFET, 0) == TYPE_PSYCHIC);
        ASSUME(GetSpeciesType(SPECIES_WOBBUFFET, 1) == TYPE_PSYCHIC);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(species) { MaxHP(500); HP(500); SpDefense(100); }
    } WHEN {
        TURN { MOVE(player, MOVE_MELTED_ICE, WITH_RNG(RNG_SECONDARY_EFFECT, FALSE)); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_MELTED_ICE, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(2.0), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Melted Ice is super effective on Fire/Water types since only the Fire type is overridden")
{
    GIVEN {
        ASSUME(GetSpeciesType(SPECIES_VOLCANION, 0) == TYPE_FIRE);
        ASSUME(GetSpeciesType(SPECIES_VOLCANION, 1) == TYPE_WATER);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_VOLCANION);
    } WHEN {
        TURN { MOVE(player, MOVE_MELTED_ICE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_MELTED_ICE, player);
        HP_BAR(opponent);
        NOT MESSAGE("It's super effective!");
    }
}

#if B_USE_FROSTBITE == TRUE
SINGLE_BATTLE_TEST("Melted Ice has a 10% chance to inflict frostbite")
#else
SINGLE_BATTLE_TEST("Melted Ice has a 10% chance to inflict freeze")
#endif
{
    PASSES_RANDOMLY(10, 100, RNG_SECONDARY_EFFECT);
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_MELTED_ICE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_MELTED_ICE, player);
        HP_BAR(opponent);
        ANIMATION(ANIM_TYPE_STATUS, (B_USE_FROSTBITE ? B_ANIM_STATUS_FRB : B_ANIM_STATUS_FRZ), opponent);
        FREEZE_OR_FROSTBURN_STATUS(opponent, TRUE);
    }
}
