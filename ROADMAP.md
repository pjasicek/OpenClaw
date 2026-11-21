# OpenClaw Roadmap

Fork of [pjasicek/OpenClaw](https://github.com/pjasicek/OpenClaw). Goal: complete
the game using the original assets from the physical CD/DVD (CLAW.REZ, MOVIES,
MUSIC copied to `~/Games/OpenClaw/Build_Release/` and symlinked into
`Build_Release/`).

Current branch for recent work: `cutscenes-video`. Upstream: `upstream` remote.

## Build & run

```sh
cmake -S . -B build -DCMAKE_POLICY_VERSION_MINIMUM=3.5 \
      -DCMAKE_EXE_LINKER_FLAGS="-L/opt/homebrew/lib"
cmake --build build -j 8        # binary lands in Build_Release/openclaw
cd Build_Release && ./openclaw
```

- ffmpeg is a runtime dependency (Homebrew).
- `MOVIES/`, `MUSIC/`, `CUSTOM/`, `CLAW.REZ` are symlinks to the DVD copy
  (gitignored). Copy permissions were fixed with `chmod -R u+w`; the rsync
  originally preserved the read-only UDF permissions.
- Console: `video <file>` replays any cutscene, `loadlevel <1-14>` jumps to a
  level. Do NOT put `loadlevel` in `startup_commands.txt` — startup commands
  execute after every level load (`BaseGameLogic.cpp:421`), so it loops forever.

## Done

### Cutscene system (commits a1d0eeb..e03eccd)

- `Engine/Video/ScreenElementVideo.{h,cpp}`: decodes the DVD's MPEG-2 clips via
  an ffmpeg pipe (`rawvideo rgb24`) into an SDL texture, 720x480, ~30 fps,
  4:3 letterbox (clips are 4:3 despite the storage resolution). Any key/click
  skips. Missing ffmpeg or clip = logged error, game continues normally.
- Audio: SDL2_mixer cannot decode AC3, so the track is extracted once with
  ffmpeg to `<name>.VOB.wav` in the game's working directory and streamed via
  `Mix_PlayMusic`. Missing WAV = silent video (non-blocking).
- `HumanView::PlayVideo` + `kLevelVideos` table: the level's cutscene plays
  before the level loads; the load is re-queued when the video ends
  (`m_HasPendingLevelLoad`). Per-session replay guard
  (`m_VideoPlayedForLevel`), reset on every new game so the intro replays.
- Full cutscene-to-level mapping identified by frame inspection and confirmed
  by the REZ "Movies" replay menu (10 narrative slots matching the VOB
  numbering):

  | Clip | Duration | Level |
  |---|---|---|
  | CLAW0.VOB | 6:55 | 1 (new game only) |
  | CLAW1.VOB | 1:03 | 2 Battlements |
  | CLAW2.VOB | 0:33 | 3 |
  | CLAW3.VOB | 1:50 | 5 Town |
  | CLAW4.VOB | 0:32 | 8 Shipyards |
  | CLAW5.VOB | 2:26 | 9 Pirate's Cove |
  | CLAW6.VOB | 0:39 | 10 Cliffs |
  | CLAW7.VOB | 1:29 | 13 Tiger Island |
  | CLAW8.VOB | 2:16 | 14 final |
  | CLAW.FEC | ~9:40? | 10th slot (ending) |

  Uncertainties (identified from frames, not gameplay — fix the table row if
  wrong when playing through): CLAW2 could be 5, CLAW4 could be 7, CLAW6
  could be 13.
- Verified in-game with window captures: video renders (letterbox + audio),
  on finish the pending level load fires and the level loads. The "frozen
  render" suspicion from the first session was a screenshot artifact (captured
  exactly during the loading screen).
- Note: level names in `GameSaves.h` are the fork author's approximations and
  do not match the real ones (level 3's loading screen reads "The Footpath",
  not "Thief's Forest"). Mapping uses level numbers, so it is unaffected.

## Todo

### Gameplay blockers (large)

- **Level 14 + final boss** — does not exist (upstream issue #155). CLAW8
  cutscene (amulet complete, magic portal) leads into it.
- **Level 13 / Red Tail boss** — playable with a *dummy* boss (`e44c2ad`,
  `RedTailEncounter.cpp` = 131 lines).
- **Level 12 / Aquatis boss** — simplified, only stands still attacking melee
  (`AquatisEncounter.cpp` = 136 lines).
- **Level 10 / Marrow boss** — very simplified (304 lines).
- **Level 4 / Katherine** — does not parry Claw's shots/claws/dynamite
  (upstream issue #111).
- **Refactor all bosses** — long-standing upstream request (issue #71).
  `GabrielEncounter.cpp` (559 lines) is the quality reference.

### Gameplay mechanics

- **Pick up and throw barrels/enemies** — not implemented (upstream #174).
  `ActorTemplates.cpp:1550` has a literal `TODO: create xml actor prototype
  and implement lift-throw logic`. Open PR #223 already does this — do NOT
  redo; track and review instead.
- **Original game cheats** (#166) — partially present in the console.
- `SoundTriggerComponent` and `GlobalAmbientSoundComponent` — literal
  `TODO: Implement` stubs.
- `FollowableComponent`, `PickupComponent` — partial.

### Cutscenes / media

- **Decode `CLAW.FEC`** (ending cinematic) — Monolith's proprietary FEC
  format, ffmpeg and libwap cannot read it. Only worth it when level 14
  exists.
- CLAW2/CLAW4/CLAW6 placement uncertainties (see table above) — confirm by
  playing through. (CLAW4 visually confirmed as the docks night scene.)

### Display on macOS

- Double-click launcher: `/Applications/OpenClaw.app` (local, gitignored) —
  a minimal bundle (script + Info.plist + icns converted from
  `ClawLauncher/launcher_icon.ico`). The script runs the game binary with
  `Build_Release` as the working directory: if the bundle sits inside
  `Build_Release` it uses the sibling binary, otherwise it falls back to
  the built checkout at `~/vibe-coding-projects/OpenClaw/Build_Release`.
  Log at `/tmp/openclaw_last.log`. Regenerate the icns from the .ico if
  rebuilding the bundle. The fork's C# `ClawLauncher/` is Windows-only
  (WinForms); the original CD's `AUTORUN.EXE` is a 1997 Windows binary.
- Fullscreen works and is on by default: `IsFullscreenDesktop=true` in
  `Build_Release/config.xml`. macOS silently drops the
  `SDL_SetWindowFullscreen` request while the app is still activating
  (e.g. launched from Finder), so `BaseGameApp::InitializeDisplay` retries
  for up to 2s and verifies the flag took. Result: true fullscreen (menu
  bar and Dock hidden), world view widens without stretching, HUD, menus
  and cutscene letterboxing all adapt (verified at 1512x982).
- Exclusive fullscreen (`IsFullscreen=true`) does NOT stick on macOS for
  apps launched from a background terminal (SDL exclusive mode bounces back).
  Not recommended on Retina anyway — it switches to a scaled non-native
  video mode. Keep it false.

### Audio (macOS)

- Music is NOT dead by design: the non-Windows path in `Audio::PlayMusic`
  already feeds the XMI/MIDI data to `Mix_LoadMUS_RW`, and the linked
  SDL2_mixer has FluidSynth — it fails only with "No SoundFonts have been
  requested". Providing a `.sf2` soundfont (via `SDL_MIXER_SOUNDFONTS` env
  var or `Mix_SetSoundFonts`) should make the MIDI soundtrack play. Much
  smaller task than a MidiProc port.
- After a `video` console command mid-game, level music is not resumed
  (matters only on platforms where music works).

### Technical debt

- Physics has several `TODO: HACK` markers (falling, ladders, platforms).
- `HumanView::VOnRender` contains a pre-existing `m_RunFullSpeed || true`.
- Level WAV cache (`CLAWn.VOB.wav`) accumulates in the working directory;
  harmless but not cleaned up.
