#include "./editorEntryPoints.h"

// external
#include <assert.h>

// core
#include "engineEntryPoints.h"
#include "core/Engine.h"

// logger
#include "logger/Logger.h"

// editor
#include "editor/core/EditorEngine.h"

// Set the engine ptr function.
static gallus::core::EditorEngine* s_Engine = nullptr;
void SetEnginePtr(gallus::core::EditorEngine* a_pPtr)
{
    s_Engine = a_pPtr;
}

namespace gallus
{
    logger::Logger& GetLogger()
    {
        logger::Logger* logger = GetEngine().GetLogger();
        if (!logger)
        {
            assert(false && "Logger was null.");
        }
    
    	return *logger;
    }
    resources::ResourceAtlas& GetResourceAtlas()
    {
        resources::ResourceAtlas* resourceAtlas = GetEngine().GetResourceAtlas();
        if (!resourceAtlas)
        {
            assert(false && "Resource atlas was null.");
        }
    
    	return *resourceAtlas;
    }
    //---------------------------------------------------------------------
    graphics::dx12::DX12System& GetDX12System()
    {
        graphics::dx12::DX12System* dx12System = GetEngine().GetDX12();
        if (!dx12System)
        {
            LOG(LogSeverity::LOGSEVERITY_ASSERT, LOG_CATEGORY_DX12, "Failed retrieving DX12 System: DX12 System was null.");
        }
    
        return *dx12System;
    }
    //---------------------------------------------------------------------
    core::Engine& GetEngine()
    {
        return *s_Engine;
    }
    
    //---------------------------------------------------------------------
    core::EditorEngine& GetEditorEngine()
    {
        return *s_Engine;
    }
}