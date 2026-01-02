# Gallus 2D Engine - AI Coding Agent Instructions

## Project Overview

**Gallus** is a 3D/2D game engine built in C++20 with DirectX 12 rendering. Despite the "2D" repository name, the current branch (3d) implements a 3D architecture. The engine uses an Entity-Component-System (ECS) pattern and includes an integrated ImGui-based editor. Three build targets exist: `editor` (with ImGui), `game` (runtime), and `game_shared`.

**Key Structure:**
- `engine/` - Core engine library (static lib, compiled as both `engine` and `engine_noimgui`)
- `editor/` - ImGui editor executable
- `game/` / `game_shared/` - Game implementations
- `external/` - DirectX 12, GLM, ImGui, RapidJSON, tiny_gltf, stb_image

## Build & Development Workflow

### Initial Setup
```powershell
# Generate Visual Studio project (CMake outputs to ../ parent directory)
.\generate.bat          # Runs: cmake -G "Visual Studio 17 2022" -A x64 -S . -B ..
```

### Running from Repository
```powershell
# Build/Debug configurations for each target:
.\editor_debug.bat      # Runs editor with assetPath pointing to .\data\assets
.\editor_release.bat
.\game_debug.bat
.\game_release.bat
```

### Key CMake Patterns
- **Platform:** x64 Windows, Visual Studio 17 2022 only
- **Output:** Build artifacts go to `../` (parent of source directory)
- **Solution:** `../game.sln` contains all targets
- **Configurations:** Debug (`_DEBUG`) and Release (`NDEBUG`)
- **C++ Standard:** C++20, MSVC with `/ZI` debug info and `/MT` runtime linking
- **Preprocessor:** `_LOAD_BY_PATH` enables, `TINYGLTF_NO_STB_IMAGE` disables

## Architecture: Entity-Component-System

### Core ECS Structure
Located in `engine/src/gameplay/`:

1. **Entity** (`Entity.h`) - Lightweight wrapper with ID and name
2. **EntityID** - Unique identifier (used for serialization/references)
3. **EntityComponentSystem** (`EntityComponentSystem.h`) - Manages all gameplay entities and systems
4. **Component** (`systems/components/Component.h`) - Base class for all components
5. **ECSBaseSystem<T>** (`ECSBaseSystem.h`) - Templated system base managing components of type `T`

### Component Example Pattern
```cpp
// Components inherit from Component base
class SpriteComponent : public Component {
public:
    void SetDefaults(const gameplay::EntityID& a_EntityID) override;
    void UpdateRealtime(float a_fDeltaTime, UpdateTime a_UpdateTime) override;
};

// Systems manage one component type
class SpriteSystem : public ECSBaseSystem<SpriteComponent> {
    std::string GetPropertyName() const override;
    std::string GetSystemName() const override;
};
```

**Built-in Systems:** AnimationSystem, CollisionSystem, HealthSystem, MeshSystem, ProjectileSystem, RigidbodySystem, SpriteSystem, TransformSystem

### Resource Management Pattern
- `ResourceAtlas` (`resources/ResourceAtlas.h`) - Central registry for all resources
- **Memory Pattern:** `std::shared_ptr` for ownership, `std::weak_ptr` for references
  - Textures, Meshes, Shaders return `weak_ptr` (lifetime managed by ResourceAtlas)
  - PixelShader, VertexShader return `shared_ptr` (need explicit ownership)
- Resources loaded on-demand: `LoadTexture(name, commandQueue)`, `LoadMesh(name)`, `LoadShaderBind(name)`

## System Architecture

### Core System Hierarchy
`System` (base) → `ThreadedSystem` (optional threading) → Specialized systems

Located in `engine/src/core/System.h`:
- Override `Initialize()` and `Destroy()` 
- Check `Running()` atomic flag before operations
- ThreadedSystem provides `Loop()` for background threads

### Engine Initialization Pipeline
`Engine::Initialize()` (in `engine/src/core/Engine.h`) sets up:
1. Window (Win32)
2. DX12System (rendering)
3. InputSystem
4. ResourceAtlas
5. EntityComponentSystem

Access via: `Engine::GetDX12()`, `Engine::GetResourceAtlas()`, `Engine::GetInputSystem()`, `Engine::GetWindow()`

## Graphics: DirectX 12

Located in `engine/src/graphics/dx12/`:

### Key Classes
- **DX12System** - Manages devices, command queues, swap chains, rendering
- **Texture** - GPU textures (can be Texture2D or SpriteSheet by TextureType enum)
- **Mesh** - Vertex/index buffers
- **Shader** / **ShaderBind** - Pixel and vertex shaders with pipeline state objects
- **CommandQueue** / **CommandList** - Command buffer management
- **Camera** - Projection/view matrices (GLM-based)
- **Transform** - World matrix calculations

### Rendering Pattern
- `CommandQueue::ExecuteCommandList()` submits work
- Textures linked to Shaders via ShaderBind
- Sprite rendering uses SpriteComponent + DX12 texture sampling

## Editor-Specific Features

### Editor Exposure Macro System
Components/resources expose fields to the editor via macros (in `engine/src/editor/EditorExpose.h`):

```cpp
BEGIN_EXPOSE_FIELDS_PARENT(ClassName, ParentClass)
    EXPOSE_FIELD(ClassName, m_field, "Display Name", "Tooltip description", ...)
END_EXPOSE_FIELDS(ClassName)

BEGIN_EXPOSE_GIZMOS(ClassName)
    // Gizmo definitions here
END_EXPOSE_GIZMOS(ClassName)

END_EXPOSE_TO_EDITOR(ClassName)
```

These macros auto-generate editor UI for tweaking component properties at runtime.

### Editor Preprocessor Guards
- `_EDITOR` - Code only in editor build
- `_RENDER_TEX` - Render-to-texture in editor (debug visualization)
- Editor inherits all engine predefinitions + adds `IMGUI_DEFINE_MATH_OPERATORS`

## Code Style & Conventions

### Naming
- **Classes:** `PascalCase` (e.g., `SpriteComponent`, `DX12System`)
- **Methods:** `PascalCase`, virtual methods override with `override` keyword
- **Member Variables:** `m_prefix` + `camelCase` (e.g., `m_pMesh`, `m_vPosition`, `m_sName`, `m_bRunning`, `m_iIndex`)
  - Prefixes: `m_p` = pointer, `m_v` = vector/float, `m_s` = string, `m_b` = bool, `m_i` = int
- **Parameters:** `a_paramName` (leading `a_`)

### Header Guards & Documentation
- Always `#pragma once` (no `#ifndef` guards)
- XML `///` doc comments for public interfaces with `<summary>`, `<param>`, `<returns>` tags
- Include path structure mirrors namespace hierarchy

### Memory Management
- Prefer `weak_ptr` by default for references to atlas-managed resources
- Use `shared_ptr` only when explicit ownership is needed
- Check `weak_ptr::lock()` before use (returns `nullptr` if resource destroyed)
- No raw pointers to managed resources

## Data Persistence & Serialization

- **Format:** RapidJSON (in `external/rapidjson/`)
- **SrcData** (`resources/SrcData.h`) - JSON wrapper for serialization
- **Component::Serialize/Deserialize** - Override to handle component data persistence
- **Prefab** (`gameplay/Prefab.h`) - Template entities saved as JSON
- **Scene** (`gameplay/Scene.h`) - Collection of entities with serialization support

**Editor-only serialization:** Wrapped in `#ifdef _EDITOR` blocks

## Debugging & Known Issues

### Debug Helpers
- **ArgProcessor** (`core/ArgProcessor.h`) - Parse command-line arguments
- **Observable<T>** (`core/Observable.h`) - Reactive value wrapper with change events
- **Event** (`core/Event.h`) - Publish-subscribe event system
- **Logger** (`engine/src/logger/`) - Logging system

## External Dependencies

- **GLM** (`external/glm/`) - Math library (vectors, matrices)
- **ImGui** (`external/imgui/`) - UI toolkit with ImGuizmo, ImPlot extensions
- **RapidJSON** (`external/rapidjson/`) - JSON serialization
- **tiny_gltf** (`external/tiny_gltf/`) - glTF model loading
- **DirectX 12** (`external/dx12/`) - GPU API headers
- **stb_image** - Image loading (disabled via TINYGLTF_NO_STB_IMAGE)

## Adding New Components & Systems

### Adding a Component
1. Create `engine/src/gameplay/systems/components/MyComponent.h` inheriting `Component`
2. Implement `SetDefaults()`, `UpdateRealtime()`, `Serialize()`, `Deserialize()`
3. Use `BEGIN_EXPOSE_FIELDS_PARENT()` macro for editor properties
4. Ensure destructor is virtual

### Adding a System
1. Create `engine/src/gameplay/systems/MySystem.h` inheriting `ECSBaseSystem<MyComponent>`
2. Implement `Initialize()`, `GetPropertyName()`, `GetSystemName()`
3. Register in `EntityComponentSystem::Initialize()` via `AddSystem()`
4. System automatically called during ECS update loop

## Cross-Project Communication

- **Game ↔ Engine:** Game links `engine_noimgui` library (IMGUI_DISABLE preprocessor)
- **Editor ↔ Engine:** Editor links full `engine` library (with ImGui support)
- Shared code goes in `game_shared/` (linked by both game and editor)


### Modules
- `core/` - Core systems (Engine, System, ThreadedSystem, Logger, Event)
- `graphics/dx12/` - DirectX 12 rendering systems (DX12System, Texture, Mesh, Shader)
- `graphics/win32/` - Win32 windowing systems (Window, Win32Window)
- `graphics/imgui/` - ImGui integration (ImGuiLayer, ImGuizmoLayer)
- `gameplay/` - ECS systems (Entity, Component, EntityComponentSystem, various
  components/systems)
- `editor/` - ImGui editor code (EditorExpose macros, editor UI)
- `resources/` - Resource management (ResourceAtlas, SrcData)
- `external/` - Third-party libraries (GLM, ImGui, RapidJSON, tiny_gltf, stb_image)
- `game/` - Game-specific code (game logic, prefabs, scenes)
- `audio/` - Audio-specific code (audio logic, prefabs, scenes)
- `tools/` - Utility tools (asset converters, build scripts)
- `animation/` - Animation systems (skeletons, keyframes, animation playback)
- `input/` - Input handling systems (keyboard, mouse, gamepad)
- `logger/` - Logging systems (log levels, output formats)
- `utils/` - Utility functions (math helpers, string utilities)

### Variable Naming
*Member variables only* (prefix m_ + type indicator):
- int: m_iCount
- float: m_fHealth
- double: m_dHealth
- char: m_cKey
- bool: m_bKilled
- string: m_sName
- unsigned int/flags: m_uIndex, m_uFlags
- pointers: m_pEntity
- arrays/vectors: m_aComponents
- maps/dictionaries: m_mDictionary
- events/enums: m_eOnKilled
- function pointers: m_fnSetHealth
- structs: m_tStats
- id/guid: m_idEntity

*Local variables*:
- lowerCamelCase (e.g., currentHealth, isActive).
- Pointer locals: prefix with p (e.g., pEntity). Do not prefix other types.

### General Formatting
- Always use braces {} for if, else, for, while, switch, etc., even for single statements. Never use single-line conditionals.
- switch statements must always include a default case.
- No Unicode or special symbols in code/comments (plain ASCII only).
- Use `nullptr` instead of `NULL` or `0` for pointers.
- Use `override` keyword for all overridden virtual methods.
- Use `final` keyword for classes that should not be inherited.
- Use `= default` for default constructors/destructors where applicable.
- Use `= delete` for deleted constructors/operators where applicable.
- Use `constexpr` instead of defines for compile-time constants and functions where applicable.
- Use `static_cast`, `dynamic_cast`, `const_cast`, and `reinterpret_cast` instead of C-style casts.
- Use `emplace_back` instead of `push_back` where applicable.
- Use `std::make_shared` and `std::make_unique` for creating smart pointers.
- Use tabs for indentation.
- Line endings: LF (Unix style).
- Use 1 space after keywords (if, for, while, switch) before the opening parenthesis.
- Use 1 space on either side of binary operators (=, +, -, *, /, ==, !=, <, >, <=, >=, &&, ||).

#### Header Files (.h)
- Class description (/** @brief ... */) directly above the class definition.
- Every public/protected function must have full XML documentation comments.

#### Source Files (.cpp)
- First function of each class preceded by:
  
  //---------------------------------------------------------------------
  // ClassName
  //---------------------------------------------------------------------
  
- All subsequent functions of the same class use only:
  
  //---------------------------------------------------------------------
  

#### Include Order (in both .h and .cpp)
1. Inherited/base class headers first.
2. External includes (STL, third-party, system).
3. Project includes.