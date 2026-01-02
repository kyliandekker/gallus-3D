// Quick test example to verify the new macro syntax compiles correctly

class TestComponent : public Component {
private:
    glm::vec3 m_vPosition = glm::vec3(0);
    float m_fHealth = 100.0f;
    bool m_bEnabled = true;

public:
    BEGIN_EXPOSABLE(TestComponent)
        EXPOSE_FIELD(m_vPosition, "Position", "Object position in world space",
            .type = EditorFieldWidgetType::EditorFieldWidgetType_Vector3)
        EXPOSE_FIELD(m_fHealth, "Health", "Current health",
            .type = EditorFieldWidgetType::EditorFieldWidgetType_Float,
            .min = "0",
            .max = "100",
            .step = 1.0f)
        EXPOSE_FIELD_SIMPLE(m_bEnabled, "Enabled", EditorFieldWidgetType::EditorFieldWidgetType_Bool)
        END_EXPOSE_FIELDS()
    END_EXPOSABLE(TestComponent)
};

// Expected macro expansion (for reference):
// 
// class TestComponent : public Component {
// private:
//     glm::vec3 m_vPosition = glm::vec3(0);
//     float m_fHealth = 100.0f;
//     bool m_bEnabled = true;
// 
// public:
//     // From BEGIN_EXPOSABLE(TestComponent):
//     using _ExposableClass = TestComponent;
//     using _FieldClass = TestComponent;
//     using _GizmoClass = TestComponent;
//     static const std::vector<EditorFieldInfo>& StaticEditorFields() {
//         static std::vector<EditorFieldInfo> fields = []() {
//             std::vector<EditorFieldInfo> f;
// 
//             // EXPOSE_FIELD calls:
//             f.push_back({ "m_vPosition", offsetof(_FieldClass, m_vPosition), "Position", "Object position in world space", FieldOptions{ .type = ... } });
//             f.push_back({ "m_fHealth", offsetof(_FieldClass, m_fHealth), "Health", "Current health", FieldOptions{ .type = ..., .min = "0", .max = "100", .step = 1.0f } });
//             f.push_back({ "m_bEnabled", offsetof(_FieldClass, m_bEnabled), "Enabled", "", FieldOptions{ .type = ... } });
// 
//             // From END_EXPOSE_FIELDS():
//             return f;
//         }();
//         return fields;
//     }
//     static const std::vector<EditorGizmoInfo>& StaticEditorGizmos() {
//         static std::vector<EditorGizmoInfo> gizmos = []() {
//             std::vector<EditorGizmoInfo> g;
//             // (no gizmos in this example)
//             return g;
//         }();
//         return gizmos;
//     }
//     // From END_EXPOSABLE(TestComponent):
//     const std::vector<EditorFieldInfo>& GetEditorFields() const override { return _ExposableClass::StaticEditorFields(); }
//     const std::vector<EditorGizmoInfo>& GetEditorGizmos() const override { return _ExposableClass::StaticEditorGizmos(); }
//     const char* GetTypeName() const override { return "TestComponent"; }  // #CLASSNAME = "TestComponent"
// };
