#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(GetMoveTerrainType(MOVE_ZOMBIE_WORLD) == B_TERRAIN_ZOMBIE_WORLD);
    ASSUME(GetMoveType(MOVE_SHADOW_PUNCH) == TYPE_GHOST);
    ASSUME(GetMoveEffect(MOVE_SOAK) == EFFECT_SOAK);
    ASSUME(GetMoveEffect(MOVE_FORESTS_CURSE) == EFFECT_THIRD_TYPE);
    ASSUME(GetMoveEffect(MOVE_ICE_SPINNER) == EFFECT_ICE_SPINNER);
}

// The boost applies to base power, so rounding later in the formula
// (+2 constant, STAB, type effectiveness) can push the final damage ratio slightly off 1.1x
SINGLE_BATTLE_TEST("Zombie World increases power of Ghost-type moves by 10 percent", s16 damage)
{
    bool32 terrain;
    PARAMETRIZE { terrain = FALSE; }
    PARAMETRIZE { terrain = TRUE; }
    GIVEN {
        PLAYER(SPECIES_SKULL_SERVANT);
        OPPONENT(SPECIES_THIRTEENTH_GRAVE) { MaxHP(999); HP(999); } // Already pure Ghost, so the matchup doesn't change
    } WHEN {
        if (terrain)
            TURN { MOVE(player, MOVE_ZOMBIE_WORLD); }
        TURN { MOVE(player, MOVE_SHADOW_PUNCH); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SHADOW_PUNCH, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_GE(results[1].damage * 100, results[0].damage * 106);
        EXPECT_LE(results[1].damage * 100, results[0].damage * 112);
    }
}

SINGLE_BATTLE_TEST("Zombie World turns every battler on the field into a pure Ghost type")
{
    GIVEN {
        PLAYER(SPECIES_KING_OF_THE_SKULL_SERVANTS);
        OPPONENT(SPECIES_SANGAN);
    } WHEN {
        TURN { MOVE(player, MOVE_ZOMBIE_WORLD); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ZOMBIE_WORLD, player);
        MESSAGE("The battlefield became a Zombie World!");
    } THEN {
        EXPECT_EQ(player->types[0], TYPE_GHOST);
        EXPECT_EQ(player->types[1], TYPE_GHOST);
        EXPECT_EQ(player->types[2], TYPE_MYSTERY);
        EXPECT_EQ(opponent->types[0], TYPE_GHOST);
        EXPECT_EQ(opponent->types[1], TYPE_GHOST);
        EXPECT_EQ(opponent->types[2], TYPE_MYSTERY);
    }
}

SINGLE_BATTLE_TEST("Zombie World lets Ghost-type moves hit Normal types")
{
    GIVEN {
        PLAYER(SPECIES_SKULL_SERVANT);
        OPPONENT(SPECIES_SANGAN);
    } WHEN {
        TURN { MOVE(player, MOVE_ZOMBIE_WORLD); }
        TURN { MOVE(player, MOVE_SHADOW_PUNCH); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ZOMBIE_WORLD, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SHADOW_PUNCH, player);
        HP_BAR(opponent);
        MESSAGE("It's super effective!");
    }
}

SINGLE_BATTLE_TEST("Zombie World turns battlers that switch in into a pure Ghost type")
{
    GIVEN {
        PLAYER(SPECIES_KING_OF_THE_SKULL_SERVANTS);
        OPPONENT(SPECIES_SANGAN);
        OPPONENT(SPECIES_FROG_THE_JAM);
    } WHEN {
        TURN { MOVE(player, MOVE_ZOMBIE_WORLD); }
        TURN { SWITCH(opponent, 1); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ZOMBIE_WORLD, player);
    } THEN {
        EXPECT_EQ(opponent->species, SPECIES_FROG_THE_JAM);
        EXPECT_EQ(opponent->types[0], TYPE_GHOST);
        EXPECT_EQ(opponent->types[1], TYPE_GHOST);
    }
}

SINGLE_BATTLE_TEST("Zombie World restores original types when it ends, including types changed by Soak")
{
    GIVEN {
        PLAYER(SPECIES_KING_OF_THE_SKULL_SERVANTS);
        OPPONENT(SPECIES_FORTRESS_WHALE);
    } WHEN {
        TURN { MOVE(player, MOVE_ZOMBIE_WORLD); }
        TURN { MOVE(opponent, MOVE_SOAK); }
        TURN {}
        TURN {}
        TURN {}
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ZOMBIE_WORLD, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SOAK, opponent);
        MESSAGE("The Zombie World faded away.");
    } THEN {
        EXPECT_EQ(player->types[0], TYPE_GHOST);
        EXPECT_EQ(player->types[1], TYPE_GROUND);
        EXPECT_EQ(player->types[2], TYPE_MYSTERY);
        EXPECT_EQ(opponent->types[0], TYPE_WATER);
        EXPECT_EQ(opponent->types[1], TYPE_WATER);
    }
}

SINGLE_BATTLE_TEST("Zombie World restores original types when another terrain replaces it, including added third types")
{
    GIVEN {
        PLAYER(SPECIES_KING_OF_THE_SKULL_SERVANTS);
        OPPONENT(SPECIES_INPACHI);
    } WHEN {
        TURN { MOVE(player, MOVE_ZOMBIE_WORLD); }
        TURN { MOVE(opponent, MOVE_FORESTS_CURSE); }
        TURN { MOVE(opponent, MOVE_FOREST_TERRAIN); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ZOMBIE_WORLD, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FORESTS_CURSE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FOREST_TERRAIN, opponent);
        MESSAGE("A forest grew around the battlefield!");
    } THEN {
        EXPECT_EQ(player->types[0], TYPE_GHOST);
        EXPECT_EQ(player->types[1], TYPE_GROUND);
        EXPECT_EQ(player->types[2], TYPE_MYSTERY);
        EXPECT_EQ(opponent->types[0], TYPE_GRASS);
        EXPECT_EQ(opponent->types[1], TYPE_GRASS);
    }
}

SINGLE_BATTLE_TEST("Zombie World restores original types when a move clears the terrain")
{
    GIVEN {
        PLAYER(SPECIES_KING_OF_THE_SKULL_SERVANTS) { MaxHP(999); HP(999); }
        OPPONENT(SPECIES_BLADE_SKATER);
    } WHEN {
        TURN { MOVE(player, MOVE_ZOMBIE_WORLD); }
        TURN { MOVE(opponent, MOVE_ICE_SPINNER); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ZOMBIE_WORLD, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ICE_SPINNER, opponent);
        MESSAGE("The Zombie World faded away.");
    } THEN {
        EXPECT_EQ(player->types[0], TYPE_GHOST);
        EXPECT_EQ(player->types[1], TYPE_GROUND);
        EXPECT_EQ(opponent->types[0], TYPE_ICE);
        EXPECT_EQ(opponent->types[1], TYPE_ICE);
    }
}
