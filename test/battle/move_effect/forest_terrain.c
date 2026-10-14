#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(GetMoveTerrainType(MOVE_FOREST_TERRAIN) == B_TERRAIN_FOREST);
    ASSUME(GetMoveType(MOVE_SEED_BOMB) == TYPE_GRASS);
    ASSUME(GetMoveType(MOVE_X_SCISSOR) == TYPE_BUG);
    ASSUME(GetMoveType(MOVE_LEECH_LIFE) == TYPE_BUG);
    ASSUME(GetMoveCategory(MOVE_SCRATCH) == DAMAGE_CATEGORY_PHYSICAL);
}

SINGLE_BATTLE_TEST("Forest Terrain increases power of Grass-type moves by 30 percent", s16 damage)
{
    bool32 terrain;
    PARAMETRIZE { terrain = FALSE; }
    PARAMETRIZE { terrain = TRUE; }
    GIVEN {
        PLAYER(SPECIES_INPACHI);
        OPPONENT(SPECIES_SANGAN);
    } WHEN {
        if (terrain)
            TURN { MOVE(player, MOVE_FOREST_TERRAIN); }
        TURN { MOVE(player, MOVE_SEED_BOMB); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SEED_BOMB, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(1.3), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Forest Terrain increases power of Bug-type moves by 30 percent", s16 damage)
{
    bool32 terrain;
    PARAMETRIZE { terrain = FALSE; }
    PARAMETRIZE { terrain = TRUE; }
    GIVEN {
        PLAYER(SPECIES_BASIC_INSECT) { Ability(ABILITY_LONG_REACH); }
        OPPONENT(SPECIES_BLAZING_INPACHI); // Grass/Fire takes neutral damage from Bug
    } WHEN {
        if (terrain)
            TURN { MOVE(opponent, MOVE_FOREST_TERRAIN); }
        TURN { MOVE(player, MOVE_X_SCISSOR); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_X_SCISSOR, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(1.3), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Forest Terrain increases Defense of Pokémon with Swarm or Overgrow by 30 percent", s16 damage)
{
    enum Species species;
    enum Ability ability;
    PARAMETRIZE { species = SPECIES_JERRY_BEANS_MAN; ability = ABILITY_HARVEST; }
    PARAMETRIZE { species = SPECIES_JERRY_BEANS_MAN; ability = ABILITY_OVERGROW; }
    PARAMETRIZE { species = SPECIES_BASIC_INSECT; ability = ABILITY_LONG_REACH; }
    PARAMETRIZE { species = SPECIES_BASIC_INSECT; ability = ABILITY_SWARM; }
    GIVEN {
        PLAYER(species) { Ability(ability); }
        OPPONENT(SPECIES_INPACHI);
    } WHEN {
        TURN { MOVE(opponent, MOVE_FOREST_TERRAIN); }
        TURN { MOVE(opponent, MOVE_SCRATCH); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, opponent);
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[1].damage, Q_4_12(1.3), results[0].damage);
        EXPECT_MUL_EQ(results[3].damage, Q_4_12(1.3), results[2].damage);
    }
}

SINGLE_BATTLE_TEST("Forest Terrain cures the status of Grass and Bug types at the end of the turn")
{
    u32 status;
    PARAMETRIZE { status = STATUS1_BURN; }
    PARAMETRIZE { status = STATUS1_PARALYSIS; }
    PARAMETRIZE { status = STATUS1_POISON; }
    PARAMETRIZE { status = STATUS1_SLEEP; }
    GIVEN {
        PLAYER(SPECIES_INPACHI);
        OPPONENT(SPECIES_BASIC_INSECT) { Status1(status); }
    } WHEN {
        TURN { MOVE(player, MOVE_FOREST_TERRAIN); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FOREST_TERRAIN, player);
        MESSAGE("A forest grew around the battlefield!");
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_SIMPLE_HEAL, opponent);
        MESSAGE("The opposing B Insect's status was cured by the forest!");
        STATUS_ICON(opponent, none: TRUE);
    } THEN {
        EXPECT_EQ(opponent->status1, STATUS1_NONE);
    }
}

SINGLE_BATTLE_TEST("Forest Terrain does not cure the status of non-Grass and non-Bug types")
{
    GIVEN {
        PLAYER(SPECIES_INPACHI);
        OPPONENT(SPECIES_SANGAN) { Status1(STATUS1_BURN); }
    } WHEN {
        TURN { MOVE(player, MOVE_FOREST_TERRAIN); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FOREST_TERRAIN, player);
        NOT MESSAGE("The opposing Sangan's status was cured by the forest!");
        HP_BAR(opponent);
    } THEN {
        EXPECT_EQ(opponent->status1, STATUS1_BURN);
    }
}

SINGLE_BATTLE_TEST("Forest Terrain boosts Bug-type moves of ungrounded Pokémon", s16 damage)
{
    bool32 terrain;
    PARAMETRIZE { terrain = FALSE; }
    PARAMETRIZE { terrain = TRUE; }
    GIVEN {
        PLAYER(SPECIES_KILLER_NEEDLE) { Ability(ABILITY_SNIPER); }
        OPPONENT(SPECIES_BLAZING_INPACHI); // Grass/Fire takes neutral damage from Bug
    } WHEN {
        if (terrain)
            TURN { MOVE(opponent, MOVE_FOREST_TERRAIN); }
        TURN { MOVE(player, MOVE_LEECH_LIFE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_LEECH_LIFE, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        // The boost applies to base power, so rounding later in the formula
        // (+2 constant, STAB) pulls the final damage ratio slightly under 1.3x
        EXPECT_GE(results[1].damage * 100, results[0].damage * 125);
        EXPECT_LE(results[1].damage * 100, results[0].damage * 130);
    }
}

SINGLE_BATTLE_TEST("Forest Terrain cures the status of ungrounded Bug types")
{
    GIVEN {
        PLAYER(SPECIES_INPACHI);
        OPPONENT(SPECIES_KILLER_NEEDLE) { Status1(STATUS1_PARALYSIS); }
    } WHEN {
        TURN { MOVE(player, MOVE_FOREST_TERRAIN); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FOREST_TERRAIN, player);
        MESSAGE("The opposing Killer Ndl's status was cured by the forest!");
    } THEN {
        EXPECT_EQ(opponent->status1, STATUS1_NONE);
    }
}

SINGLE_BATTLE_TEST("Forest Terrain activates the Grassy Seed")
{
    GIVEN {
        ASSUME(gItemsInfo[ITEM_GRASSY_SEED].holdEffect == HOLD_EFFECT_TERRAIN_SEED);
        PLAYER(SPECIES_INPACHI);
        OPPONENT(SPECIES_SANGAN) { Item(ITEM_GRASSY_SEED); }
    } WHEN {
        TURN { MOVE(player, MOVE_FOREST_TERRAIN); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FOREST_TERRAIN, player);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, opponent);
        MESSAGE("The Grassy Seed boosted the opposing Sangan's Defense!");
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Forest Terrain activates Grass Pelt", s16 damage)
{
    enum Ability ability;
    PARAMETRIZE { ability = ABILITY_CONTRARY; }
    PARAMETRIZE { ability = ABILITY_GRASS_PELT; }
    GIVEN {
        PLAYER(SPECIES_PREDAPLANT_VERTE_ANACONDA) { Ability(ability); }
        OPPONENT(SPECIES_INPACHI);
    } WHEN {
        TURN { MOVE(opponent, MOVE_FOREST_TERRAIN); }
        TURN { MOVE(opponent, MOVE_SCRATCH); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, opponent);
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[1].damage, Q_4_12(1.5), results[0].damage);
    }
}

SINGLE_BATTLE_TEST("Forest Terrain lasts for 5 turns")
{
    GIVEN {
        PLAYER(SPECIES_INPACHI);
        OPPONENT(SPECIES_SANGAN);
    } WHEN {
        TURN { MOVE(opponent, MOVE_CELEBRATE); MOVE(player, MOVE_FOREST_TERRAIN); }
        TURN {}
        TURN {}
        TURN {}
        TURN {}
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FOREST_TERRAIN, player);
        MESSAGE("A forest grew around the battlefield!");
        MESSAGE("The forest disappeared from the battlefield.");
    }
}
