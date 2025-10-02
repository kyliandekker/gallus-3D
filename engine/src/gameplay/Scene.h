#pragma once

// base class
#include "core/EngineResource.h"

// external
#include <rapidjson/document.h>
#include <string>

// core
#include "core/Data.h"
#include "core/Observable.h"

#define JSON_SCENE_ENTITIES_VAR "entities"
#define JSON_SCENE_ENTITIES_VAR_NAME "name"
#define JSON_SCENE_ENTITIES_VAR_ACTIVE "isActive"
#define JSON_SCENE_ENTITIES_VAR_COMPONENTS "components"

namespace gallus
{
	namespace gameplay
	{
		//---------------------------------------------------------------------
		// Scene
		//---------------------------------------------------------------------
		class Scene : public core::EngineResource
		{
		public:
			Scene() = default;

			/// <summary>
			/// Constructs a scene with a given name.
			/// </summary>
			/// <param name="a_sName">Name of the resource.</param>
			Scene(const std::string& a_sName) : core::EngineResource(a_sName)
			{}

			virtual bool LoadData();
#ifdef _EDITOR
			bool Save();
			bool Load();
#endif
			void SetData(const core::Data& a_Data);

			const core::Data& GetData() const;
			virtual const core::Data GetSceneData() const;
#ifdef _EDITOR
			const core::Observable<bool>& IsDirty() const;

			void SetIsDirty(bool a_fIsDirty);
#endif
			bool IsValid() const
			{
				return true;
			}
		protected:
			core::Data m_Data;
#ifdef _EDITOR
			core::Observable<bool> m_fIsDirty;
#endif
		};
	}
}