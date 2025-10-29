/*
 * DisplayHelper.h
 * Helper para abstrair chamadas repetitivas de renderização M5.Display
 * Reduz duplicação e facilita manutenção
 */

#ifndef DISPLAY_HELPER_H
#define DISPLAY_HELPER_H

#include <M5Unified.h>

// Estrutura de cores do tema
struct ThemeColors {
    uint32_t bg;        // Background
    uint32_t fg;        // Foreground/Text
    uint32_t header;    // Header bar
    uint32_t selected;  // Selected item
    uint32_t accent;    // Accent color
};

class DisplayHelper {
public:
    static constexpr int SCREEN_WIDTH = 240;
    static constexpr int SCREEN_HEIGHT = 135;
    static constexpr int HEADER_HEIGHT = 16;
    static constexpr int FOOTER_HEIGHT = 12;

    // Inicializa o display com configurações padrão
    static void init() {
        M5.Display.setRotation(1);
        M5.Display.setTextSize(1);
        M5.Display.setFont(&fonts::FreeMono9pt7b);
    }

    // Limpa a tela com cor de fundo
    static void clear(uint32_t color) {
        M5.Display.fillScreen(color);
    }

    // Desenha header com título
    static void drawHeader(const char* title, const ThemeColors& theme) {
        M5.Display.fillRect(0, 0, SCREEN_WIDTH, HEADER_HEIGHT, theme.header);
        M5.Display.setTextColor(theme.fg);
        M5.Display.setCursor(4, 4);
        M5.Display.print(title);
    }

    // Desenha footer com texto
    static void drawFooter(const char* text, const ThemeColors& theme) {
        M5.Display.fillRect(0, SCREEN_HEIGHT - FOOTER_HEIGHT, SCREEN_WIDTH, FOOTER_HEIGHT, theme.header);
        M5.Display.setTextColor(theme.fg);
        M5.Display.setCursor(4, SCREEN_HEIGHT - FOOTER_HEIGHT + 2);
        M5.Display.print(text);
    }

    // Desenha uma linha de menu
    static void drawMenuItem(int index, const char* text, bool selected, const ThemeColors& theme, int yOffset = HEADER_HEIGHT) {
        int y = yOffset + (index * 20);

        if (selected) {
            M5.Display.fillRect(0, y, SCREEN_WIDTH, 18, theme.selected);
            M5.Display.setTextColor(theme.bg);
        } else {
            M5.Display.setTextColor(theme.fg);
        }

        M5.Display.setCursor(8, y + 3);
        M5.Display.print(text);
    }

    // Desenha texto centralizado
    static void drawCenteredText(const char* text, int y, uint32_t color) {
        M5.Display.setTextColor(color);
        int textWidth = M5.Display.textWidth(text);
        int x = (SCREEN_WIDTH - textWidth) / 2;
        M5.Display.setCursor(x, y);
        M5.Display.print(text);
    }

    // Desenha texto em posição específica
    static void drawText(const char* text, int x, int y, uint32_t color) {
        M5.Display.setTextColor(color);
        M5.Display.setCursor(x, y);
        M5.Display.print(text);
    }

    // Desenha retângulo preenchido
    static void drawRect(int x, int y, int w, int h, uint32_t color) {
        M5.Display.fillRect(x, y, w, h, color);
    }

    // Desenha borda de retângulo
    static void drawBorder(int x, int y, int w, int h, uint32_t color) {
        M5.Display.drawRect(x, y, w, h, color);
    }

    // Desenha lista com scroll
    static void drawScrollableList(const char** items, int itemCount, int selectedIndex, int scrollOffset, const ThemeColors& theme) {
        int visibleItems = (SCREEN_HEIGHT - HEADER_HEIGHT - FOOTER_HEIGHT) / 20;

        for (int i = 0; i < visibleItems && (scrollOffset + i) < itemCount; i++) {
            int itemIndex = scrollOffset + i;
            drawMenuItem(i, items[itemIndex], itemIndex == selectedIndex, theme);
        }

        // Draw scrollbar if needed
        if (itemCount > visibleItems) {
            int scrollbarHeight = (visibleItems * (SCREEN_HEIGHT - HEADER_HEIGHT - FOOTER_HEIGHT)) / itemCount;
            int scrollbarY = HEADER_HEIGHT + (scrollOffset * (SCREEN_HEIGHT - HEADER_HEIGHT - FOOTER_HEIGHT)) / itemCount;
            M5.Display.fillRect(SCREEN_WIDTH - 4, scrollbarY, 4, scrollbarHeight, theme.accent);
        }
    }

    // Desenha barra de progresso
    static void drawProgressBar(int x, int y, int width, int height, float progress, uint32_t fillColor, uint32_t bgColor) {
        M5.Display.fillRect(x, y, width, height, bgColor);
        int fillWidth = (int)(width * progress);
        M5.Display.fillRect(x, y, fillWidth, height, fillColor);
        M5.Display.drawRect(x, y, width, height, TFT_WHITE);
    }

    // Desenha indicador de status (LED-like)
    static void drawStatusIndicator(int x, int y, int radius, uint32_t color, bool filled = true) {
        if (filled) {
            M5.Display.fillCircle(x, y, radius, color);
        }
        M5.Display.drawCircle(x, y, radius, TFT_WHITE);
    }

    // Desenha panel/card com borda
    static void drawPanel(int x, int y, int w, int h, const ThemeColors& theme) {
        M5.Display.fillRect(x, y, w, h, theme.bg);
        M5.Display.drawRect(x, y, w, h, theme.accent);
    }

    // Desenha separador horizontal
    static void drawSeparator(int y, const ThemeColors& theme) {
        M5.Display.drawLine(4, y, SCREEN_WIDTH - 4, y, theme.accent);
    }

    // Formata e desenha valor numérico
    static void drawValue(const char* label, int value, int x, int y, const ThemeColors& theme) {
        M5.Display.setTextColor(theme.fg);
        M5.Display.setCursor(x, y);
        M5.Display.print(label);
        M5.Display.print(": ");
        M5.Display.print(value);
    }

    // Formata e desenha valor string
    static void drawValue(const char* label, const char* value, int x, int y, const ThemeColors& theme) {
        M5.Display.setTextColor(theme.fg);
        M5.Display.setCursor(x, y);
        M5.Display.print(label);
        M5.Display.print(": ");
        M5.Display.print(value);
    }

    // Desenha badge/tag
    static void drawBadge(const char* text, int x, int y, uint32_t bgColor, uint32_t fgColor) {
        int padding = 4;
        int textW = M5.Display.textWidth(text);
        int badgeW = textW + (padding * 2);
        int badgeH = 14;

        M5.Display.fillRoundRect(x, y, badgeW, badgeH, 3, bgColor);
        M5.Display.setTextColor(fgColor);
        M5.Display.setCursor(x + padding, y + 2);
        M5.Display.print(text);
    }

    // Atualiza display
    static void refresh() {
        M5.Display.display();
    }
};

#endif // DISPLAY_HELPER_H
