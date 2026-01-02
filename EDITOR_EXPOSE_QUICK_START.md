# EditorExpose - Quick Start Guide

## The Simplest Possible Example

```cpp
class MyComponent : public Component {
public:
    glm::vec3 m_vPosition;
    float m_fHealth;
    bool m_bEnabled;
    
    BEGIN_EXPOSABLE(MyComponent)
        EXPOSE_FIELD_SIMPLE(m_vPosition, "Position", EditorFieldWidgetType::EditorFieldWidgetType_Vector3)
        EXPOSE_FIELD_SIMPLE(m_fHealth, "Health", EditorFieldWidgetType::EditorFieldWidgetType_Float)
        EXPOSE_FIELD_SIMPLE(m_bEnabled, "Enabled", EditorFieldWidgetType::EditorFieldWidgetType_Bool)
    END_EXPOSABLE(MyComponent)
};
```

That's it! Three lines of macro code for three fields. The class name appears only in the start and end.

---

## With Options

```cpp
BEGIN_EXPOSABLE(MyComponent)
    EXPOSE_FIELD(m_fSpeed, "Speed", "Movement speed",
        .type = EditorFieldWidgetType::EditorFieldWidgetType_Float,
        .min = "0",
        .max = "100",
        .step = 0.1f)
END_EXPOSABLE(MyComponent)
```

---

## With Parent Class

```cpp
BEGIN_EXPOSABLE_PARENT(MyDerivedComponent, MyBaseComponent)
    EXPOSE_FIELD_SIMPLE(m_iExtraValue, "Extra", EditorFieldWidgetType::EditorFieldWidgetType_Int32)
END_EXPOSABLE(MyDerivedComponent)
```

---

## With Gizmos

```cpp
BEGIN_EXPOSABLE(MyComponent)
    EXPOSE_FIELD_SIMPLE(m_vPosition, "Position", EditorFieldWidgetType::EditorFieldWidgetType_Vector3)
    EXPOSE_GIZMO(m_vPosition, GizmoOptions{ .type = EditorGizmoType::EditorGizmoType_Transform })
END_EXPOSABLE(MyComponent)
```

---

## Key Points

- **No class name repetition** - use it only in `BEGIN_EXPOSABLE(ClassName)` and `END_EXPOSABLE(ClassName)`
- **`EXPOSE_FIELD_SIMPLE`** for fields with just a type
- **`EXPOSE_FIELD`** for fields with options (min, max, callbacks, etc.)
- **`EXPOSE_GIZMO`** can be mixed with fields - no separator needed!
- **`END_EXPOSABLE(ClassName)`** always closes the block with the class name

That's the entire system!
