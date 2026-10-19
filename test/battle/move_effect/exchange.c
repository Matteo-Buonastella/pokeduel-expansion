#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(GetMoveEffect(MOVE_EXCHANGE) == EFFECT_EXCHANGE);
    ASSUME(GetMoveEffect(MOVE_TRANSFORM) == EFFECT_TRANSFORM);
}

SINGLE_BATTLE_TEST("Exchange transforms the user into the target and the target into the user")
{
    GIVEN {
        PLAYER(SPECIES_KURIBABYLON) { HP(100); MaxHP(200); Attack(50); Moves(MOVE_EXCHANGE, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_TYRANT_DRAGON) { HP(300); MaxHP(300); Attack(150); Moves(MOVE_FLAME_BLAST, MOVE_TACKLE, MOVE_GROWL); }
    } WHEN {
        TURN { MOVE(player, MOVE_EXCHANGE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_EXCHANGE, player);
        MESSAGE("Kuribabyln and the opposing Tyrant D exchanged forms!");
    } THEN {
        EXPECT_EQ(player->species, SPECIES_TYRANT_DRAGON);
        EXPECT_EQ(opponent->species, SPECIES_KURIBABYLON);
        EXPECT_EQ(player->attack, 150);
        EXPECT_EQ(opponent->attack, 50);
        EXPECT_EQ(player->moves[0], MOVE_FLAME_BLAST);
        EXPECT_EQ(player->moves[2], MOVE_GROWL);
        EXPECT_EQ(opponent->moves[0], MOVE_EXCHANGE);
        EXPECT_EQ(opponent->moves[1], MOVE_CELEBRATE);
        EXPECT_EQ(player->pp[0], 5);
        EXPECT_EQ(opponent->pp[0], 5);
        // HP is kept
        EXPECT_EQ(player->hp, 100);
        EXPECT_EQ(player->maxHP, 200);
        EXPECT_EQ(opponent->hp, 300);
        EXPECT_EQ(opponent->maxHP, 300);
        EXPECT(player->volatiles.transformed);
        EXPECT(opponent->volatiles.transformed);
    }
}

SINGLE_BATTLE_TEST("Exchange makes a slower target use the move in its chosen slot from its new moveset")
{
    GIVEN {
        PLAYER(SPECIES_KURIBABYLON) { Speed(50); Moves(MOVE_CELEBRATE, MOVE_EXCHANGE); }
        OPPONENT(SPECIES_TYRANT_DRAGON) { Speed(10); Moves(MOVE_FLAME_BLAST, MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_EXCHANGE); MOVE(opponent, MOVE_FLAME_BLAST); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_EXCHANGE, player);
        NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_FLAME_BLAST, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
    }
}

SINGLE_BATTLE_TEST("Exchange fails if the target is transformed")
{
    GIVEN {
        PLAYER(SPECIES_KURIBABYLON) { Speed(10); Moves(MOVE_EXCHANGE); }
        OPPONENT(SPECIES_DITTO) { Speed(50); Moves(MOVE_TRANSFORM); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_TRANSFORM); MOVE(player, MOVE_EXCHANGE); }
    } SCENE {
        MESSAGE("The opposing Ditto transformed into Kuribabyln!");
        MESSAGE("Kuribabyln used Exchange!");
        MESSAGE("But it failed!");
    } THEN {
        EXPECT_EQ(player->species, SPECIES_KURIBABYLON);
    }
}

SINGLE_BATTLE_TEST("Exchange fails if the user is transformed")
{
    GIVEN {
        PLAYER(SPECIES_KURIBABYLON) { Speed(50); Moves(MOVE_TRANSFORM, MOVE_EXCHANGE); }
        // Exchange is in the same slot for both, since the player uses the opponent's moveset after Transform
        OPPONENT(SPECIES_TYRANT_DRAGON) { Speed(10); Moves(MOVE_CELEBRATE, MOVE_EXCHANGE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TRANSFORM); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_EXCHANGE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        MESSAGE("Kuribabyln transformed into Tyrant D!");
        MESSAGE("Kuribabyln used Exchange!");
        MESSAGE("But it failed!");
    } THEN {
        EXPECT_EQ(opponent->species, SPECIES_TYRANT_DRAGON);
    }
}

AI_SINGLE_BATTLE_TEST("AI uses Exchange only when the target has better stats")
{
    enum Species species;
    enum Move expectedMove;

    PARAMETRIZE { species = SPECIES_RAYQUAZA; expectedMove = MOVE_EXCHANGE; }
    PARAMETRIZE { species = SPECIES_KURIBOH;  expectedMove = MOVE_CELEBRATE; }

    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(species) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_KURIBABYLON) { Moves(MOVE_EXCHANGE, MOVE_CELEBRATE); }
    } WHEN {
        TURN { EXPECT_MOVE(opponent, expectedMove); }
    }
}

AI_SINGLE_BATTLE_TEST("AI uses Exchange as a last resort if it's faster and about to be KO'd, but only if the target is stronger")
{
    u32 playerAttack;
    enum Move expectedMove;

    // Stat total 510 vs 500, a 2% gain: too small to be worth it normally
    PARAMETRIZE { playerAttack = 160; expectedMove = MOVE_EXCHANGE; }
    // Stat total 360 vs 500: the target is weaker, so Exchange isn't used
    PARAMETRIZE { playerAttack = 10;  expectedMove = MOVE_CELEBRATE; }

    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_KURIBOH) { Attack(playerAttack); Defense(100); SpAttack(100); SpDefense(100); Speed(50); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_KURIBABYLON) { HP(1); Attack(100); Defense(100); SpAttack(100); SpDefense(100); Speed(100); Moves(MOVE_EXCHANGE, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); EXPECT_MOVE(opponent, expectedMove); }
    }
}
