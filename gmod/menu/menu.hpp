#pragma once

#include "../render.hpp"
#include "widgets.hpp"

namespace menu {
	bool MenuOpen();

	void render_menu();
	void InitializeMenu();

	void AddElementToCategory(EMenuCategory category, std::shared_ptr<MenuElement> element);
	decltype(MenuTabContent::elements)& GetElementsForCategory(EMenuCategory category);
}