/*
 * M5GOTCHI UNIVERSAL CONTROLS
 * Sistema de controles unificado para M5Stack e Cardputer
 * 
 * CARDPUTER:
 * ↑ (`) = ACTION_UP
 * ↓ (;) = ACTION_DOWN  
 * ← (,) = ACTION_LEFT
 * → (.) = ACTION_RIGHT
 * ⏎ = ACTION_SELECT
 * ESC (Del) = ACTION_BACK
 * 
 * M5STACK:
 * A = ACTION_BACK
 * B = ACTION_SELECT  
 * C = ACTION_RIGHT
 * A+C = ACTION_UP
 * B+C = ACTION_DOWN
 */

#ifndef M5GOTCHI_UNIVERSAL_CONTROLS_H
#define M5GOTCHI_UNIVERSAL_CONTROLS_H

// ==================== CONSTANTES DE AÇÃO ====================
#define ACTION_UP      0
#define ACTION_DOWN    1
#define ACTION_LEFT    2
#define ACTION_RIGHT   3
#define ACTION_SELECT  4
#define ACTION_BACK    5
#define ACTION_NONE    6

// ==================== INTERFACE UNIVERSAL ====================
class UniversalControlsInterface {
public:
    virtual void handleUniversalControls(int action) = 0;
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual void update() = 0;
    virtual void init() = 0;
};

#endif // M5GOTCHI_UNIVERSAL_CONTROLS_H