# Handoff — 2026-10-03

ZMK v0.3 config for a wireless Sofle (nice_nano_v2 board target, OLEDs, per-key RGB, encoders).
GitHub Actions builds firmware on every push to `main`; flash `sofle_left` / `sofle_right` from the run's Artifacts.

## Repo conventions
- Firmware versions are just commit titles: `v38`, `v39`, … Latest is **v42**.
- After every push, `keymap-drawer` bot commits a re-rendered `keymap-drawer/sofle.svg` → `git pull` before editing.
- The web keymap editor also commits (`keymap-editor[bot]`, e.g. v38) and reformats `config/sofle.keymap`.

## What changed this session
| Commit | Change |
|---|---|
| `624f6bb` | Windows layer: Ctrl+Backspace → Delete (`win_bspc_del` mod-morph, LCTL only). Key under Backspace → Caps Lock on Windows + Mac. Mac Backspace left stock on purpose. |
| v38 (user, keymap editor) | Windows: key right of `L` → `LA(LEFT_CONTROL)`; `RA(RIGHT_CONTROL)` → `RA(PRINTSCREEN)`. |
| v39 | Mac: key right of `L` → `LA(LEFT_CONTROL)`. Deep sleep 30 → 10 min (`CONFIG_ZMK_IDLE_SLEEP_TIMEOUT=600000`). Tried `RGB_UNDERGLOW_EXT_POWER=y` → **killed the OLEDs**, reverted. |
| v40 | RGB disabled; Lower+Z = `&ext_power EP_ON` (screen power recovery). |
| v41 | RGB back with `RGB_UNDERGLOW_EXT_POWER=n`; custom module `src/rgb_usb_only.c` lights RGB only while *that half* has USB power. RGB toggle key removed; effect/hue/brightness keys restored (Lower+X/C/V/B). |
| v42 | Fix: unplugged right half showed frozen LED colors from power-up. Module now always writes the off frame when unpowered (3 s after boot and on unplug). |

## Status
- v42 built green ([run](https://github.com/jazurite-dev/Sofle-oled_wireless/actions/runs/37099189817)). **Not yet verified on hardware.**
- Expected: right half LEDs go dark ~3 s after boot; plugging either half in lights that half only; unplugging turns it off.
- User confirmed on v41: screens back, left-half USB-only RGB works.

## Things learned (non-obvious)
- On this board the LEDs and OLEDs share the nice!nano switched VCC (ext power). Cutting it to save LED power also kills the screens, and the OLEDs stay blank after power returns until the half restarts.
- ZMK saves the ext-power on/off state in settings and restores it across reflashes. Recovery: Lower+Z, wait ~60 s (settings save debounce), power-cycle both halves.
- RGB behaviors are global (run on both halves) — that's why there's no RGB toggle key: it would light the unplugged half.
- ZMK v0.3's built-in `CONFIG_ZMK_RGB_UNDERGLOW_AUTO_OFF_USB` isn't usable here: the saved RGB state overrides the boot-time USB check, and after any reboot (deep-sleep wake = reboot) plugging in doesn't turn RGB on. Hence the custom module.
- ZMK doesn't blank the strip at boot when RGB is saved as off; WS2812s latch power-up garbage.
- The repo is also a Zephyr module (`zephyr/module.yml`, root `Kconfig` + `CMakeLists.txt`); the v0.3 build workflow picks it up via `ZMK_EXTRA_MODULES`. Enabled by `CONFIG_RGB_USB_ONLY=y` in `config/sofle.conf`.

## Open threads
1. **Verify v42** on hardware (see Status).
2. **Battery**: baseline was ~2 days at 12 h/day, and the *whole keyboard* drops at ~40 % → the left (central) half is dying. ZMK maps 3.45–4.2 V linearly, so 40 % ≈ 3.75 V — not a real LiPo cutoff. Suspects: voltage sag under LED load, or a clone controller misreading voltage. Unknown whether the controllers are genuine nice!nano (user didn't know). Dark-but-powered LEDs still draw idle current — unavoidable while the screens stay on.
3. Untouched levers if battery is still poor: lower `CONFIG_ZMK_RGB_UNDERGLOW_BRT_MAX` (30), drop `CONFIG_BT_CTLR_TX_PWR_PLUS_8`.
4. Housekeeping: `.idea/` untracked, `.DS_Store` committed (no `.gitignore`); ASCII layout comments in `sofle.keymap` are stale stock-Sofle diagrams.
