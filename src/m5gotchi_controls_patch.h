/*
 * 🎮 M5GOTCHI UNIVERSAL CONTROLS PATCH
 * Adiciona handleUniversalControls a TODOS os módulos
 */

#ifndef M5GOTCHI_CONTROLS_PATCH_H
#define M5GOTCHI_CONTROLS_PATCH_H

// Macro para adicionar controles universais rapidamente
#define ADD_UNIVERSAL_CONTROLS(CLASS_NAME) \
    void handleUniversalControls(int action) { \
        switch (action) { \
            case ACTION_UP: \
                if (selectedItem > 0) selectedItem--; \
                draw(); \
                break; \
            case ACTION_DOWN: \
                selectedItem++; \
                draw(); \
                break; \
            case ACTION_LEFT: \
                if (currentPage > 0) currentPage--; \
                selectedItem = 0; \
                draw(); \
                break; \
            case ACTION_RIGHT: \
                currentPage++; \
                selectedItem = 0; \
                draw(); \
                break; \
            case ACTION_SELECT: \
                executeSelection(); \
                break; \
        } \
    } \
    virtual void executeSelection() { /* Override in each class */ } \
    virtual void draw() { /* Override in each class */ }

// Template padrão para controles universais
template<typename T>
class UniversalControlsMixin {
public:
    virtual void handleUniversalControls(int action) {
        switch (action) {
            case ACTION_UP:
                navigateUp();
                break;
            case ACTION_DOWN:
                navigateDown();
                break;
            case ACTION_LEFT:
                navigateLeft();
                break;
            case ACTION_RIGHT:
                navigateRight();
                break;
            case ACTION_SELECT:
                selectItem();
                break;
        }
    }
    
protected:
    virtual void navigateUp() = 0;
    virtual void navigateDown() = 0;
    virtual void navigateLeft() = 0;
    virtual void navigateRight() = 0;
    virtual void selectItem() = 0;
};

#endif // M5GOTCHI_CONTROLS_PATCH_H