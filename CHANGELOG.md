# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added
- Added more subway assets in leve3
- Added some floors to 2.2 building
- Added all floors to 2.2 building
- Added plants 2.2
- Added BP_LampPost_KMP

### Changed
- Changed sized of level3
- Changed landscape on level 2, 2-2, 3, 4. 
- Made all levels align  

### Deprecated
- 

### Removed
- 

### Fixed
- Fixed Tunnel in level3

### Security
-

# [0.3.1] - 2026-02-27

### Changed
- Changed location of assets
- Moved Kaylas Assets
- Assigned Some Textures 

### Removed
- Deleted Alyshas and Zanias assets


# [0.3.0] - 2026-02-25

### Added
- Added Textures for subway, Plants (Artists)
- Added Flame BP in AssetOverview Level (Artists)
- Added new fixed version of mail box (Artists)
- Added textures to plant assets (Artists)
- Added plant assets to Lv_Assetoverview (Artists)
- Added Floor,ceiling asset for chaos (Artists)
- Added Empty building for chaos (Artists)
- Added more buildings in level1 (Artists)
- Added test level for level streaming `Content/Levels/CodersTestLevels/LVL_Streaming/LV_CodeTest_4.umap`
- Added setup for future functionality for a master pawn/player for authority for Sifan `Content/Blueprints/BP_GameState.uasset`
- Added Aim Assist feature MVP `Content/Custom_Shooter/Shooter_BPs/BP_ShooterPlayer.uasset`
- Added Race Standings Feature
- Added Standings Enum `Content/DataStuctures/F_Standings.uasset`
- Added Race Standing Widget `Content/UI/WBP_RaceStandings.uasset`
- Added Track Spline Actor `Source/DeathRace/TrackSplineBakerActor.h`

### Changed
- Changed Master material for MI_Plants_YDJ (Artists)
- Changed asset location of oldAssetsCombined to NewAssets_Forlevel Folder (Artists)
- Changed Lighting (Artists)
- Changed Dongjoos building assets to new asset folder (Artists)
- Moved Cams asset to newasset folder (Artists)
- Player State now holds position, nitro and team information based on the player's role. `Content/Networking/BP_PlayerState.uasset`
- Updated logic for level streaming `Content/Levels/CodersTestLevels/LVL_Streaming/LV_CodeTest_4.umap`
- Edited gamemode to include the setting of teams `Content/Blueprints/BP_GameMode_Countdown.uasset`
- updated game state to allow team setting and storing between game state and player state `Content/Blueprints/BP_GameState.uasset`
- Integrate level stream feature with Checkpoint_SW. `Content/Levels/TestLevel_Buleprint/Checkpoint_SW.uasset`
- Updated player controller to read race standings info `Content/Blueprints/BP_PlayerController_Countdown.uasset`

### Removed
- Removed old mailbox mesh (Artists)
- Removed Cams asset folder (Artists)
- Removed Old asset folder (Artists)

### Fixed
- Fixed collision for broken road mesh (Artists)
- Fixed collision for grass mesh (Artists)
- updated character select to remove shooter pawns ability to create a session as they need the driver to exist `Content/Networking/W_Character_Select.uasset`
- fixed bug in player controller where the sim booleans wouldn't be updated `Content/Blueprints/BP_PlayerController_Countdown.uasset`


## [0.2.0] - 2026-02-19

### Added
- Added a spawned and parented shooter pawn to the vehicle pawn (currently unpossessed). Also set the shooter pawn to ignore collisions from vehicles and pawns.
- Added building assets for chaos destruction
- Added textures for the subway station
- Added lighting test levels


### Changed
- Updated hud game mode and default level to Level Design Master Level


### Fixed
- Driver and shooter parented and spawn corrected
- Fixed collisions for buildings


## [0.1.0] - 2026-02-19

### Added
- Initial project setup
- Vehicle template implementation
- FMOD Studio integration for audio
- Chaos Vehicles Plugin integration
- Networking framework setup
- UI framework initialization
- Character systems foundation

### Changed
- Configured Unreal Engine 5.6 project settings

### Security
- Integrated Steam Sockets for secure networking

---

## How to Update This Changelog

### Categories Explained:
- **Added**: New features or functionality
- **Changed**: Changes to existing functionality
- **Deprecated**: Features marked for future removal
- **Removed**: Features that have been removed
- **Fixed**: Bug fixes
- **Security**: Security-related changes

### Update Process for Every PR:

1. **Before creating your PR**, add your changes to the `[Unreleased]` section
2. **Be specific**: Include what changed, not just "Updated systems"
3. **Group by category**: List your changes under the appropriate heading
4. **Use clear language**: Write for both technical and non-technical readers

### Example PR Update:

If you fixed a bug in vehicle physics and added a new damage system:

```markdown
## [Unreleased]

### Added
- Vehicle damage system with visual feedback
- New particle effects for collision impacts

### Fixed
- Vehicle sliding on water surfaces issue
- Physics collision detection false positives
```

### Release Process (When Releasing a New Version):

1. Move the `[Unreleased]` section to a new version section with today's date
2. Use semantic versioning: `[X.Y.Z]` where:
   - **X** = Major version (breaking changes)
   - **Y** = Minor version (new features)
   - **Z** = Patch version (bug fixes)
3. Empty the `[Unreleased]` section for future changes

### Example Release:

```markdown
## [Unreleased]

### Added
- 

### Changed
- 

## [1.0.0] - 2026-03-15

### Added
- Vehicle damage system with visual feedback
- New particle effects for collision impacts

### Fixed
- Vehicle sliding on water surfaces issue
```

### Best Practices:

✅ **DO:**
- Update the changelog **in the same commit** as your changes
- Be descriptive but concise
- Keep entries organized by category

❌ **DON'T:**
- Leave it until the end of the release
- Use vague language like "Fixed stuff"
- Mix multiple unrelated changes in one entry
- Update the version number until release time

---

## Version History

Currently tracking versions starting from **0.1.0** (2026-02-19)

