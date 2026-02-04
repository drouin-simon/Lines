# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Lines is a minimalistic frame-by-frame 2D animation program with real-time network collaboration between animators. Written in C++17 using Qt 6 and OpenGL. Current version: 1.2 Beta4.

## Build Commands

```bash
# Configure (from a build directory outside src/)
mkdir build && cd build
cmake ../src

# Build
cmake --build .

# CMake options
cmake ../src -DLINES_USE_BOOST=ON      # Enable Boost shared_ptr (older compilers)
cmake ../src -DLINES_MASTER_BUILD=ON   # Enable master functionality
```

**Dependencies:** Qt 6.8.2+ (Widgets, OpenGL, OpenGLWidgets, Network), OpenGL, CMake 3.16+, Boost 1.63+ (optional).

**Bundled libraries:** SVL (vector/matrix math in `svl/`), GLee (OpenGL extensions in `GLee/`, non-macOS only).

No automated test suite, linter, or formatter is configured.

## Architecture

The codebase is split into four CMake libraries/targets:

- **linesCore** - Animation engine. Scene hierarchy: `Scene` -> `Frame` -> `Node` -> primitives (`Line`, `WideLine`). Contains drawing tools (`drwLineTool`, `drwToolbox`), OpenGL rendering (`drwGLRenderer`, `drwGlslShader`), and the main controller (`LinesCore`).

- **linesNetwork** - Real-time collaboration layer. Server/client architecture over TCP using Qt Network. `drwNetworkManager` orchestrates connections; commands are serialized and transmitted between peers.

- **linesDesktopLib** - Desktop UI library. OpenGL drawing canvas (`drwDrawingWidget`, `DrawingWidgetMT` for multithreaded rendering), toolbar widgets, parameter editors, and tablet input handling.

- **linesDesktop** - Application entry point. Main window, menus, file operations, export dialog, and preferences.

### Key Design Patterns

- **Command pattern** (`drwCommand` and subclasses) is central: all drawing actions are serializable commands. This enables both undo/redo and network synchronization. Commands are transmitted via `QDataStream`.

- **Thread safety**: `QReadWriteLock` on `Scene`/`Frame` for concurrent read access; `QMutex` on the network command queue in `LinesCore`.

### Naming Conventions

- Classes use `drw` prefix (drawing-related) or descriptive names
- Member variables: `m_camelCase`
- Methods: `camelCase`
- Macros in headers: `SetMacro`, `GetMacro`, `SharedPtrMacro` for boilerplate

### Platform Notes

- macOS: Uses native OpenGL headers, packages as DMG via `macdeployqt`
- Windows: Uses GLee for OpenGL extensions, packages via NSIS with `windeployqt`
- Linux: Uses GLee, packages as TGZ
