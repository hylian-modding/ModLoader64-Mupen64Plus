#pragma once

#include <string>

#include "imgui.h"
#include "imgui_internal.h"

namespace ImGuiUtil
{

namespace details
{

static float CalcMaxPopupHeightFromItemCount(int items_count)
{
	ImGuiContext& g = *GImGui;
	if (items_count <= 0) return FLT_MAX;
	return (g.FontSize + g.Style.ItemSpacing.y) * items_count - g.Style.ItemSpacing.y + (g.Style.WindowPadding.y * 2);
}

}

template <class T>
bool Combo(const char* label, int* currentItem, T begin, T end, int popupMaxHeightInItems = -1)
{
	ImGuiContext& g = *GImGui;

	if (popupMaxHeightInItems != -1 && !(g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasSizeConstraint)) {
		ImGui::SetNextWindowSizeConstraints(ImVec2(0, 0), ImVec2(FLT_MAX, details::CalcMaxPopupHeightFromItemCount(popupMaxHeightInItems)));
	}

	const std::string& currentItemText = *std::next(begin, *currentItem);
	bool changed{};

	if (ImGui::BeginCombo(label, currentItemText.c_str())) {
		std::size_t i{};
		for (auto it{ begin }; it != end; ++it, ++i) {
			ImGui::PushID(reinterpret_cast<void*>(i));

			bool selected = i == *currentItem;

			if (ImGui::Selectable(it->c_str(), &selected)) {
				changed = true;
				*currentItem = i;
			}

			if (selected) {
				ImGui::SetItemDefaultFocus();
			}

			ImGui::PopID();
		}

		ImGui::EndCombo();
	}

	return changed;
}

inline ImVec4 vec4FromRGBA(int r, int g, int b, int a = 255)
{
	return{ r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f };
}

}