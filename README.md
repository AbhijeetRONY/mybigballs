# mybigballs

[![Linux](https://img.shields.io/badge/Linux-FCC624?style=for-the-badge&logo=linux&logoColor=black)](https://www.linux.org/) [![macOS](https://img.shields.io/badge/mac%20os-000000?style=for-the-badge&logo=macos&logoColor=F0F0F0)](https://www.apple.com/macos) [![Windows](https://img.shields.io/badge/Windows-0078D6?style=for-the-badge&logo=windows&logoColor=white)](https://www.microsoft.com/windows) [![Web](https://img.shields.io/badge/Web-3498DB?style=for-the-badge&logo=WebAssembly&logoColor=white)](https://webassembly.org/)

An extreme, visually explosive vertical paddle arcade game where you catch and eat falling balls with insane screen shake, particle explosions, and mind-blowing animations. Built with raylib 5 and C++, featuring a vertical glowing paddle, combo system, health mechanics, and epic visual effects.

Eat green and blue balls to score, avoid yellow to preserve points, and never touch the red balls of death.

---

## Game Features

### Core Gameplay
- **Vertical Paddle Control**: Move left/right to catch falling balls with smooth interpolation
- **Four Ball Types** with unique effects:
  - GREEN (+) - Healthy, +100 points, builds combos
  - BLUE (*) - Bonus, +200 points, restore 1 HP
  - YELLOW (-) - Bad, -50 points, breaks combos
  - RED (X) - Death, instant HP loss with extreme shake

### Visual Effects
- EXTREME Screen Shake - 2.5x amplified camera trauma with rotation
- Particle Systems - 500+ particles with gravity physics
- Multi-Layer Glows - 5-layer paddle glow, 3-layer ball glows
- Energy Waves - Flowing effects along the paddle
- Motion Trails - Semi-transparent paddle copies
- Score Popups - Floating +/- indicators that scale and fade
- Animated Background - Color waves and pulsing grid
- Camera Effects - Zoom, rotation, and shake

### Game Systems
- **Health System**: 3 hearts with visual pulse animations
- **Combo System**: Chain green/blue balls for multipliers with gold text
- **Progressive Difficulty**: Spawn rate increases every 8 seconds
- **High Score Tracking**: Persistent best score with new record celebration
- **Smooth Animations**: Lerp interpolation for all movements

---

## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes.

### Prerequisites

Before you begin, ensure you have the following tools installed on your system:

-   **C++ Compiler**: [Clang](https://clang.llvm.org/) version 20 or newer.
-   **Project Manager**: [Leaf](https://github.com/vishal-ahirwar/leaf)

Leaf will automatically handle:
- Downloading and installing raylib
- Setting up the build environment
- Managing dependencies via Conan
- Building with CMake and Ninja
- Cross-platform compilation

### Building the Project

Follow these steps to build and run the project locally.

1.  **Clone the repository:**
    ```sh
    git clone <your-repository-url>
    cd mybigballs
    ```

2.  **Compile the code:**
    Run leaf to build the project. Leaf will automatically install raylib and all required dependencies.
    ```sh
    leaf compile
    ```

Leaf handles all platform-specific configurations automatically for Linux, macOS, Windows, and WebAssembly builds.

---

## Usage

After a successful build, you can run the application using leaf.

To run the application, execute the following command from the project root:
```sh
leaf run mybigballs
```

### Controls

**In-Game:**
- LEFT Arrow or A - Move paddle left
- RIGHT Arrow or D - Move paddle right
- ESC - Exit game

**Menus:**
- SPACE - Start game / Restart after game over
- ENTER - Restart after game over (alternative)

### Gameplay Tips

1. **Position Early** - Smooth interpolation means you need to predict ball trajectories
2. **Blue = Life** - Always prioritize blue balls for health restoration
3. **Build Combos** - Chain green/blue balls for "COMBO x2", "COMBO x3"
4. **Avoid Red at All Costs** - One hit = -1 HP with extreme screen shake
5. **Yellow Risk Assessment** - Worth taking if your score is high enough
6. **Use Full Screen Width** - Don't camp in the center
7. **Feel the Shake** - Shake intensity indicates ball impact severity

---

## Animation System

### Start Screen (3-second intro)
- 200 particles explode from center
- Title with 5-layer glow effect
- Pulsing "Press SPACE" text
- Camera zoom breathing effect
- Animated grid background

### During Gameplay
- **Score Text**: Scales to 1.5x and pulses when increased
- **Health Hearts**: Pulse animation when alive, gray when lost
- **Combo Display**: Scales and glows with gold multi-layer effect
- **Paddle**: 5 glow layers + 3 energy waves flowing downward
- **Balls**: 3 glow layers + rotating rings + pulse animation
- **Particles**: 60+ per ball with gravity physics
- **Screen Shake**: Varies by ball type (8x to 40x intensity)

### Score Popups
- Spawn at ball position
- Scale from 0 to 1 in 0.2 seconds
- Float upward for 2 seconds
- Color-matched to ball type
- Smooth alpha fade out

### Game Over Sequence
- 500 red particles explode screen-wide
- Extreme screen shake (40x intensity)
- Camera zooms out to 0.8x
- Camera rotates continuously (20 degrees per second)
- Black overlay fades in
- "GAME OVER" text scales in with 10-layer glow
- Pulsing text animations
- New high score celebration with gold pulse

---

## Toolchain

This project is configured with a specific set of modern C++ tools:

-   **Compiler**: `Clang C++20`
-   **Graphics**: `raylib 5` - Simple and easy-to-use game programming library
-   **Project Manager**: `Leaf` for template generation and project management, which handles dependencies (via Conan) and building (via CMake and Ninja) for you.
-   **Build System**: CMake + Ninja (managed by Leaf)
-   **Standard**: C++11 minimum


### Technical Specifications

**Performance:**
- Target FPS: 60
- Particle System: Up to 500 active particles
- Trail System: Up to 30 segments
- Delta time physics for frame independence

**Rendering Pipeline:**
1. Animated background (color wave)
2. Pulsing grid lines (50x50 cells)
3. Particle system with gravity
4. Paddle trails (motion blur)
5. Paddle (5 glow layers + energy waves)
6. Balls (3 glow layers + rotating rings)
7. Score popups (floating text)
8. UI elements (score, health, combo)
9. Overlays (start screen, game over)

**Math Systems:**
- Lerp interpolation for smooth movement
- Sin/Cos wave functions for animations
- Particle physics with gravity
- Distance-based collision detection
- Camera shake with random offsets and rotation

### Tested Platforms

This Project has been tested and is expected to work on the following platforms:
-   **Linux** (Ubuntu, Arch, etc.)
-   **macOS**
-   **Windows** (with MSVC or ClangCL)
-   **Web** (via Emscripten/WebAssembly)

All platforms are automatically supported by Leaf's build system.

---

## Scoring System

| Ball Type | Points | Effect | Spawn Rate |
|-----------|--------|--------|------------|
| Green (+) | +100   | Builds combo | 50% |
| Blue (*)  | +200   | +1 HP, builds combo | 15% |
| Yellow (-) | -50   | Breaks combo | 25% |
| Red (X)   | 0      | -1 HP, screen trauma | 10% |

**Combo Multiplier:**
- Consecutive green/blue catches increase combo count
- Visual "COMBO x2" feedback with gold glow
- Yellow or red balls break the combo chain
- 2-second window between catches to maintain combo

**Screen Shake Intensity:**
- Green: 8 units
- Yellow: 15 units
- Blue: 12 units
- Red: 40 units + camera rotation + particle spam

---

## Development

### Code Architecture

**Main Classes:**
- `Game` - Core game loop, state management, rendering
- `Ball` - Individual ball with type, physics, animation state
- `Particle` - Explosion particle with velocity and lifetime
- `PaddleTrail` - Motion trail effect segment
- `ScorePopup` - Floating score indicator

**Key Systems:**
- Object-oriented design with clean encapsulation
- Delta time for frame-independent physics
- Vector-based dynamic memory management
- Camera2D for advanced effects (zoom, rotation, shake)
- Efficient particle cleanup and trail management

### Adding New Features

1. **New Ball Type:**
   - Add enum to `BallType`
   - Implement spawn logic in `SpawnBall()`
   - Add eat behavior in `EatBall()`
   - Define visual properties

2. **New Animation:**
   - Use `Lerp()` for smooth interpolation
   - Use `sin()/cos()` for wave effects
   - Update in `Update()` loop
   - Render in `Draw()` method

3. **New Particle Effect:**
   - Call `CreateParticles()` with position, color, count
   - Particles auto-manage with gravity and fade

---

## Contributing

Contributions are what make the open-source community such an amazing place to learn, inspire, and create. Any contributions you make are **greatly appreciated**.

### How to Contribute

1. Fork the Project
2. Create your Feature Branch (`git checkout -b feature/AmazingFeature`)
3. Commit your Changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the Branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

### Contribution Ideas

- Sound effects and background music
- Gamepad/controller support
- Online leaderboard integration
- Save system for unlockables
- Power-ups (shield, magnet, slow-motion)
- Alternative themes/skins
- Boss battles or special events
- Mobile touch controls
- WebAssembly port improvements
- Unit tests for game logic

Please refer to the project's issue tracker and pull request guidelines.

---

## License

This project is licensed under the MIT License - see the `LICENSE.md` file for details.

---

## Acknowledgments

- **raylib** - Amazing game programming library (raylib.com)
- **Ramon Santamaria** (@raysan5) - Creator of raylib
- **Leaf Project Manager** - Simplifying C++ project management
- **Community Contributors** - Everyone who helps improve this project

---

## Links

- **raylib Documentation**: https://www.raylib.com/cheatsheet/cheatsheet.html
- **raylib Examples**: https://www.raylib.com/examples.html
- **Leaf Package Manager**: https://github.com/vishal-ahirwar/leaf
- **Report Issues**: [GitHub Issues](<your-repository-url>/issues)
- **Discussions**: [GitHub Discussions](<your-repository-url>/discussions)

---

**Built with passion and excessive particle effects**

Prepare for visual insanity.