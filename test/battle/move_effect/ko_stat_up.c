#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(GetMoveEffect(MOVE_FLAME_BLAST) == EFFECT_KO_STAT_UP);
    ASSUME(GetMoveKOStatUpStat(MOVE_FLAME_BLAST) == STAT_ATK);
    ASSUME(GetMoveKOStatUpStages(MOVE_FLAME_BLAST) == 1);
}

SINGLE_BATTLE_TEST("Flame Blast raises user's Attack by 1 stage if it KO's the target")
{
    GIVEN {
        PLAYER(SPECIES_TYRANT_DRAGON) { Moves(MOVE_FLAME_BLAST); }
        OPPONENT(SPECIES_KURIBOH) { HP(1); }
        OPPONENT(SPECIES_KURIBOH);
    } WHEN {
        TURN { MOVE(player, MOVE_FLAME_BLAST); SEND_OUT(opponent, 1); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FLAME_BLAST, player);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Flame Blast doesn't raise user's Attack if it doesn't faint target")
{
    GIVEN {
        PLAYER(SPECIES_TYRANT_DRAGON) { Moves(MOVE_FLAME_BLAST); }
        OPPONENT(SPECIES_YUBEL_ULTIMATE_KNIGHTMARE);
    } WHEN {
        TURN { MOVE(player, MOVE_FLAME_BLAST); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FLAME_BLAST, player);
        NOT ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
    }
}

AI_SINGLE_BATTLE_TEST("AI prefers Flame Blast when it can KO the target")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_KURIBOH) { HP(1); }
        PLAYER(SPECIES_KURIBOH);
        OPPONENT(SPECIES_TYRANT_DRAGON) { Moves(MOVE_FLAME_BLAST, MOVE_TACKLE); }
    } WHEN {
        TURN { EXPECT_MOVE(opponent, MOVE_FLAME_BLAST); SEND_OUT(player, 1); }
    }
}
