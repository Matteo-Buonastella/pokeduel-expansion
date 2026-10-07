#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(MoveIgnoresGhostImmunity(MOVE_CHAOS_BLADE));
}

SINGLE_BATTLE_TEST("Chaos Blade hits a Normal-type target")
{
    GIVEN {
        ASSUME(GetSpeciesType(SPECIES_SANGAN, 0) == TYPE_NORMAL);
        ASSUME(GetSpeciesType(SPECIES_SANGAN, 1) == TYPE_NORMAL);
        PLAYER(SPECIES_BLACK_LUSTER_SOLDIER);
        OPPONENT(SPECIES_SANGAN);
    } WHEN {
        TURN { MOVE(player, MOVE_CHAOS_BLADE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CHAOS_BLADE, player);
        HP_BAR(opponent);
    }
}

SINGLE_BATTLE_TEST("Chaos Blade hits a Ghost-type target")
{
    GIVEN {
        ASSUME(GetSpeciesType(SPECIES_SKULL_SERVANT, 0) == TYPE_GHOST);
        ASSUME(GetSpeciesType(SPECIES_SKULL_SERVANT, 1) == TYPE_GHOST);
        PLAYER(SPECIES_BLACK_LUSTER_SOLDIER);
        OPPONENT(SPECIES_SKULL_SERVANT);
    } WHEN {
        TURN { MOVE(player, MOVE_CHAOS_BLADE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CHAOS_BLADE, player);
        HP_BAR(opponent);
        NOT MESSAGE("It doesn't affect the opposing Skull Srv…");
    }
}
