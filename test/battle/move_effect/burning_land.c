#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(GetMoveEffect(MOVE_BURNING_LAND) == EFFECT_BURNING_LAND);
    ASSUME(GetMoveNonVolatileStatus(MOVE_BURNING_LAND) == MOVE_EFFECT_BURN);
}

SINGLE_BATTLE_TEST("Burning Land burns both the target and the user")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_BURNING_LAND); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BURNING_LAND, player);
        STATUS_ICON(opponent, burn: TRUE);
        STATUS_ICON(player, burn: TRUE);
    } THEN {
        EXPECT(opponent->status1 & STATUS1_BURN);
        EXPECT(player->status1 & STATUS1_BURN);
    }
}

SINGLE_BATTLE_TEST("Burning Land removes any active terrain")
{
    GIVEN {
        ASSUME(GetMoveEffect(MOVE_ELECTRIC_TERRAIN) == EFFECT_TERRAIN);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_ELECTRIC_TERRAIN); MOVE(player, MOVE_BURNING_LAND); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ELECTRIC_TERRAIN, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BURNING_LAND, player);
        MESSAGE("The electricity disappeared from the battlefield.");
    } THEN {
        EXPECT_EQ(gFieldTimers.terrain, B_TERRAIN_NONE);
    }
}

SINGLE_BATTLE_TEST("Burning Land removes any active weather")
{
    GIVEN {
        ASSUME(GetMoveEffect(MOVE_RAIN_DANCE) == EFFECT_WEATHER);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_RAIN_DANCE); MOVE(player, MOVE_BURNING_LAND); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_RAIN_DANCE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BURNING_LAND, player);
        MESSAGE("The rain stopped.");
    } THEN {
        EXPECT(!(gBattleWeather & B_WEATHER_ANY));
    }
}

SINGLE_BATTLE_TEST("Burning Land fails if the target can't be burned")
{
    GIVEN {
        ASSUME(GetSpeciesType(SPECIES_CHARMANDER, 0) == TYPE_FIRE);
        ASSUME(GetMoveEffect(MOVE_ELECTRIC_TERRAIN) == EFFECT_TERRAIN);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_CHARMANDER);
    } WHEN {
        TURN { MOVE(opponent, MOVE_ELECTRIC_TERRAIN); MOVE(player, MOVE_BURNING_LAND); }
    } SCENE {
        NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_BURNING_LAND, player);
    } THEN {
        EXPECT(!(player->status1 & STATUS1_BURN));
        EXPECT_EQ(gFieldTimers.terrain, B_TERRAIN_ELECTRIC); // Terrain isn't removed either
    }
}

SINGLE_BATTLE_TEST("Burning Land still burns the target if the user can't be burned")
{
    GIVEN {
        ASSUME(GetSpeciesType(SPECIES_CHARMANDER, 0) == TYPE_FIRE);
        PLAYER(SPECIES_CHARMANDER);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_BURNING_LAND); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BURNING_LAND, player);
        STATUS_ICON(opponent, burn: TRUE);
        NOT STATUS_ICON(player, burn: TRUE);
    } THEN {
        EXPECT(opponent->status1 & STATUS1_BURN);
        EXPECT(!(player->status1 & STATUS1_BURN));
    }
}

AI_SINGLE_BATTLE_TEST("AI prefers Burning Land when burning itself activates Guts")
{
    u32 ability;

    PARAMETRIZE { ability = ABILITY_GUTS; }
    PARAMETRIZE { ability = ABILITY_RUN_AWAY; }

    GIVEN {
        ASSUME(GetMoveEffect(MOVE_WILL_O_WISP) == EFFECT_NON_VOLATILE_STATUS);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_RATTATA) { Ability(ability); Moves(MOVE_BURNING_LAND, MOVE_WILL_O_WISP, MOVE_SCRATCH); }
    } WHEN {
        if (ability == ABILITY_GUTS)
            TURN { MOVE(player, MOVE_CELEBRATE); EXPECT_MOVE(opponent, MOVE_BURNING_LAND); }
        else
            TURN { MOVE(player, MOVE_CELEBRATE); NOT_EXPECT_MOVE(opponent, MOVE_BURNING_LAND); }
    }
}

SINGLE_BATTLE_TEST("Burning Land's self-burn is cured by the user's Rawst Berry")
{
    GIVEN {
        ASSUME(GetItemHoldEffect(ITEM_RAWST_BERRY) == HOLD_EFFECT_CURE_BRN);
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_RAWST_BERRY); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_BURNING_LAND); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BURNING_LAND, player);
        STATUS_ICON(player, burn: TRUE);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, player);
    } THEN {
        EXPECT(!(player->status1 & STATUS1_BURN));
        EXPECT(opponent->status1 & STATUS1_BURN);
    }
}

AI_SINGLE_BATTLE_TEST("AI doesn't count the self-burn against Burning Land if a berry will cure it")
{
    u32 item;

    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_LUM_BERRY; }

    GIVEN {
        ASSUME(GetMoveEffect(MOVE_WILL_O_WISP) == EFFECT_NON_VOLATILE_STATUS);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_SCRATCH); }
        OPPONENT(SPECIES_WOBBUFFET) { Item(item); Moves(MOVE_BURNING_LAND, MOVE_WILL_O_WISP, MOVE_SCRATCH); }
    } WHEN {
        // Both moves burn the target the same way, so only the self-burn separates them
        if (item == ITEM_NONE)
            TURN { MOVE(player, MOVE_SCRATCH); SCORE_LT(opponent, MOVE_BURNING_LAND, MOVE_WILL_O_WISP); }
        else
            TURN { MOVE(player, MOVE_SCRATCH); SCORE_EQ(opponent, MOVE_BURNING_LAND, MOVE_WILL_O_WISP); }
    }
}

AI_SINGLE_BATTLE_TEST("AI values Burning Land more when it clears the foe's weather, and less when it clears its own")
{
    u32 species;

    // Sun boosts Charizard's Fire moves; rain boosts Kyogre's Water moves
    PARAMETRIZE { species = SPECIES_KYOGRE; }
    PARAMETRIZE { species = SPECIES_CHARIZARD; }

    GIVEN {
        ASSUME(GetMoveEffect(MOVE_WILL_O_WISP) == EFFECT_NON_VOLATILE_STATUS);
        ASSUME(GetMoveEffect(MOVE_SUNNY_DAY) == EFFECT_WEATHER);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_SUNNY_DAY, MOVE_SCRATCH); }
        OPPONENT(species) { Moves(MOVE_BURNING_LAND, MOVE_WILL_O_WISP, MOVE_FLAMETHROWER, MOVE_SURF); }
    } WHEN {
        TURN { MOVE(player, MOVE_SUNNY_DAY); MOVE(opponent, MOVE_FLAMETHROWER); }
        // Both moves burn the target the same way, so only the weather clear separates them
        if (species == SPECIES_KYOGRE)
            TURN { MOVE(player, MOVE_SCRATCH); SCORE_GT(opponent, MOVE_BURNING_LAND, MOVE_WILL_O_WISP); }
        else
            TURN { MOVE(player, MOVE_SCRATCH); SCORE_LT(opponent, MOVE_BURNING_LAND, MOVE_WILL_O_WISP); }
    }
}

AI_SINGLE_BATTLE_TEST("AI doesn't use Burning Land if the target can't be burned")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_CHARMANDER) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_BURNING_LAND, MOVE_SCRATCH); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); EXPECT_MOVE(opponent, MOVE_SCRATCH); }
    }
}
