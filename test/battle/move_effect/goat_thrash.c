#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(GetMoveEffect(MOVE_GOAT_THRASH) == EFFECT_GOAT_THRASH);
    ASSUME(GetMoveEffect(MOVE_SUBSTITUTE) == EFFECT_SUBSTITUTE);
}

SINGLE_BATTLE_TEST("Goat Thrash doubles in power when the user is behind a Substitute", s16 damage)
{
    enum Move setupMove;

    PARAMETRIZE { setupMove = MOVE_CELEBRATE; }
    PARAMETRIZE { setupMove = MOVE_SUBSTITUTE; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { MaxHP(500); HP(500); }
    } WHEN {
        TURN { MOVE(player, setupMove); }
        TURN { MOVE(player, MOVE_GOAT_THRASH); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GOAT_THRASH, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(2.0), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Goat Thrash doesn't break the user's Substitute")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_SUBSTITUTE); }
        TURN { MOVE(player, MOVE_GOAT_THRASH); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GOAT_THRASH, player);
    } THEN {
        EXPECT(player->volatiles.substitute);
    }
}

SINGLE_BATTLE_TEST("Goat Thrash's power returns to normal once the user's Substitute breaks", s16 damage)
{
    bool32 breakSubstitute;

    PARAMETRIZE { breakSubstitute = FALSE; }
    PARAMETRIZE { breakSubstitute = TRUE; }

    GIVEN {
        ASSUME(GetMoveEffect(MOVE_SUPER_FANG) == EFFECT_FIXED_PERCENT_DAMAGE);
        PLAYER(SPECIES_WOBBUFFET) { Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { MaxHP(500); HP(500); Speed(2); }
    } WHEN {
        TURN { MOVE(player, MOVE_SUBSTITUTE); }
        if (breakSubstitute)
            TURN { MOVE(opponent, MOVE_SUPER_FANG); MOVE(player, MOVE_GOAT_THRASH); }
        else
            TURN { MOVE(opponent, MOVE_CELEBRATE); MOVE(player, MOVE_GOAT_THRASH); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GOAT_THRASH, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[1].damage, Q_4_12(2.0), results[0].damage);
    }
}
