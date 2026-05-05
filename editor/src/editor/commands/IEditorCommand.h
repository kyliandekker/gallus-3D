#pragma once

namespace gallus::editor
{
	class IEditorCommand
	{
	public:
		virtual void Undo() = 0;
		virtual void Redo() = 0;
		virtual ~IEditorCommand() = default;
	};
}