#include "EditorGlobalFunctions.h"

// editor
#include "editor/core/EditorEngine.h"

namespace gallus
{
	namespace editor
	{
		void SaveScene()
		{
			if (!core::EDITOR_ENGINE->GetEditor().GetScene().IsValid())
			{
				fs::path scenePath;
				if (file::SaveFile(scenePath, {
					{ L"Scene Files (*.scene)", L"*.scene" },
					{ L"Prefab Files (*.prefab)", L"*.prefab" },
					}, core::EDITOR_ENGINE->GetResourceAtlas().GetResourceFolder().GetPath()))
				{
					if (scenePath.extension() == ".scene")
					{
						core::EDITOR_ENGINE->GetEditor().SetEditorMethod(editor::EditorMethod::EDITOR_METHOD_SCENE);
					}
					else if (scenePath.extension() == ".prefab")
					{
						core::EDITOR_ENGINE->GetEditor().SetEditorMethod(editor::EditorMethod::EDITOR_METHOD_PREFAB);
					}

					core::EDITOR_ENGINE->GetEditor().GetScene().Save(scenePath);
				}
			}
			else
			{
				core::EDITOR_ENGINE->GetEditor().GetScene().Save();
			}
		}
	}
}
