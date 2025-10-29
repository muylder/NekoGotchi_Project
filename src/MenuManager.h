/*
 * MenuManager.h
 * Gerencia estrutura e navegação de menus
 * Elimina código duplicado de renderização de menu
 */

#ifndef MENU_MANAGER_H
#define MENU_MANAGER_H

#include <Arduino.h>
#include <vector>
#include <functional>
#include "DisplayHelper.h"
#include "StateManager.h"

// Item de menu com callback
struct MenuItem {
    String label;
    std::function<void()> action;
    bool enabled = true;
    String icon = "";  // Opcional: emoji ou símbolo

    MenuItem(const String& lbl, std::function<void()> act, bool en = true, const String& ic = "")
        : label(lbl), action(act), enabled(en), icon(ic) {}
};

// Menu com navegação
class Menu {
public:
    Menu(const String& title) : title(title) {}

    // Adiciona item ao menu
    void addItem(const String& label, std::function<void()> action, bool enabled = true, const String& icon = "") {
        items.emplace_back(label, action, enabled, icon);
    }

    // Adiciona separador
    void addSeparator() {
        items.emplace_back("---", nullptr, false);
    }

    // Navegação
    void moveUp() {
        if (selectedIndex > 0) {
            selectedIndex--;
            updateScroll();
        }
    }

    void moveDown() {
        if (selectedIndex < (int)items.size() - 1) {
            selectedIndex++;
            updateScroll();
        }
    }

    void selectCurrent() {
        if (selectedIndex >= 0 && selectedIndex < (int)items.size()) {
            MenuItem& item = items[selectedIndex];
            if (item.enabled && item.action) {
                item.action();
            }
        }
    }

    // Renderização
    void draw(const ThemeColors& theme) {
        // Header
        DisplayHelper::drawHeader(title.c_str(), theme);

        // Itens visíveis
        int contentHeight = DisplayHelper::SCREEN_HEIGHT - DisplayHelper::HEADER_HEIGHT - DisplayHelper::FOOTER_HEIGHT;
        int visibleItems = contentHeight / 20;

        for (int i = 0; i < visibleItems && (scrollOffset + i) < (int)items.size(); i++) {
            int itemIndex = scrollOffset + i;
            MenuItem& item = items[itemIndex];

            // Separador
            if (item.label == "---") {
                int y = DisplayHelper::HEADER_HEIGHT + (i * 20) + 10;
                DisplayHelper::drawSeparator(y, theme);
                continue;
            }

            // Item normal
            bool isSelected = (itemIndex == selectedIndex);
            int y = DisplayHelper::HEADER_HEIGHT + (i * 20);

            if (isSelected) {
                DisplayHelper::drawRect(0, y, DisplayHelper::SCREEN_WIDTH, 18, theme.selected);
                M5.Display.setTextColor(theme.bg);
            } else {
                M5.Display.setTextColor(item.enabled ? theme.fg : theme.accent);
            }

            // Ícone (se houver)
            int textX = 8;
            if (!item.icon.isEmpty()) {
                M5.Display.setCursor(textX, y + 3);
                M5.Display.print(item.icon);
                textX += 20;
            }

            // Label
            M5.Display.setCursor(textX, y + 3);
            M5.Display.print(item.label);

            // Indicador de desabilitado
            if (!item.enabled) {
                M5.Display.setCursor(DisplayHelper::SCREEN_WIDTH - 30, y + 3);
                M5.Display.print("✗");
            }
        }

        // Scrollbar
        if ((int)items.size() > visibleItems) {
            int scrollbarHeight = (visibleItems * contentHeight) / items.size();
            int scrollbarY = DisplayHelper::HEADER_HEIGHT + (scrollOffset * contentHeight) / items.size();
            DisplayHelper::drawRect(DisplayHelper::SCREEN_WIDTH - 4, scrollbarY, 4, scrollbarHeight, theme.accent);
        }

        // Footer com dicas
        DisplayHelper::drawFooter("↑/↓:Nav ENTER:Ok ESC:Back", theme);
    }

    // Reset
    void reset() {
        selectedIndex = 0;
        scrollOffset = 0;
    }

    // Getters
    int getSelectedIndex() const { return selectedIndex; }
    size_t getItemCount() const { return items.size(); }
    const String& getTitle() const { return title; }

private:
    String title;
    std::vector<MenuItem> items;
    int selectedIndex = 0;
    int scrollOffset = 0;

    void updateScroll() {
        int contentHeight = DisplayHelper::SCREEN_HEIGHT - DisplayHelper::HEADER_HEIGHT - DisplayHelper::FOOTER_HEIGHT;
        int visibleItems = contentHeight / 20;

        if (selectedIndex < scrollOffset) {
            scrollOffset = selectedIndex;
        } else if (selectedIndex >= scrollOffset + visibleItems) {
            scrollOffset = selectedIndex - visibleItems + 1;
        }
    }
};

// Gerenciador de pilha de menus para navegação hierárquica
class MenuManager {
public:
    static MenuManager& getInstance() {
        static MenuManager instance;
        return instance;
    }

    MenuManager(const MenuManager&) = delete;
    MenuManager& operator=(const MenuManager&) = delete;

    // Empilha novo menu
    void pushMenu(Menu* menu) {
        menuStack.push_back(menu);
        if (menu) {
            menu->reset();
        }
    }

    // Remove menu atual e volta ao anterior
    void popMenu() {
        if (!menuStack.empty()) {
            menuStack.pop_back();
        }
    }

    // Obtém menu atual
    Menu* getCurrentMenu() {
        return menuStack.empty() ? nullptr : menuStack.back();
    }

    // Limpa pilha
    void clear() {
        menuStack.clear();
    }

    // Verifica se há menus
    bool isEmpty() const {
        return menuStack.empty();
    }

    // Tamanho da pilha
    size_t getDepth() const {
        return menuStack.size();
    }

    // Input handling para menu atual
    void handleInput(char key) {
        Menu* current = getCurrentMenu();
        if (!current) return;

        switch(key) {
            case 'w':  // Up
            case ';':  // Up arrow
                current->moveUp();
                break;

            case 's':  // Down
            case '.':  // Down arrow
                current->moveDown();
                break;

            case '\n':  // Enter
            case '\r':
                current->selectCurrent();
                break;

            case 27:   // ESC
            case 'q':  // Q para quit/back
                if (getDepth() > 1) {
                    popMenu();
                } else {
                    // Volta ao dashboard ou modo anterior
                    STATE.changeMode(MODE_DASHBOARD);
                }
                break;
        }
    }

    // Desenha menu atual
    void draw(const ThemeColors& theme) {
        Menu* current = getCurrentMenu();
        if (current) {
            current->draw(theme);
        }
    }

private:
    MenuManager() = default;
    std::vector<Menu*> menuStack;
};

// Macro para acesso rápido
#define MENU_MGR MenuManager::getInstance()

#endif // MENU_MANAGER_H
