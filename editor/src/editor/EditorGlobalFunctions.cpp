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
