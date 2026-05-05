#pragma once

#include "imgui_system/modals/BaseModal.h"

// external
#include <string>
#include <vector>
#include <functional>
#include <memory>

namespace gallus::graphics::imgui
{
	class ImGuiWindow;

	struct PromptButton
	{
		std::string m_sName;
		std::function<void()> m_Function;
	};

	class PromptModal : public BaseModal
	{
	public:
		/// <summary>
		/// Constructs a window.
		/// </summary>
		/// <param name="a_System">The ImGui system for rendering the view.</param>
		PromptModal(ImGuiSystem& a_System);

		/// <summary>
		/// Renders the element.
		/// </summary>
		void Render() override;

		/// <summary>
		/// Renders the modal.
		/// </summary>
		bool ModalBegin() override;

		/// <summary>
		/// Sets the data of the file picker modal.
		/// </summary>
		/// <param name="a_sQuestion">The question in the prompt.</param>
		/// <param name="a_vButtons">Vector of buttons.</param>
		void SetData(const std::string& a_sQuestion, const std::vector<PromptButton>& a_vButtons);
	private:
		std::string m_sQuestion;
		std::vector<PromptButton> m_vButtons;
	};
}