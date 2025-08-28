#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#pragma once

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			class EditorSelectable
			{
				virtual void RenderEditorSelectable()
				{

				}
			};
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE