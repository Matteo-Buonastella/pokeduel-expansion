# pokeduel-expansion

A pokeemerald-expansion based romhack with Yu-Gi-Oh themed custom moves, species and abilities.

## Builds

Never run `make` or any build/compile/test command. The user runs builds and tests themselves. Diagnose by reading code and git history, make the edits, then hand over the command to run.

## Adding custom moves

Custom moves live at the end of the move list (before `MOVES_COUNT_POKEDUEL`). The user usually adds the constant, the `moves_info.h` entry and commented-out learnset lines first, then asks for the effect and AI.

Where things go:
- Move constant: `include/constants/moves.h`
- Move data: `src/data/moves_info.h` (custom moves are just before the Z-Moves section)
- Learnsets: `src/data/pokemon/level_up_learnsets/pokeduel.h`. Uncomment the move's `// LEVEL_UP_MOVE(...)` lines once it's implemented, but leave lines that are commented out twice (`// //`), since those are disabled on purpose.
- New move effect: `EFFECT_*` in `include/constants/battle_move_effects.h` (with a short comment) plus an entry in `src/data/battle_move_effects.h`
- Battle scripts: `data/battle_scripts_1.s` plus an `extern` in `include/battle_scripts.h`
- Native script commands: `BS_*` function in `src/battle_script_commands.c` plus a macro in `asm/macros/battle_script.inc`
- Secondary effects (applied on hit): `MOVE_EFFECT_*` in `include/constants/battle_set_effect.h` plus a handler in the table in `src/battle_set_effect.c`
- Base power modifiers: `CalcMoveBasePowerAfterModifiers` in `src/battle_util.c`
- Protect variants: `ProtectMethod` in `include/move.h`, plus the protect checks in `src/battle_util.c`, `src/battle_ai_util.c` and `MoveEndProtectLikeEffect` in `src/battle_move_resolution.c`
- New battle messages: `include/constants/battle_string_ids.h` plus `src/battle_message.c`

Good existing custom implementations to copy from: Fiendish Chain (two effects in one move), Warrior Return Alive (heal + stat raise), Moon Block / Rock Crush Sword (custom secondary effect), Skull Dice (setting a stat change at runtime), Cold Wave (custom secondary effect with a battle script).

Conventions:
- A move that does two things should fail only when neither half can apply, and should still do whichever half it can.
- Move names must fit in `MOVE_NAME_LENGTH` (16 characters). `test/text.c` also checks the pixel width.
- `battleAnimScript` is often a placeholder; animations are done separately, so leave it unless asked.
- Additional effects with a `.chance` are secondary effects: Sheer Force boosts them and removes the effect, and Shield Dust / Covert Cloak block them. Effects without `.chance` are guaranteed and ignore all of these.

## Sheer Force test list

`test/battle/ability/sheer_force.c` has `IsMoveSheerForceBoosted`, a list of moves that Sheer Force should boost. The last test loops up to `MOVES_COUNT`, so it checks every custom move. When a custom move gains or loses an additional effect with a `.chance`, add it to or remove it from that list. Moves whose only additional effect has no `.chance` (e.g. a plain drain) must not be listed.

## AI

- Move effect scoring is in `src/battle_ai_main.c`: `AI_CheckBadMove` (penalties), `AI_CalcMoveEffectScore` (status move bonuses) and `AI_CalcAdditionalEffectScore` (secondary effects of damaging moves). Ability-changing moves also need cases in the helpers in `src/battle_ai_util.c` (`CanEffectChangeAbility`, `AbilityChangeScore`, etc.).
- Damage-based effects (power changes) need no AI work, because the AI uses the same damage calculation.
- Penalty scale in `AI_CheckBadMove`: use `ADJUST_SCORE(-10)` only when the move would fail or do nothing. Moves that work but are a bad idea get -6 to -8, so the AI can still fall back on them (Recover at 90%+ HP gets -9).

## Tests

- Move effect tests go in `test/battle/move_effect/<effect>.c`; secondary effect tests go in `test/battle/move_effect_secondary/<effect>.c`.
- Avoid AI tests that depend on the AI choosing a fixed sequence of moves; they are unreliable.
- In AI tests that check a discouraged move isn't picked, give the AI a real alternative such as Tackle, not Splash, since Splash itself scores -10.
