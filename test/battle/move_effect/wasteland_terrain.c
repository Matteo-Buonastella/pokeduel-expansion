#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(GetMoveTerrainType(MOVE_WASTELAND_TERRAIN) == B_TERRAIN_WASTELAND);
    ASSUME(GetMoveType(MOVE_BULLDOZE) == TYPE_GROUND);
    ASSUME(GetMoveType(MOVE_ROCK_TOMB) == TYPE_ROCK);
    ASSUME(GetMoveType(MOVE_SHADOW_BONE) == TYPE_GHOST);
    ASSUME(GetMoveType(MOVE_SEED_BOMB) == TYPE_GRASS);
    ASSUME(GetMoveEffect(MOVE_GROWTH) == EFFECT_GROWTH);
    ASSUME(GetMoveEffect(MOVE_LEECH_SEED) == EFFECT_LEECH_SEED);
    ASSUME(GetMoveEffect(MOVE_INGRAIN) == EFFECT_INGRAIN);
}

// The modifiers apply to base power, so rounding later in the formula
// (+2 constant, STAB, type effectiveness) can push the final damage ratio slightly off
SINGLE_BATTLE_TEST("Wasteland Terrain increases power of Ground-, Rock- and Ghost-type moves by 30 percent", s16 damage)
{
    bool32 terrain;
    enum Move move;
    PARAMETRIZE { terrain = FALSE; move = MOVE_BULLDOZE; }
    PARAMETRIZE { terrain = TRUE;  move = MOVE_BULLDOZE; }
    PARAMETRIZE { terrain = FALSE; move = MOVE_ROCK_TOMB; }
    PARAMETRIZE { terrain = TRUE;  move = MOVE_ROCK_TOMB; }
    PARAMETRIZE { terrain = FALSE; move = MOVE_SHADOW_BONE; }
    PARAMETRIZE { terrain = TRUE;  move = MOVE_SHADOW_BONE; }
    GIVEN {
        PLAYER(SPECIES_MAMMOTH_GRAVEYARD);
        OPPONENT(SPECIES_FROG_THE_JAM) { Ability(ABILITY_DAMP); MaxHP(999); HP(999); } // Water takes neutral damage from all three types
    } WHEN {
        if (terrain)
            TURN { MOVE(player, MOVE_WASTELAND_TERRAIN); }
        TURN { MOVE(player, move); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        for (u32 j = 0; j < 6; j += 2)
        {
            EXPECT_GE(results[j + 1].damage * 100, results[j].damage * 125);
            EXPECT_LE(results[j + 1].damage * 100, results[j].damage * 131);
        }
    }
}

SINGLE_BATTLE_TEST("Wasteland Terrain halves the power of Grass-type moves", s16 damage)
{
    bool32 terrain;
    PARAMETRIZE { terrain = FALSE; }
    PARAMETRIZE { terrain = TRUE; }
    GIVEN {
        PLAYER(SPECIES_INPACHI);
        OPPONENT(SPECIES_MAMMOTH_GRAVEYARD) { MaxHP(999); HP(999); }
    } WHEN {
        if (terrain)
            TURN { MOVE(opponent, MOVE_WASTELAND_TERRAIN); }
        TURN { MOVE(player, MOVE_SEED_BOMB); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SEED_BOMB, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_GE(results[1].damage * 100, results[0].damage * 48);
        EXPECT_LE(results[1].damage * 100, results[0].damage * 52);
    }
}

SINGLE_BATTLE_TEST("Wasteland Terrain makes Growth, Leech Seed and Ingrain fail")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_GROWTH; }
    PARAMETRIZE { move = MOVE_LEECH_SEED; }
    PARAMETRIZE { move = MOVE_INGRAIN; }
    GIVEN {
        PLAYER(SPECIES_SPORE);
        OPPONENT(SPECIES_MAMMOTH_GRAVEYARD);
    } WHEN {
        TURN { MOVE(opponent, MOVE_WASTELAND_TERRAIN); }
        TURN { MOVE(player, move); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WASTELAND_TERRAIN, opponent);
        if (move == MOVE_GROWTH)
            MESSAGE("Spore can't use Growth in the wasteland!");
        else if (move == MOVE_LEECH_SEED)
            MESSAGE("Spore can't use Leech Seed in the wasteland!");
        else
            MESSAGE("Spore can't use Ingrain in the wasteland!");
        NOT ANIMATION(ANIM_TYPE_MOVE, move, player);
    }
}

SINGLE_BATTLE_TEST("Growth, Leech Seed and Ingrain work normally outside Wasteland Terrain")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_GROWTH; }
    PARAMETRIZE { move = MOVE_LEECH_SEED; }
    PARAMETRIZE { move = MOVE_INGRAIN; }
    GIVEN {
        PLAYER(SPECIES_SPORE);
        OPPONENT(SPECIES_MAMMOTH_GRAVEYARD);
    } WHEN {
        TURN { MOVE(player, move); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, player);
    }
}

SINGLE_BATTLE_TEST("Wasteland Terrain lasts for 5 turns")
{
    GIVEN {
        PLAYER(SPECIES_MAMMOTH_GRAVEYARD);
        OPPONENT(SPECIES_SANGAN);
    } WHEN {
        TURN { MOVE(opponent, MOVE_CELEBRATE); MOVE(player, MOVE_WASTELAND_TERRAIN); }
        TURN {}
        TURN {}
        TURN {}
        TURN {}
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WASTELAND_TERRAIN, player);
        MESSAGE("The battlefield became a barren wasteland!");
        MESSAGE("The wasteland faded from the battlefield.");
    }
}

AI_SINGLE_BATTLE_TEST("AI does not use Growth, Leech Seed or Ingrain in Wasteland Terrain")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_GROWTH; }
    PARAMETRIZE { move = MOVE_LEECH_SEED; }
    PARAMETRIZE { move = MOVE_INGRAIN; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        // Dragon/Ghost resists Absorb, so the AI only picks it because the other move would fail
        PLAYER(SPECIES_DRAGON_QUEEN_OF_TRAGIC_ENDING);
        OPPONENT(SPECIES_SPORE) { Moves(move, MOVE_ABSORB); }
    } WHEN {
        TURN { MOVE(player, MOVE_WASTELAND_TERRAIN); }
        TURN { EXPECT_MOVE(opponent, MOVE_ABSORB); }
    }
}
