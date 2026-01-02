# EditorExpose Macro System - Simplified Design

## Overview

The EditorExpose macro system has been completely redesigned to be as simple and clean as possible.

### The Key Innovation: Single `BEGIN_EXPOSABLE()` Block

**No more class name repetition. No more separate BEGIN/END pairs.**

```cpp
BEGIN_EXPOSABLE(MyComponent)
    // All fields and gizmos in one clean block
    EXPOSE_FIELD(m_vPosition, "Position", "...", .type = ...)
    EXPOSE_FIELD_SIMPLE(m_fHealth, "Health", EditorFieldWidgetType::EditorFieldWidgetType_Float)
    
    // Gizmos can be mixed seamlessly - no separator needed!
    EXPOSE_GIZMO(m_Transform, GizmoOptions{ .type = EditorGizmoType::EditorGizmoType_Transform })
END_EXPOSABLE(MyComponent)  // ← Pass the class name here
```

That's it! One block, no class name anywhere except `BEGIN_EXPOSABLE(ClassName)`.

---

## Core Macros

### `BEGIN_EXPOSABLE(CLASSNAME)` 
Starts the exposable block. Captures the class name once and makes it available to all field/gizmo macros.

**Usage:**
```cpp
class MyComponent : public Component {
public:
    BEGIN_EXPOSABLE(MyComponent)
        // Fields go here
        // Gizmos go here
    END_EXPOSABLE()
};
```

### `BEGIN_EXPOSABLE_PARENT(CLASSNAME, PARENT)`
Same as above but automatically inherits fields from parent class.

**Usage:**
```cpp
class MyDerivedComponent : public MyBaseComponent {
public:
    BEGIN_EXPOSABLE_PARENT(MyDerivedComponent, MyBaseComponent)
        // Additional fields for this class
        EXPOSE_FIELD(m_iExtraValue, "Extra", "...", .type = ...)
    END_EXPOSABLE()
};
```

### `EXPOSE_FIELD(VAR, "Name", "Description", .option = value, ...)`
Add a field with custom options.

```cpp
EXPOSE_FIELD(m_vPosition, "Position", "World position",
    .type = EditorFieldWidgetType::EditorFieldWidgetType_Vector3)

EXPOSE_FIELD(m_fSpeed, "Speed", "Movement speed",
    .type = EditorFieldWidgetType::EditorFieldWidgetType_Float,
    .min = "0", .max = "100", .step = 0.1f)
```

### `EXPOSE_FIELD_SIMPLE(VAR, "Name", TYPE)`
Quick way to add a field with just a type. No options needed.

```cpp
EXPOSE_FIELD_SIMPLE(m_iHealth, "Health", EditorFieldWidgetType::EditorFieldWidgetType_Int32)
EXPOSE_FIELD_SIMPLE(m_bEnabled, "Enabled", EditorFieldWidgetType::EditorFieldWidgetType_Bool)
```

### `END_EXPOSE_FIELDS()`
Separates the fields section from the gizmos section. Required if you have gizmos.

```cpp
BEGIN_EXPOSABLE(MyComponent)
    EXPOSE_FIELD(...)
    EXPOSE_FIELD_SIMPLE(...)
    END_EXPOSE_FIELDS()  // Transition to gizmos
    EXPOSE_GIZMO(...)
END_EXPOSABLE()
```

### `EXPOSE_GIZMO(VAR, GizmoOptions{ ... })`
Add a gizmo for visual manipulation in the editor. Can be mixed seamlessly with fields - no separator needed!

```cpp
EXPOSE_GIZMO(m_vPosition, GizmoOptions{ .type = EditorGizmoType::EditorGizmoType_Transform })
```

### `END_EXPOSABLE(CLASSNAME)`
Closes the entire exposable block and generates three required method overrides:
- `GetEditorFields()`
- `GetEditorGizmos()`
- `GetTypeName()`

Requires the class name parameter for the `GetTypeName()` stringification.

---

## Complete Examples

### Simple Data Structure

```cpp
class SpriteRect : public IExposableToEditor {
public:
    uint32_t x = 0;
    uint32_t y = 0;
    uint32_t width = 0;
    uint32_t height = 0;
    
    BEGIN_EXPOSABLE(SpriteRect)
        EXPOSE_FIELD_SIMPLE(x, "X", EditorFieldWidgetType::EditorFieldWidgetType_Int64)
        EXPOSE_FIELD_SIMPLE(y, "Y", EditorFieldWidgetType::EditorFieldWidgetType_Int64)
        EXPOSE_FIELD_SIMPLE(width, "Width", EditorFieldWidgetType::EditorFieldWidgetType_Int64)
        EXPOSE_FIELD_SIMPLE(height, "Height", EditorFieldWidgetType::EditorFieldWidgetType_Int64)
    END_EXPOSABLE(SpriteRect)
};
```

### Component with Parent

```cpp
class ColliderComponent : public Component {
private:
    glm::vec2 m_vOffset;
    glm::vec2 m_vSize;
    
public:
    BEGIN_EXPOSABLE_PARENT(ColliderComponent, Component)
        EXPOSE_FIELD(m_vOffset, "Offset", 
            "The local offset of the collider relative to the object's pivot.",
            .type = EditorFieldWidgetType::EditorFieldWidgetType_Vector2)
        EXPOSE_FIELD(m_vSize, "Size", 
            "The size of the collider in local space.",
            .type = EditorFieldWidgetType::EditorFieldWidgetType_Vector2)
    END_EXPOSABLE(ColliderComponent)
};
```

### With Gizmos

```cpp
class TransformComponent : public Component {
private:
    glm::vec3 m_vPosition;
    glm::quat m_qRotation;
    
public:
    BEGIN_EXPOSABLE_PARENT(TransformComponent, Component)
        EXPOSE_FIELD_SIMPLE(m_vPosition, "Position", EditorFieldWidgetType::EditorFieldWidgetType_Vector3)
        EXPOSE_FIELD_SIMPLE(m_qRotation, "Rotation", EditorFieldWidgetType::EditorFieldWidgetType_Quaternion)
        EXPOSE_GIZMO(m_vPosition, GizmoOptions{ .type = EditorGizmoType::EditorGizmoType_Transform })
    END_EXPOSABLE(TransformComponent)
};
```

### Complex Example

```cpp
class AnimationKeyFrameSpriteComponent : public AnimationKeyFrameComponentBase {
private:
    std::weak_ptr<Texture> m_pSprite;
    int8_t m_iSpriteIndex = 0;
    
public:
    BEGIN_EXPOSABLE_PARENT(AnimationKeyFrameSpriteComponent, AnimationKeyFrameComponentBase)
        EXPOSE_FIELD(m_pSprite, "Sprite", 
            "Texture asset for this sprite.",
            .type = EditorFieldWidgetType::EditorFieldWidgetType_EngineResource,
            .assetType = resources::AssetType::Sprite)
        EXPOSE_FIELD_SIMPLE(m_iSpriteIndex, "Sprite Index", EditorFieldWidgetType::EditorFieldWidgetType_Int8)
        EXPOSE_FIELD(m_pSprite, "Sprite Preview", "",
            .type = EditorFieldWidgetType::EditorFieldWidgetType_TexturePreview,
            .relatedIndexFieldOffset = offsetof(AnimationKeyFrameSpriteComponent, m_iSpriteIndex))
    END_EXPOSABLE(AnimationKeyFrameSpriteComponent)
};
```

---

## How It Works (The Implementation)

The system uses three typedefs created in one place and reused everywhere:

```cpp
#define BEGIN_EXPOSABLE(CLASSNAME) \
        public: \
            using _ExposableClass = CLASSNAME;   // ← For GetTypeName() and override methods \
            using _FieldClass = CLASSNAME;       // ← Used by EXPOSE_FIELD \
            using _GizmoClass = CLASSNAME;       // ← Used by EXPOSE_GIZMO \
            static const std::vector<EditorFieldInfo>& StaticEditorFields() { \
                static std::vector<EditorFieldInfo> fields = []() { \
                    std::vector<EditorFieldInfo> f;
```

Once these typedefs are in scope, every macro can access the class name via `_FieldClass` or `_GizmoClass`:

```cpp
#define EXPOSE_FIELD(VAR, UINAME, DESCRIPTION, ...) \
                    f.push_back({ #VAR, offsetof(_FieldClass, VAR), UINAME, DESCRIPTION, FieldOptions{ __VA_ARGS__ } });

#define EXPOSE_GIZMO(VAR, GIZMO_OPTIONS) \
                    g.push_back({ #VAR, offsetof(_GizmoClass, VAR), GIZMO_OPTIONS });
```

And `END_EXPOSABLE()` uses `_ExposableClass` to generate the overrides:

```cpp
#define END_EXPOSABLE() \
                    return g; \
                }(); \
                return gizmos; \
            } \
            const std::vector<EditorFieldInfo>& GetEditorFields() const override { return _ExposableClass::StaticEditorFields(); } \
            const std::vector<EditorGizmoInfo>& GetEditorGizmos() const override { return _ExposableClass::StaticEditorGizmos(); } \
            const char* GetTypeName() const override { return #_ExposableClass; }
```

---

## Migration from Old Syntax

### Old Way (v1)
```cpp
BEGIN_EXPOSE_FIELDS(MyComponent)
    EXPOSE_FIELD(MyComponent, m_var, "Name", "Desc", (FieldOptions{ ... }))
END_EXPOSE_FIELDS(MyComponent)
BEGIN_EXPOSE_GIZMOS(MyComponent)
    EXPOSE_GIZMO(MyComponent, m_transform, GizmoOptions{ ... })
END_EXPOSE_GIZMOS(MyComponent)
END_EXPOSE_TO_EDITOR(MyComponent)
```

### New Way (v2)
```cpp
BEGIN_EXPOSABLE(MyComponent)
    EXPOSE_FIELD(m_var, "Name", "Desc", .option = value)
    END_EXPOSE_FIELDS()
    EXPOSE_GIZMO(m_transform, GizmoOptions{ ... })
END_EXPOSABLE()
```

**Benefits:**
- ✅ Class name appears only once (in `BEGIN_EXPOSABLE`)
- ✅ No wrapper `(FieldOptions{ })` braces
- ✅ No class name in field/gizmo declarations
- ✅ Single unified block instead of three separate pairs
- ✅ Cleaner, more intuitive syntax

---

## Backward Compatibility

The old macros (`BEGIN_EXPOSE_FIELDS`, `BEGIN_EXPOSE_FIELDS_PARENT`, `BEGIN_EXPOSE_GIZMOS`, `END_EXPOSE_TO_EDITOR`) still exist and work exactly as before. You can migrate gradually - mix old and new syntax in different files.

---

## FAQ

**Q: Do I need both `END_EXPOSE_FIELDS()` and `END_EXPOSABLE()`?**
A: Yes. `END_EXPOSE_FIELDS()` separates fields from gizmos (required if you have both). `END_EXPOSABLE()` closes the entire block and generates the overrides.

**Q: What if I have no gizmos?**
A: You can skip `END_EXPOSE_FIELDS()` and go straight from fields to `END_EXPOSABLE()`. Actually, you can skip it entirely - just call `END_EXPOSABLE()` after your fields!

**Q: Can I mix old and new syntax?**
A: Yes, they're fully compatible. You can have some files using the old style and others using the new style.

**Q: Do I need the class name after `BEGIN_EXPOSABLE`?**
A: No! Just use `BEGIN_EXPOSABLE(ClassName)` - the class name is captured internally and you never need to type it again.

---

## Benefits Summary

| Aspect | Old (v1) | New (v2) |
|--------|----------|---------|
| Lines per simple component | 8-10 | 5-7 |
| Class name repetition | 3-4 times | 1 time |
| Visual clutter | High | Low |
| Macro blocks needed | 3 pairs | 1 pair |
| Learning curve | Moderate | Very simple |
| Backward compatible | N/A | ✅ Yes |

---

## Best Practices

1. **Use `EXPOSE_FIELD_SIMPLE` for simple fields** - cleaner and less verbose
2. **Group related fields together** - improves readability
3. **Put gizmos after `END_EXPOSE_FIELDS()`** - natural separation of concerns
4. **Use descriptive names and descriptions** - helps in the editor UI
5. **Inherit from parent classes when possible** - avoids duplicating base fields

---

## Summary

The new `BEGIN_EXPOSABLE()` system is a dramatic simplification:
- ✨ **Single block** instead of three macro pairs
- ✨ **No class name repetition** (appears only in `BEGIN_EXPOSABLE`)
- ✨ **Cleaner field options** (no wrapper braces)
- ✨ **Fully backward compatible** with the old syntax
- ✨ **Zero runtime overhead** (pure preprocessor trick with typedefs)
