# Copilot Instructions for dd4 (C-based Multi-User Dungeon)

This project is a C-based MUD. All game files are in `/server`. Code is in `/server/src`. Area files in `/server/area` provide good examples of working areas. Follow these conventions strictly and ask for interaction on any decisions not covered below.

Copilot must:
- Align formatting with existing files.
- Prompt for confirmation when making decisions.
- Prefer comments that describe intended function (the "why" and "goal"), not a narration of the code that exists.
- Adhere to established directory structure and module responsibilities.

---

## Project Structure

- `/server/src`: Core C source files for the server, game logic, networking, persistence, area loading, and command handling.
- `/server/area`: Area definition files. Use these as canonical examples for content and structure.
- `/server`: Other server resources and configuration files.

Do not introduce new top-level directories without explicit approval. If a new module or file is proposed, ask for confirmation and placement (e.g., `/server/src/<module>.c` for C, `/server/include/<module>.h` if headers exist in the repo).

---

## Coding and Commenting Standards

- Language: C99 or the project's existing standard. Ask if you are unsure which standard the project enforces.
- Comments MUST describe the intent (purpose, constraints, invariants, expected behavior), not what the code does line-by-line.
  - Good: "Validates player input to prevent command injection and enforces game rules for target selection."
  - Avoid: "Loop over args, call parse(), check strlen, print error."

- Use existing formatting conventions. When unsure:
  - Match brace placement, indentation, and spacing found in `/server/src`.
  - Match naming conventions for functions, structs, and macros seen in existing code.

- Header files:
  - Place externally visible declarations in headers.
  - Use include guards or `#pragma once` consistent with existing headers.
  - Keep function prototypes and struct definitions consistent with existing style.

- Error handling:
  - Return codes and error logging should match existing patterns.
  - Avoid abrupt exits in library-like code paths unless that's the existing convention.

- Memory management:
  - Follow existing ownership and allocation patterns (`malloc`/`free`, pools, arenas, etc.).
  - Document ownership in intent comments (who frees, lifecycle, and constraints).

---

## Interaction Requirements (Always Ask Before Proceeding)

Copilot must ask for confirmation and details when:
- Adding new commands, mechanics, skills, spells, or areas.
- Changing data formats for area files or save files.
- Introducing new dependencies, libraries, build flags, or compiler options.
- Modifying network protocol behavior (message formats, timing, or concurrency model).
- Changing persistence model or storage paths.
- Creating or renaming directories or moving files.
- Introducing new configuration options or environment variables.
- Altering player-facing text beyond minor typos (tone and lore must be approved).

When proposing any of the above, ask:
- "What is the intended player experience or gameplay outcome?"
- "Which existing area or feature should this be consistent with?"
- "What constraints or invariants must be preserved?"
- "Where should the file(s) live?"
- "Do we need migration for existing data?"

---

## Area Files

- Reference `/server/area` examples for format, tags, IDs, room layout, NPCs, items, and scripts.
- Keep IDs unique and stable; document any ID reservations or ranges.
- Validate links (exits, spawns, triggers). On ambiguity, ask for confirmation.
- Maintain lore consistency and difficulty progression; ask if unsure.

Intent comments for area-related code should specify:
- The narrative role of the area (theme, difficulty).
- Progression and gating mechanics.
- Expected player pathways and edge cases.

---

## Commands and Game Logic

- Match existing command parsing and dispatch style in `/server/src`.
- Preserve permission checks, cooldowns, resource costs, and error messages style.
- Document intended behavior:
  - Preconditions: required state, permissions.
  - Postconditions: effects on player/NPC/world.
  - Side-effects: persistence, cooldowns, logs.

If adding or modifying a command, ask:
- "What is the intended use-case and balance considerations?"
- "Which roles/classes can use this?"
- "What is the expected failure feedback?"

---

## Networking and Concurrency

- Follow existing socket handling, event loop, and threading/concurrency model.
- Document intended throughput, latency targets, and fairness constraints if relevant.
- Avoid protocol changes without explicit approval; propose migrations if needed.

---

## Persistence and Save Data

- Match existing serialization/deserialization approach.
- Document intended durability guarantees, versioning, and migration strategy.
- Ask before changing file formats or locations.

---

## Logging and Diagnostics

- Use existing logging macros or functions.
- Intent comments should state:
  - "Log to aid runtime triage of X under Y conditions."
  - "Rate-limit logging to avoid spam."

---

## Testing and Validation

- Mirror existing test approach if present (unit, integration, runtime validation).
- For features lacking tests, ask for desired validation strategy:
  - Simulation inputs, expected outputs, performance bounds.

---

## Build, Tooling, and CI

- Always create a new Branch for any new idea.
- Do not alter build system without approval. If needed, ask:
  - "Which compilers/flags are supported?"
  - "What target platforms/environments must we support?"

---

## Security and Stability

- Input validation and bounds checking must follow existing patterns.
- Document intended security properties (e.g., "Prevent malformed area files from crashing loader; fail closed with actionable error.").
- Ask before weakening or changing security controls.

---

## Documentation Style

- Place developer-facing docs near code, following existing patterns (top-of-file headers, function-level intent blocks).
- Player-facing docs must be approved and aligned with existing tone.

---

## Intent Comment Templates

Use these templates at the top of new or modified functions or modules. Avoid describing the code structure; focus on intent, constraints, and expected behavior.

### Function Intent (C)
```
/*
  Intent: <What this function is supposed to achieve for gameplay or system behavior.>
  Inputs: <Meaningful inputs and constraints (ranges, nullability, ownership).>
  Outputs: <Meaningful outputs; success/failure semantics; side-effects.>
  Preconditions: <Required state/permissions; invariants to uphold.>
  Postconditions: <State changes; triggered events; persisted data.>
  Failure Behavior: <Errors surfaced to callers; recovery strategy; logging.>
  Performance: <Expected bounds or hot path notes if relevant.>
  Notes: <Compatibility/versioning; references to area formats or command specs.>
*/
```

### Module/File Intent (C)
```
/*
  Module Intent: <Role in the MUD architecture and player experience.>
  Responsibilities: <Primary responsibilities and boundaries.>
  Interactions: <Key modules it integrates with; protocols; data formats.>
  Constraints: <Invariants; ordering; lifetime; thread-safety.>
  Extensibility: <How to safely extend or configure this module.>
*/
```

---

## Example Interaction Prompts Copilot Should Use

- "Confirm the exact file and directory for this addition (default: /server/src)."
- "Provide the intended gameplay behavior, edge cases, and balance constraints for this feature."
- "Which area in /server/area should this be consistent with?"
- "Do we need migration or compatibility with existing save/area formats?"
- "Approve the logging level and messages for failure paths."
- "Confirm naming conventions and header placement to match existing files."

---

## Non-Goals

- Copilot should not refactor broad subsystems without explicit approval.
- Copilot should not invent lore, classes, items, or NPCs without direction.
- Copilot should not change public protocols or file formats silently.

---

## Acceptance Checklist (for PRs touching /server)

- Formatting matches existing files (indentation, braces, naming).
- Comments explain intent and constraints, not code narration.
- Interaction prompts were addressed; decisions recorded.
- Area consistency validated against examples in `/server/area`.
- No unapproved changes to build, protocol, persistence, or directory layout.
- Tests or validation notes included where applicable.

If any item requires a decision, pause and request confirmation before proceeding.
