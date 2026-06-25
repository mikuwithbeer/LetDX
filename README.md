![Banner](.github/assets/banner.svg)

> [!WARNING]
> **This project is currently under active development!**
>
> Features, APIs, and architecture are subject to change without notice.

### Layout

Project is separated into two primary parts:

- **`LetDB`**: The core database server and storage engine.
- **`LetDD`**: The public-facing database driver that serves as a secure exposure layer.

### AI Usage

AI is used within the project but only in limited and clearly defined areas:

- **Documentation & Comments**: AI is used to help for documenting and to ensure comments use clear, proper English.
- **Code Generation**: We are **NOT** relying on AI to write the actual code or core logic. This is a deliberate choice
  to maintain code quality and demystify the architecture. We don't want to turn the codebase to unmaintainable slop.

### Building

#### Prerequisites

To build from source, make sure your environment meets the following requirements:

| Requirement | Version                                               | 
|-------------|-------------------------------------------------------|
| CMake       | **4.0** or newer                                      |
| C Compiler  | **C23** compatible (We prefer latest LLVM toolchains) |
| Go Compiler | **1.26** or newer                                     |

#### Steps

1. Clone the Repository:
   ```bash
   git clone https://github.com/mikuwithbeer/LetDX.git
   cd LetDX
   ```
2. Configure the Build:
   ```bash
   cmake -S . -B build -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release
   ```
3. Compile the Project(s):
   ```bash
   cmake --build build --config Release
   ```
   Upon a successful build, the compiled binaries will be located inside the `build/` directory.

### License

We use a dual-licensing model:

| Component | License                                      |
|-----------|----------------------------------------------|
| **LetDB** | BSD 3-Clause License[^1]                     |
| **LetDD** | European Union Public License (EUPL) 1.2[^2] |

[^1]: https://interoperable-europe.ec.europa.eu/licence/bsd-3-clause-new-or-revised-license

[^2]: https://interoperable-europe.ec.europa.eu/collection/eupl/eupl-text-eupl-12
