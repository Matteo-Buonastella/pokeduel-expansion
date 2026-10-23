#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(GetMoveEffect(MOVE_ORDER_TO_SMASH) == EFFECT_ORDER_TO_SMASH);
    ASSUME(MoveHasAdditionalEffect(MOVE_ORDER_TO_SMASH, MOVE_EFFECT_BREAK_SCREEN));
}

SINGLE_BATTLE_TEST("Order to Smash knocks off the target's item")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Item(ITEM_LEFTOVERS); }
    } WHEN {
        TURN { MOVE(player, MOVE_ORDER_TO_SMASH); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ORDER_TO_SMASH, player);
        HP_BAR(opponent);
        MESSAGE("Wobbuffet knocked off the opposing Wobbuffet's Leftovers!");
    } THEN {
        EXPECT(opponent->item == ITEM_NONE);
    }
}

SINGLE_BATTLE_TEST("Order to Smash doesn't get a power boost if the target is holding an item", s16 damage)
{
    enum Item item;

    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_LEFTOVERS; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Item(item); }
    } WHEN {
        TURN { MOVE(player, MOVE_ORDER_TO_SMASH); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Order to Smash breaks Light Screen before dealing damage", s16 damage)
{
    bool32 lightScreen;

    PARAMETRIZE { lightScreen = FALSE; }
    PARAMETRIZE { lightScreen = TRUE; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(50); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); }
    } WHEN {
        TURN { MOVE(opponent, lightScreen ? MOVE_LIGHT_SCREEN : MOVE_CELEBRATE); MOVE(player, MOVE_ORDER_TO_SMASH); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ORDER_TO_SMASH, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        // The screen is broken before the hit, so damage is the same
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Order to Smash breaks Reflect")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(50); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_REFLECT); MOVE(player, MOVE_ORDER_TO_SMASH); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_REFLECT, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ORDER_TO_SMASH, player);
        MESSAGE("The opposing side's Reflect wore off!");
    } THEN {
        EXPECT(!(gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_REFLECT));
    }
}

SINGLE_BATTLE_TEST("Order to Smash still breaks screens if the target has Sticky Hold")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(50); }
        OPPONENT(SPECIES_MUK) { Ability(ABILITY_STICKY_HOLD); Item(ITEM_LEFTOVERS); Speed(100); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_LIGHT_SCREEN); MOVE(player, MOVE_ORDER_TO_SMASH); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ORDER_TO_SMASH, player);
        MESSAGE("The opposing side's Light Screen wore off!");
        ABILITY_POPUP(opponent, ABILITY_STICKY_HOLD);
    } THEN {
        EXPECT(opponent->item == ITEM_LEFTOVERS);
        EXPECT(!(gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_LIGHTSCREEN));
    }
}
