#pragma once

#include <rapidjson/document.h>

// core includes
#include "core/Data.h"
#include "core/Observable.h"

// utils includes
#include "utils/file_abstractions.h"

namespace gallus
{
	namespace gameplay
	{
		//---------------------------------------------------------------------
		// Scene
		//---------------------------------------------------------------------
		class Scene
		{
		public:
			~Scene();

			bool LoadData();

			bool SaveData();

			void SetData(const core::Data& a_Data);

			const core::Data& GetData() const;
#ifdef _EDITOR
			const fs::path& GetScenePath() const;

			void SetScenePath(const fs::path& a_ScenePath);

			const core::Observable<bool>& IsDirty() const;

			void SetIsDirty(bool a_fIsDirty);
#endif
		private:
			core::Data m_Data;
#ifdef _EDITOR
			fs::path m_ScenePath;
			core::Observable<bool> m_fIsDirty;
#endif
		};
	}
}