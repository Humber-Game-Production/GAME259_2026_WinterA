# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added
- 

### Changed
-

### Deprecated
- 

### Removed
- 

### Fixed
- 

### Security
- 


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

