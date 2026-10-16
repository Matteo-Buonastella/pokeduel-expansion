#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(GetMoveTerrainType(MOVE_VENOM_SWAMP) == B_TERRAIN_VENOM_SWAMP);
    ASSUME(GetMoveType(MOVE_GUNK_SHOT) == TYPE_POISON);
    ASSUME(GetMoveAccuracy(MOVE_GUNK_SHOT) < 100);
    ASSUME(GetSpeciesType(SPECIES_GRIMER, 0) == TYPE_POISON);
}

SINGLE_BATTLE_TEST("Venom Swamp lowers a poisoned monster's Attack and Sp. Atk at the end of each turn")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Status1(STATUS1_POISON); }
    } WHEN {
        TURN { MOVE(player, MOVE_VENOM_SWAMP); }
        TURN {}
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_VENOM_SWAMP, player);
        MESSAGE("The opposing Wobbuffet is weakened by the Venom Swamp!");
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, opponent);
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 2);
        EXPECT_EQ(opponent->statStages[STAT_SPATK], DEFAULT_STAT_STAGE - 2);
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Venom Swamp doesn't lower the stats of Poison types or monsters with poison-related abilities")
{
    enum Species species = SPECIES_WOBBUFFET;
    enum Ability ability = ABILITY_NONE;
    PARAMETRIZE { species = SPECIES_GRIMER; }
    PARAMETRIZE { ability = ABILITY_POISON_HEAL; }
    PARAMETRIZE { ability = ABILITY_TOXIC_BOOST; }
    PARAMETRIZE { ability = ABILITY_POISON_POINT; }
    PARAMETRIZE { ability = ABILITY_POISON_TOUCH; }
    PARAMETRIZE { ability = ABILITY_MERCILESS; }
    PARAMETRIZE { ability = ABILITY_CORROSION; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        if (ability != ABILITY_NONE)
            OPPONENT(species) { Status1(STATUS1_POISON); Ability(ability); }
        else
            OPPONENT(species) { Status1(STATUS1_POISON); }
    } WHEN {
        TURN { MOVE(player, MOVE_VENOM_SWAMP); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_VENOM_SWAMP, player);
        NONE_OF {
            ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, opponent);
        }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(opponent->statStages[STAT_SPATK], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Venom Swamp doesn't lower the stats of monsters that aren't poisoned")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Status1(STATUS1_BURN); }
    } WHEN {
        TURN { MOVE(player, MOVE_VENOM_SWAMP); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_VENOM_SWAMP, player);
        NONE_OF {
            MESSAGE("The opposing Wobbuffet is weakened by the Venom Swamp!");
        }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(opponent->statStages[STAT_SPATK], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Venom Swamp doubles Poison Heal's recovery", s16 healing)
{
    bool32 terrain;
    PARAMETRIZE { terrain = FALSE; }
    PARAMETRIZE { terrain = TRUE; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_GLISCOR) { Ability(ABILITY_POISON_HEAL); Status1(STATUS1_POISON); MaxHP(400); HP(1); }
    } WHEN {
        TURN { MOVE(player, terrain ? MOVE_VENOM_SWAMP : MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(opponent, ABILITY_POISON_HEAL);
        HP_BAR(opponent, captureDamage: &results[i].healing);
    } FINALLY {
        EXPECT_EQ(results[0].healing, -50);
        EXPECT_EQ(results[1].healing, -100);
    }
}

SINGLE_BATTLE_TEST("Venom Swamp doubles Toxic Boost's power boost", s16 damage)
{
    bool32 terrain;
    PARAMETRIZE { terrain = FALSE; }
    PARAMETRIZE { terrain = TRUE; }
    GIVEN {
        ASSUME(GetMoveCategory(MOVE_SCRATCH) == DAMAGE_CATEGORY_PHYSICAL);
        PLAYER(SPECIES_ZANGOOSE) { Ability(ABILITY_TOXIC_BOOST); Status1(STATUS1_POISON); }
        OPPONENT(SPECIES_WOBBUFFET) { MaxHP(999); HP(999); }
    } WHEN {
        if (terrain)
            TURN { MOVE(opponent, MOVE_VENOM_SWAMP); MOVE(player, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_SCRATCH); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        // 2.0x vs 1.5x, with some slack for rounding later in the formula
        EXPECT_GE(results[1].damage * 100, results[0].damage * 128);
        EXPECT_LE(results[1].damage * 100, results[0].damage * 138);
    }
}

SINGLE_BATTLE_TEST("Venom Swamp makes Poison-type moves never miss")
{
    PASSES_RANDOMLY(100, 100, RNG_ACCURACY);
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_VENOM_SWAMP); }
        TURN { MOVE(player, MOVE_GUNK_SHOT); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GUNK_SHOT, player);
    }
}

SINGLE_BATTLE_TEST("Venom Swamp lasts for 5 turns")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_CELEBRATE); MOVE(player, MOVE_VENOM_SWAMP); }
        TURN {}
        TURN {}
        TURN {}
        TURN {}
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_VENOM_SWAMP, player);
        MESSAGE("The battlefield became a Venom Swamp!");
        MESSAGE("The Venom Swamp dried up.");
    }
}
