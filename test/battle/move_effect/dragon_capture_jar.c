#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(GetMoveEffect(MOVE_DRAGON_CAPTURE_JAR) == EFFECT_DRAGON_CAPTURE_JAR);
    ASSUME(GetSpeciesType(SPECIES_DRATINI, 0) == TYPE_DRAGON);
}

SINGLE_BATTLE_TEST("Dragon Capture Jar lowers the target's Speed")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_DRAGON_CAPTURE_JAR); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DRAGON_CAPTURE_JAR, player);
        HP_BAR(opponent);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, opponent);
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Dragon Capture Jar doubles in power against Dragon types", s16 damage)
{
    bool32 isDragon;

    PARAMETRIZE { isDragon = FALSE; }
    PARAMETRIZE { isDragon = TRUE; }

    GIVEN {
        ASSUME(GetSpeciesType(SPECIES_RATTATA, 0) == TYPE_NORMAL && GetSpeciesType(SPECIES_RATTATA, 1) == TYPE_NORMAL);
        ASSUME(GetSpeciesType(SPECIES_DRATINI, 1) == TYPE_DRAGON);
        PLAYER(SPECIES_WOBBUFFET);
        // A pure Normal type (neutral to Dragon) vs a pure Dragon type, with the same HP and Defense
        OPPONENT(isDragon ? SPECIES_DRATINI : SPECIES_RATTATA) { MaxHP(999); HP(999); Defense(100); }
    } WHEN {
        TURN { MOVE(player, MOVE_DRAGON_CAPTURE_JAR); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DRAGON_CAPTURE_JAR, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        // Dragon is weak to Dragon (2x) and power is doubled (2x)
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(4.0), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Dragon Capture Jar prevents a Dragon type from escaping")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_DRATINI);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_DRAGON_CAPTURE_JAR); }
    } THEN {
        EXPECT(opponent->volatiles.escapePrevention);
    }
}

SINGLE_BATTLE_TEST("Dragon Capture Jar doesn't trap targets that aren't Dragon types")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WYNAUT);
    } WHEN {
        TURN { MOVE(player, MOVE_DRAGON_CAPTURE_JAR); }
        TURN { SWITCH(opponent, 1); }
    } THEN {
        EXPECT_EQ(opponent->species, SPECIES_WYNAUT); // The switch went through, so it wasn't trapped
    }
}
