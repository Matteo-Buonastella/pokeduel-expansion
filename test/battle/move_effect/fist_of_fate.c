#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(MoveHasAdditionalEffect(MOVE_FIST_OF_FATE, MOVE_EFFECT_BREAK_SCREEN));
    ASSUME(MoveHasAdditionalEffectSelf(MOVE_FIST_OF_FATE, MOVE_EFFECT_RECHARGE) == TRUE);
    ASSUME(GetMoveEffect(MOVE_REFLECT) == EFFECT_REFLECT);
    ASSUME(GetMoveEffect(MOVE_CELEBRATE) == EFFECT_CELEBRATE);
}

SINGLE_BATTLE_TEST("Fist of Fate makes the user recharge for exactly one turn")
{
    GIVEN {
        PLAYER(SPECIES_OBELISK_THE_TORMENTOR);
        OPPONENT(SPECIES_DARK_MAGICIAN_GIRL) { MaxHP(999); HP(999); }
    } WHEN {
        TURN { MOVE(player, MOVE_FIST_OF_FATE); }
        TURN { SKIP_TURN(player); }
        TURN { MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FIST_OF_FATE, player);
        HP_BAR(opponent);
        MESSAGE("Obelisk must recharge!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
    }
}

SINGLE_BATTLE_TEST("Fist of Fate removes Reflect from the target's side of the field")
{
    GIVEN {
        PLAYER(SPECIES_OBELISK_THE_TORMENTOR);
        OPPONENT(SPECIES_DARK_MAGICIAN_GIRL) { MaxHP(999); HP(999); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_REFLECT); }
        TURN { MOVE(player, MOVE_FIST_OF_FATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_REFLECT, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FIST_OF_FATE, player);
        MESSAGE("The opposing side's Reflect wore off!");
        HP_BAR(opponent);
    }
}

SINGLE_BATTLE_TEST("Fist of Fate removes Reflect and then makes the user recharge")
{
    GIVEN {
        PLAYER(SPECIES_OBELISK_THE_TORMENTOR);
        OPPONENT(SPECIES_DARK_MAGICIAN_GIRL) { MaxHP(999); HP(999); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_REFLECT); }
        TURN { MOVE(player, MOVE_FIST_OF_FATE); }
        TURN { SKIP_TURN(player); }
        TURN { MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_REFLECT, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FIST_OF_FATE, player);
        MESSAGE("The opposing side's Reflect wore off!");
        HP_BAR(opponent);
        MESSAGE("Obelisk must recharge!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
    }
}
