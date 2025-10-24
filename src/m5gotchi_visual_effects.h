/*
 * M5GOTCHI VISUAL EFFECTS ENGINE
 * ✨ Kawaii Visual Effects & Particle System
 * 
 * FEATURES:
 * 🌈 Advanced Particle Effects System
 * 🎭 Animated ASCII Art Generator
 * 📺 Retro TV Static & Glitch Effects
 * 🎪 3D Wireframe Visualizations
 * 🌸 Kawaii Theme Animations
 * 💫 Real-time Visual Feedback
 * 🎨 Custom Graphics Engine
 * 🌟 Matrix Rain & Cyberpunk Effects
 */

#ifndef M5GOTCHI_VISUAL_EFFECTS_H
#define M5GOTCHI_VISUAL_EFFECTS_H

#include <M5Unified.h>
#include <vector>
#include <math.h>

// ==================== VISUAL EFFECTS ENUMS ====================
enum EffectType {
    EFFECT_PARTICLES,
    EFFECT_MATRIX_RAIN,
    EFFECT_TV_STATIC,
    EFFECT_GLITCH,
    EFFECT_WIREFRAME_3D,
    EFFECT_KAWAII_HEARTS,
    EFFECT_CYBER_GRID,
    EFFECT_NEON_PULSE,
    EFFECT_RAINBOW_WAVE,
    EFFECT_BINARY_FLOW,
    EFFECT_PLASMA,
    EFFECT_FIRE,
    EFFECT_WATER_RIPPLE,
    EFFECT_SNOW,
    EFFECT_STARS
};

enum ParticleType {
    PARTICLE_CIRCLE,
    PARTICLE_SQUARE,
    PARTICLE_TRIANGLE,
    PARTICLE_HEART,
    PARTICLE_STAR,
    PARTICLE_DIAMOND,
    PARTICLE_SAKURA,
    PARTICLE_BINARY,
    PARTICLE_SPARK
};

enum AnimationState {
    ANIM_IDLE,
    ANIM_ATTACK_START,
    ANIM_ATTACK_ACTIVE,
    ANIM_ATTACK_SUCCESS,
    ANIM_ATTACK_FAIL,
    ANIM_DISCOVERY,
    ANIM_CELEBRATION,
    ANIM_SLEEP
};

// ==================== VISUAL STRUCTS ====================
struct Particle {
    float x, y;           // Position
    float vx, vy;         // Velocity
    float life;           // Life remaining (0-1)
    float maxLife;        // Maximum life
    uint16_t color;       // Particle color
    ParticleType type;    // Particle shape
    float size;           // Particle size
    float rotation;       // Rotation angle
    float rotSpeed;       // Rotation speed
    bool gravity;         // Affected by gravity
    float bounce;         // Bounce factor
    uint8_t alpha;        // Transparency
};

struct MatrixChar {
    char character;
    int x, y;
    float speed;
    uint16_t color;
    uint8_t brightness;
    bool isLeading;
    float life;
};

struct GlitchBlock {
    int x, y, w, h;
    uint16_t* originalData;
    uint16_t* glitchedData;
    unsigned long duration;
    unsigned long startTime;
    int intensity;
};

struct WireframeVertex {
    float x, y, z;        // 3D coordinates
    int screenX, screenY; // 2D screen coordinates
    bool visible;
};

struct WireframeEdge {
    int vertex1, vertex2;
    uint16_t color;
    bool visible;
};

struct KawaiiElement {
    float x, y;
    float scale;
    float rotation;
    uint16_t color;
    String emoji;
    float bouncePhase;
    float pulsePhase;
    bool active;
};

// ==================== VISUAL EFFECTS CLASS ====================
class M5GotchiVisualEffects {
private:
    // Core System
    EffectType currentEffect;
    AnimationState currentAnimation;
    bool effectsEnabled;
    unsigned long lastUpdate;
    float globalTime;
    
    // Particle System
    std::vector<Particle> particles;
    int maxParticles;
    float gravity;
    bool particlePhysics;
    
    // Matrix Rain Effect
    std::vector<MatrixChar> matrixChars;
    int matrixColumns;
    unsigned long lastMatrixUpdate;
    String matrixCharSet;
    
    // Glitch Effects
    std::vector<GlitchBlock> glitchBlocks;
    int glitchIntensity;
    unsigned long lastGlitch;
    bool glitchActive;
    
    // 3D Wireframe
    std::vector<WireframeVertex> vertices;
    std::vector<WireframeEdge> edges;
    float cameraDistance;
    float rotationX, rotationY, rotationZ;
    float rotSpeedX, rotSpeedY, rotSpeedZ;
    
    // Kawaii Elements
    std::vector<KawaiiElement> kawaiiElements;
    unsigned long lastKawaiiUpdate;
    int kawaiiMood;
    
    // Screen Buffer
    uint16_t* screenBuffer;
    bool useDoubleBuffer;
    
    // Color Palettes
    uint16_t cyberpunkPalette[8];
    uint16_t kawaiPalette[8];
    uint16_t matrixPalette[4];
    uint16_t firePalette[6];
    uint16_t waterPalette[6];
    
    // Performance
    int frameRate;
    unsigned long frameTime;
    bool adaptiveQuality;

public:
    M5GotchiVisualEffects() {
        currentEffect = EFFECT_PARTICLES;
        currentAnimation = ANIM_IDLE;
        effectsEnabled = true;
        lastUpdate = 0;
        globalTime = 0;
        
        maxParticles = 50;
        gravity = 0.1f;
        particlePhysics = true;
        
        matrixColumns = 40;
        lastMatrixUpdate = 0;
        matrixCharSet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789@#$%^&*()";
        
        glitchIntensity = 3;
        lastGlitch = 0;
        glitchActive = false;
        
        cameraDistance = 100;
        rotationX = rotationY = rotationZ = 0;
        rotSpeedX = 0.02f;
        rotSpeedY = 0.015f;
        rotSpeedZ = 0.01f;
        
        lastKawaiiUpdate = 0;
        kawaiiMood = 0;
        
        screenBuffer = nullptr;
        useDoubleBuffer = false;
        
        frameRate = 30;
        frameTime = 0;
        adaptiveQuality = true;
        
        setupColorPalettes();
    }
    
    // ==================== CORE FUNCTIONS ====================
    void init() {
        Serial.println("✨ Initializing Visual Effects Engine...");
        
        // Initialize screen buffer if needed
        if (useDoubleBuffer) {
            screenBuffer = (uint16_t*)malloc(240 * 135 * 2);
            if (!screenBuffer) {
                Serial.println("⚠️ Failed to allocate screen buffer");
                useDoubleBuffer = false;
            }
        }
        
        // Initialize particle system
        particles.reserve(maxParticles);
        
        // Initialize matrix rain
        matrixChars.reserve(matrixColumns * 10);
        initMatrixRain();
        
        // Initialize wireframe cube
        initWireframeCube();
        
        // Initialize kawaii elements
        initKawaiiElements();
        
        Serial.println("✅ Visual Effects Ready!");
        Serial.printf("🎨 Max Particles: %d\n", maxParticles);
        Serial.printf("📺 Matrix Columns: %d\n", matrixColumns);
        Serial.printf("🎪 3D Wireframe: Enabled\n");
        Serial.printf("💖 Kawaii Elements: Loaded\n");
    }
    
    void start() {
        effectsEnabled = true;
        currentAnimation = ANIM_IDLE;
        startEffect(EFFECT_PARTICLES);
    }
    
    void stop() {
        effectsEnabled = false;
        clearAllEffects();
    }
    
    void update() {
        if (!effectsEnabled) return;
        
        unsigned long now = millis();
        float deltaTime = (now - lastUpdate) / 1000.0f;
        globalTime += deltaTime;
        
        // Update current effect
        switch (currentEffect) {
            case EFFECT_PARTICLES:
                updateParticles(deltaTime);
                break;
            case EFFECT_MATRIX_RAIN:
                updateMatrixRain(deltaTime);
                break;
            case EFFECT_TV_STATIC:
                updateTVStatic();
                break;
            case EFFECT_GLITCH:
                updateGlitchEffect();
                break;
            case EFFECT_WIREFRAME_3D:
                updateWireframe3D(deltaTime);
                break;
            case EFFECT_KAWAII_HEARTS:
                updateKawaiiHearts(deltaTime);
                break;
            case EFFECT_CYBER_GRID:
                updateCyberGrid(deltaTime);
                break;
            case EFFECT_NEON_PULSE:
                updateNeonPulse(deltaTime);
                break;
            case EFFECT_RAINBOW_WAVE:
                updateRainbowWave(deltaTime);
                break;
            case EFFECT_BINARY_FLOW:
                updateBinaryFlow(deltaTime);
                break;
            case EFFECT_PLASMA:
                updatePlasma(deltaTime);
                break;
            case EFFECT_FIRE:
                updateFire(deltaTime);
                break;
            case EFFECT_WATER_RIPPLE:
                updateWaterRipple(deltaTime);
                break;
            case EFFECT_SNOW:
                updateSnow(deltaTime);
                break;
            case EFFECT_STARS:
                updateStars(deltaTime);
                break;
        }
        
        // Performance monitoring
        frameTime = millis() - now;
        if (adaptiveQuality && frameTime > 33) { // Over 30 FPS
            adaptQuality();
        }
        
        lastUpdate = now;
    }
    
    void handleKeyboard(String key, M5Cardputer::Keyboard_Class::KeysState status) {
        if (key == "1") switchEffect();
        if (key == "2") togglePhysics();
        if (key == "3") addParticleBurst();
        if (key == "4") triggerGlitch();
        if (key == "5") changeColorPalette();
        if (key == "6") toggleDoubleBuffer();
        if (key == "+") increaseParticles();
        if (key == "-") decreaseParticles();
        if (status.enter) toggleEffects();
    }
    
    // ==================== PARTICLE SYSTEM ====================
    void updateParticles(float deltaTime) {
        // Update existing particles
        for (int i = particles.size() - 1; i >= 0; i--) {
            Particle& p = particles[i];
            
            // Update position
            p.x += p.vx * deltaTime * 60; // Scale by 60 for smooth movement
            p.y += p.vy * deltaTime * 60;
            
            // Apply gravity if enabled
            if (p.gravity && particlePhysics) {
                p.vy += gravity;
            }
            
            // Update rotation
            p.rotation += p.rotSpeed * deltaTime;
            
            // Update life
            p.life -= deltaTime;
            
            // Screen boundaries with bounce
            if (p.x < 0 || p.x > 240) {
                if (p.bounce > 0) {
                    p.vx *= -p.bounce;
                    p.x = constrain(p.x, 0, 240);
                } else {
                    particles.erase(particles.begin() + i);
                    continue;
                }
            }
            
            if (p.y < 0 || p.y > 135) {
                if (p.bounce > 0) {
                    p.vy *= -p.bounce;
                    p.y = constrain(p.y, 0, 135);
                } else {
                    particles.erase(particles.begin() + i);
                    continue;
                }
            }
            
            // Remove dead particles
            if (p.life <= 0) {
                particles.erase(particles.begin() + i);
                continue;
            }
            
            // Update alpha based on life
            p.alpha = (uint8_t)(255 * (p.life / p.maxLife));
        }
        
        // Add new particles occasionally
        if (particles.size() < maxParticles && random(0, 100) < 5) {
            addRandomParticle();
        }
        
        // Draw particles
        drawParticles();
    }
    
    void addRandomParticle() {
        if (particles.size() >= maxParticles) return;
        
        Particle p;
        p.x = random(0, 240);
        p.y = random(0, 135);
        p.vx = random(-50, 50) / 10.0f;
        p.vy = random(-50, 50) / 10.0f;
        p.life = p.maxLife = random(100, 500) / 100.0f;
        p.color = kawaiPalette[random(0, 8)];
        p.type = (ParticleType)random(0, 9);
        p.size = random(2, 8);
        p.rotation = 0;
        p.rotSpeed = random(-314, 314) / 100.0f; // -PI to PI
        p.gravity = (random(0, 2) == 1);
        p.bounce = random(0, 80) / 100.0f;
        p.alpha = 255;
        
        particles.push_back(p);
    }
    
    void addParticleBurst() {
        int burstCount = 15;
        int centerX = 120;
        int centerY = 67;
        
        for (int i = 0; i < burstCount; i++) {
            if (particles.size() >= maxParticles) break;
            
            float angle = (2 * PI * i) / burstCount;
            float speed = random(30, 80) / 10.0f;
            
            Particle p;
            p.x = centerX;
            p.y = centerY;
            p.vx = cos(angle) * speed;
            p.vy = sin(angle) * speed;
            p.life = p.maxLife = random(150, 300) / 100.0f;
            p.color = cyberpunkPalette[random(0, 8)];
            p.type = PARTICLE_SPARK;
            p.size = random(3, 6);
            p.rotation = angle;
            p.rotSpeed = random(-200, 200) / 100.0f;
            p.gravity = true;
            p.bounce = 0.3f;
            p.alpha = 255;
            
            particles.push_back(p);
        }
    }
    
    void drawParticles() {
        for (const Particle& p : particles) {
            if (p.alpha < 10) continue; // Skip nearly invisible particles
            
            // Set color with alpha (simplified)
            uint16_t color = p.color;
            if (p.alpha < 255) {
                // Simple alpha blending approximation
                int r = ((color >> 11) & 0x1F) * p.alpha / 255;
                int g = ((color >> 5) & 0x3F) * p.alpha / 255;
                int b = (color & 0x1F) * p.alpha / 255;
                color = (r << 11) | (g << 5) | b;
            }
            
            drawParticle(p, color);
        }
    }
    
    void drawParticle(const Particle& p, uint16_t color) {
        int x = (int)p.x;
        int y = (int)p.y;
        int size = (int)p.size;
        
        switch (p.type) {
            case PARTICLE_CIRCLE:
                M5.Display.fillCircle(x, y, size/2, color);
                break;
                
            case PARTICLE_SQUARE:
                M5.Display.fillRect(x - size/2, y - size/2, size, size, color);
                break;
                
            case PARTICLE_TRIANGLE:
                drawTriangle(x, y, size, p.rotation, color);
                break;
                
            case PARTICLE_HEART:
                drawHeart(x, y, size, color);
                break;
                
            case PARTICLE_STAR:
                drawStar(x, y, size, p.rotation, color);
                break;
                
            case PARTICLE_DIAMOND:
                drawDiamond(x, y, size, p.rotation, color);
                break;
                
            case PARTICLE_SAKURA:
                drawSakura(x, y, size, p.rotation, color);
                break;
                
            case PARTICLE_BINARY:
                drawBinaryDigit(x, y, size, color);
                break;
                
            case PARTICLE_SPARK:
                drawSpark(x, y, size, p.rotation, color);
                break;
        }
    }
    
    // ==================== MATRIX RAIN EFFECT ====================
    void initMatrixRain() {
        // Initialize matrix columns
        for (int col = 0; col < matrixColumns; col++) {
            MatrixChar mc;
            mc.x = col * (240 / matrixColumns);
            mc.y = random(-100, 0);
            mc.character = matrixCharSet[random(0, matrixCharSet.length())];
            mc.speed = random(20, 60) / 10.0f;
            mc.color = matrixPalette[random(0, 4)];
            mc.brightness = random(50, 255);
            mc.isLeading = (random(0, 10) == 0);
            mc.life = 1.0f;
            
            matrixChars.push_back(mc);
        }
    }
    
    void updateMatrixRain(float deltaTime) {
        for (MatrixChar& mc : matrixChars) {
            // Move character down
            mc.y += mc.speed * deltaTime * 60;
            
            // Update character occasionally
            if (random(0, 1000) < 5) {
                mc.character = matrixCharSet[random(0, matrixCharSet.length())];
            }
            
            // Fade trailing characters
            if (!mc.isLeading) {
                mc.brightness = max(mc.brightness - 2, 20);
            }
            
            // Reset at bottom
            if (mc.y > 135) {
                mc.y = random(-50, -10);
                mc.character = matrixCharSet[random(0, matrixCharSet.length())];
                mc.brightness = random(50, 255);
                mc.isLeading = (random(0, 10) == 0);
            }
        }
        
        drawMatrixRain();
    }
    
    void drawMatrixRain() {
        // Dark background with fade
        M5.Display.fillScreen(BLACK);
        
        for (const MatrixChar& mc : matrixChars) {
            if (mc.y < 0 || mc.y > 135) continue;
            
            // Create color with brightness
            uint16_t color = mc.color;
            if (mc.brightness < 255) {
                int r = ((color >> 11) & 0x1F) * mc.brightness / 255;
                int g = ((color >> 5) & 0x3F) * mc.brightness / 255;
                int b = (color & 0x1F) * mc.brightness / 255;
                color = (r << 11) | (g << 5) | b;
            }
            
            M5.Display.setTextColor(color);
            M5.Display.setCursor(mc.x, mc.y);
            M5.Display.print(mc.character);
        }
    }
    
    // ==================== GLITCH EFFECTS ====================
    void updateGlitchEffect() {
        if (!glitchActive && random(0, 1000) < 10) {
            triggerGlitch();
        }
        
        // Update active glitch blocks
        unsigned long now = millis();
        for (int i = glitchBlocks.size() - 1; i >= 0; i--) {
            GlitchBlock& block = glitchBlocks[i];
            
            if (now - block.startTime > block.duration) {
                // Remove expired glitch
                if (block.originalData) free(block.originalData);
                if (block.glitchedData) free(block.glitchedData);
                glitchBlocks.erase(glitchBlocks.begin() + i);
            }
        }
        
        if (glitchBlocks.empty()) {
            glitchActive = false;
        }
        
        drawGlitchEffect();
    }
    
    void triggerGlitch() {
        int blockCount = random(1, 5);
        
        for (int i = 0; i < blockCount; i++) {
            GlitchBlock block;
            block.x = random(0, 200);
            block.y = random(0, 100);
            block.w = random(20, 60);
            block.h = random(10, 30);
            block.intensity = glitchIntensity;
            block.duration = random(100, 500);
            block.startTime = millis();
            
            // Capture original area (simplified)
            block.originalData = nullptr; // Would capture screen data
            block.glitchedData = nullptr; // Would generate glitched version
            
            glitchBlocks.push_back(block);
        }
        
        glitchActive = true;
    }
    
    void drawGlitchEffect() {
        for (const GlitchBlock& block : glitchBlocks) {
            // Simple glitch effect - draw random colored rectangles
            for (int i = 0; i < block.intensity; i++) {
                int x = block.x + random(-5, block.w + 5);
                int y = block.y + random(-2, block.h + 2);
                int w = random(2, 15);
                int h = random(1, 5);
                
                uint16_t color = cyberpunkPalette[random(0, 8)];
                M5.Display.fillRect(x, y, w, h, color);
            }
        }
    }
    
    // ==================== 3D WIREFRAME ====================
    void initWireframeCube() {
        // Define cube vertices
        vertices.clear();
        vertices.push_back({-20, -20, -20, 0, 0, true}); // 0
        vertices.push_back({ 20, -20, -20, 0, 0, true}); // 1
        vertices.push_back({ 20,  20, -20, 0, 0, true}); // 2
        vertices.push_back({-20,  20, -20, 0, 0, true}); // 3
        vertices.push_back({-20, -20,  20, 0, 0, true}); // 4
        vertices.push_back({ 20, -20,  20, 0, 0, true}); // 5
        vertices.push_back({ 20,  20,  20, 0, 0, true}); // 6
        vertices.push_back({-20,  20,  20, 0, 0, true}); // 7
        
        // Define cube edges
        edges.clear();
        // Front face
        edges.push_back({0, 1, CYAN, true});
        edges.push_back({1, 2, CYAN, true});
        edges.push_back({2, 3, CYAN, true});
        edges.push_back({3, 0, CYAN, true});
        // Back face
        edges.push_back({4, 5, MAGENTA, true});
        edges.push_back({5, 6, MAGENTA, true});
        edges.push_back({6, 7, MAGENTA, true});
        edges.push_back({7, 4, MAGENTA, true});
        // Connecting edges
        edges.push_back({0, 4, YELLOW, true});
        edges.push_back({1, 5, YELLOW, true});
        edges.push_back({2, 6, YELLOW, true});
        edges.push_back({3, 7, YELLOW, true});
    }
    
    void updateWireframe3D(float deltaTime) {
        // Update rotation
        rotationX += rotSpeedX;
        rotationY += rotSpeedY;
        rotationZ += rotSpeedZ;
        
        // Transform vertices
        for (WireframeVertex& v : vertices) {
            // Apply rotation matrices
            float x = v.x, y = v.y, z = v.z;
            
            // Rotate around X axis
            float y1 = y * cos(rotationX) - z * sin(rotationX);
            float z1 = y * sin(rotationX) + z * cos(rotationX);
            
            // Rotate around Y axis  
            float x2 = x * cos(rotationY) + z1 * sin(rotationY);
            float z2 = -x * sin(rotationY) + z1 * cos(rotationY);
            
            // Rotate around Z axis
            float x3 = x2 * cos(rotationZ) - y1 * sin(rotationZ);
            float y3 = x2 * sin(rotationZ) + y1 * cos(rotationZ);
            
            // Project to 2D (simple perspective)
            float distance = cameraDistance + z2;
            if (distance > 0) {
                v.screenX = 120 + (int)(x3 * 100 / distance);
                v.screenY = 67 + (int)(y3 * 100 / distance);
                v.visible = true;
            } else {
                v.visible = false;
            }
        }
        
        drawWireframe3D();
    }
    
    void drawWireframe3D() {
        // Clear background
        M5.Display.fillScreen(BLACK);
        
        // Draw edges
        for (const WireframeEdge& edge : edges) {
            if (!vertices[edge.vertex1].visible || !vertices[edge.vertex2].visible) continue;
            
            int x1 = vertices[edge.vertex1].screenX;
            int y1 = vertices[edge.vertex1].screenY;
            int x2 = vertices[edge.vertex2].screenX;
            int y2 = vertices[edge.vertex2].screenY;
            
            // Check if line is on screen
            if (x1 < 0 && x2 < 0) continue;
            if (x1 > 240 && x2 > 240) continue;
            if (y1 < 0 && y2 < 0) continue;
            if (y1 > 135 && y2 > 135) continue;
            
            M5.Display.drawLine(x1, y1, x2, y2, edge.color);
        }
        
        // Draw vertices as dots
        for (const WireframeVertex& v : vertices) {
            if (!v.visible) continue;
            if (v.screenX < 0 || v.screenX > 240 || v.screenY < 0 || v.screenY > 135) continue;
            
            M5.Display.fillCircle(v.screenX, v.screenY, 2, WHITE);
        }
    }
    
    // ==================== KAWAII EFFECTS ====================
    void initKawaiiElements() {
        kawaiiElements.clear();
        
        // Add some cute elements
        for (int i = 0; i < 8; i++) {
            KawaiiElement element;
            element.x = random(20, 220);
            element.y = random(20, 115);
            element.scale = random(50, 150) / 100.0f;
            element.rotation = 0;
            element.color = kawaiPalette[random(0, 8)];
            element.bouncePhase = random(0, 628) / 100.0f; // 0 to 2PI
            element.pulsePhase = random(0, 628) / 100.0f;
            element.active = true;
            
            String emojis[] = {"♥", "★", "◆", "♪", "☆", "♡", "✿", "◇"};
            element.emoji = emojis[i % 8];
            
            kawaiiElements.push_back(element);
        }
    }
    
    void updateKawaiiHearts(float deltaTime) {
        for (KawaiiElement& element : kawaiiElements) {
            if (!element.active) continue;
            
            // Update animation phases
            element.bouncePhase += deltaTime * 3; // Bounce frequency
            element.pulsePhase += deltaTime * 2;  // Pulse frequency
            
            // Bounce effect
            element.y += sin(element.bouncePhase) * 0.5f;
            
            // Pulse effect on scale
            element.scale = 0.8f + sin(element.pulsePhase) * 0.3f;
            
            // Color cycling
            float colorPhase = element.pulsePhase / 2;
            int colorIndex = ((int)(colorPhase * 4)) % 8;
            element.color = kawaiPalette[colorIndex];
        }
        
        drawKawaiiElements();
    }
    
    void drawKawaiiElements() {
        M5.Display.fillScreen(BLACK);
        
        for (const KawaiiElement& element : kawaiiElements) {
            if (!element.active) continue;
            
            int size = (int)(element.scale * 12);
            drawKawaiiShape(element.x, element.y, size, element.color, element.emoji);
        }
    }
    
    void drawKawaiiShape(float x, float y, int size, uint16_t color, String shape) {
        int ix = (int)x;
        int iy = (int)y;
        
        if (shape == "♥") {
            drawHeart(ix, iy, size, color);
        } else if (shape == "★") {
            drawStar(ix, iy, size, 0, color);
        } else if (shape == "◆") {
            drawDiamond(ix, iy, size, 0, color);
        } else {
            // Default: draw as text
            M5.Display.setTextColor(color);
            M5.Display.setCursor(ix - size/2, iy - size/2);
            M5.Display.print(shape);
        }
    }
    
    // ==================== ADDITIONAL EFFECTS ====================
    void updateCyberGrid(float deltaTime) {
        M5.Display.fillScreen(BLACK);
        
        // Draw grid with neon effect
        float phase = globalTime * 2;
        
        for (int x = 0; x < 240; x += 20) {
            uint8_t brightness = 100 + sin(phase + x * 0.1) * 50;
            uint16_t color = M5.Display.color565(0, brightness, brightness);
            M5.Display.drawFastVLine(x, 0, 135, color);
        }
        
        for (int y = 0; y < 135; y += 15) {
            uint8_t brightness = 100 + cos(phase + y * 0.1) * 50;
            uint16_t color = M5.Display.color565(brightness, 0, brightness);
            M5.Display.drawFastHLine(0, y, 240, color);
        }
        
        // Add moving intersection points
        for (int i = 0; i < 5; i++) {
            float x = 120 + sin(phase + i * 1.2) * 80;
            float y = 67 + cos(phase + i * 0.8) * 40;
            M5.Display.fillCircle(x, y, 3, CYAN);
        }
    }
    
    void updateNeonPulse(float deltaTime) {
        M5.Display.fillScreen(BLACK);
        
        float pulse = sin(globalTime * 4) * 0.5 + 0.5; // 0-1
        uint8_t intensity = 50 + pulse * 200;
        
        // Concentric circles
        for (int r = 10; r < 100; r += 15) {
            uint8_t alpha = intensity * (100 - r) / 100;
            uint16_t color = M5.Display.color565(alpha, alpha/2, alpha);
            M5.Display.drawCircle(120, 67, r, color);
        }
    }
    
    void updateRainbowWave(float deltaTime) {
        M5.Display.fillScreen(BLACK);
        
        for (int x = 0; x < 240; x += 2) {
            float wave = sin(globalTime * 3 + x * 0.02) * 30 + 67;
            
            // Rainbow color based on position
            int hue = (x + (int)(globalTime * 50)) % 360;
            uint16_t color = HSVtoRGB565(hue, 255, 255);
            
            M5.Display.drawFastVLine(x, wave - 5, 10, color);
        }
    }
    
    void updateBinaryFlow(float deltaTime) {
        M5.Display.fillScreen(BLACK);
        
        static String binaryString = "";
        if (binaryString.length() < 1000) {
            for (int i = 0; i < 10; i++) {
                binaryString += String(random(0, 2));
            }
        }
        
        // Scroll the binary
        int scrollPos = (int)(globalTime * 50) % (binaryString.length() * 6);
        
        M5.Display.setTextColor(GREEN);
        for (int i = 0; i < 40; i++) {
            int charIndex = (scrollPos / 6 + i) % binaryString.length();
            char digit = binaryString[charIndex];
            M5.Display.setCursor(i * 6, 60);
            M5.Display.print(digit);
        }
    }
    
    void updatePlasma(float deltaTime) {
        // Simplified plasma effect using rectangles
        for (int x = 0; x < 240; x += 4) {
            for (int y = 0; y < 135; y += 4) {
                float plasma = sin(x * 0.02 + globalTime) + 
                              sin(y * 0.03 + globalTime * 1.5) +
                              sin((x + y) * 0.015 + globalTime * 0.5);
                
                int hue = (int)(plasma * 60 + globalTime * 50) % 360;
                uint16_t color = HSVtoRGB565(hue, 255, 200);
                
                M5.Display.fillRect(x, y, 4, 4, color);
            }
        }
    }
    
    void updateFire(float deltaTime) {
        M5.Display.fillScreen(BLACK);
        
        // Simple fire effect
        for (int x = 0; x < 240; x += 6) {
            float height = 40 + sin(globalTime * 5 + x * 0.1) * 20;
            
            for (int y = 135; y > 135 - height; y -= 3) {
                float intensity = (135 - y) / height;
                int colorIndex = (int)(intensity * 5) % 6;
                uint16_t color = firePalette[colorIndex];
                
                // Add some randomness
                if (random(0, 100) < 70) {
                    M5.Display.fillRect(x + random(-2, 3), y + random(-1, 2), 4, 3, color);
                }
            }
        }
    }
    
    void updateWaterRipple(float deltaTime) {
        M5.Display.fillScreen(BLACK);
        
        // Water ripple from center
        int centerX = 120, centerY = 67;
        
        for (int r = 5; r < 80; r += 8) {
            float ripple = sin(globalTime * 4 - r * 0.3) * 0.5 + 0.5;
            uint8_t alpha = ripple * 150;
            uint16_t color = M5.Display.color565(0, alpha/2, alpha);
            
            M5.Display.drawCircle(centerX, centerY, r, color);
        }
    }
    
    void updateSnow(float deltaTime) {
        // Add snow particles
        if (particles.size() < maxParticles && random(0, 100) < 20) {
            Particle snowflake;
            snowflake.x = random(0, 240);
            snowflake.y = -5;
            snowflake.vx = random(-10, 10) / 10.0f;
            snowflake.vy = random(20, 40) / 10.0f;
            snowflake.life = snowflake.maxLife = 10.0f;
            snowflake.color = WHITE;
            snowflake.type = PARTICLE_STAR;
            snowflake.size = random(2, 5);
            snowflake.rotation = 0;
            snowflake.rotSpeed = random(-100, 100) / 100.0f;
            snowflake.gravity = false;
            snowflake.bounce = 0;
            snowflake.alpha = random(100, 255);
            
            particles.push_back(snowflake);
        }
        
        updateParticles(deltaTime);
    }
    
    void updateStars(float deltaTime) {
        M5.Display.fillScreen(BLACK);
        
        // Twinkling stars
        for (int i = 0; i < 30; i++) {
            int x = (i * 73) % 240;
            int y = (i * 41) % 135;
            
            float twinkle = sin(globalTime * 3 + i) * 0.5 + 0.5;
            uint8_t brightness = 100 + twinkle * 155;
            uint16_t color = M5.Display.color565(brightness, brightness, brightness);
            
            drawStar(x, y, 3, globalTime + i, color);
        }
    }
    
    void updateTVStatic() {
        // TV static effect
        for (int i = 0; i < 1000; i++) {
            int x = random(0, 240);
            int y = random(0, 135);
            uint8_t brightness = random(0, 256);
            uint16_t color = M5.Display.color565(brightness, brightness, brightness);
            M5.Display.drawPixel(x, y, color);
        }
    }
    
    // ==================== SHAPE DRAWING FUNCTIONS ====================
    void drawTriangle(int x, int y, int size, float rotation, uint16_t color) {
        // Simple triangle drawing
        int x1 = x + cos(rotation) * size;
        int y1 = y + sin(rotation) * size;
        int x2 = x + cos(rotation + 2.094) * size; // 120 degrees
        int y2 = y + sin(rotation + 2.094) * size;
        int x3 = x + cos(rotation + 4.189) * size; // 240 degrees
        int y3 = y + sin(rotation + 4.189) * size;
        
        M5.Display.drawTriangle(x1, y1, x2, y2, x3, y3, color);
    }
    
    void drawHeart(int x, int y, int size, uint16_t color) {
        // Simple heart shape using circles and triangle
        int halfSize = size / 2;
        M5.Display.fillCircle(x - halfSize/2, y - halfSize/2, halfSize/2, color);
        M5.Display.fillCircle(x + halfSize/2, y - halfSize/2, halfSize/2, color);
        M5.Display.fillTriangle(x - halfSize, y, x + halfSize, y, x, y + halfSize, color);
    }
    
    void drawStar(int x, int y, int size, float rotation, uint16_t color) {
        // 5-pointed star
        for (int i = 0; i < 5; i++) {
            float angle1 = rotation + (i * 2 * PI / 5);
            float angle2 = rotation + ((i + 1) * 2 * PI / 5);
            
            int x1 = x + cos(angle1) * size;
            int y1 = y + sin(angle1) * size;
            int x2 = x + cos(angle2) * size;
            int y2 = y + sin(angle2) * size;
            
            M5.Display.drawLine(x, y, x1, y1, color);
            M5.Display.drawLine(x1, y1, x2, y2, color);
        }
    }
    
    void drawDiamond(int x, int y, int size, float rotation, uint16_t color) {
        // Diamond shape
        int x1 = x + cos(rotation) * size;
        int y1 = y + sin(rotation) * size;
        int x2 = x + cos(rotation + PI/2) * size;
        int y2 = y + sin(rotation + PI/2) * size;
        int x3 = x + cos(rotation + PI) * size;
        int y3 = y + sin(rotation + PI) * size;
        int x4 = x + cos(rotation + 3*PI/2) * size;
        int y4 = y + sin(rotation + 3*PI/2) * size;
        
        M5.Display.drawLine(x1, y1, x2, y2, color);
        M5.Display.drawLine(x2, y2, x3, y3, color);
        M5.Display.drawLine(x3, y3, x4, y4, color);
        M5.Display.drawLine(x4, y4, x1, y1, color);
    }
    
    void drawSakura(int x, int y, int size, float rotation, uint16_t color) {
        // Cherry blossom petals
        for (int i = 0; i < 5; i++) {
            float angle = rotation + (i * 2 * PI / 5);
            int px = x + cos(angle) * size * 0.7;
            int py = y + sin(angle) * size * 0.7;
            M5.Display.fillCircle(px, py, size/3, color);
        }
        M5.Display.fillCircle(x, y, size/4, YELLOW); // Center
    }
    
    void drawBinaryDigit(int x, int y, int size, uint16_t color) {
        String digit = (random(0, 2) == 0) ? "0" : "1";
        M5.Display.setTextColor(color);
        M5.Display.setCursor(x - size/2, y - size/2);
        M5.Display.print(digit);
    }
    
    void drawSpark(int x, int y, int size, float rotation, uint16_t color) {
        // Cross-shaped spark
        M5.Display.drawLine(x - size, y, x + size, y, color);
        M5.Display.drawLine(x, y - size, x, y + size, color);
        
        // Diagonal lines
        int offset = size * 0.7;
        M5.Display.drawLine(x - offset, y - offset, x + offset, y + offset, color);
        M5.Display.drawLine(x - offset, y + offset, x + offset, y - offset, color);
    }
    
    // ==================== COLOR UTILITIES ====================
    void setupColorPalettes() {
        // Cyberpunk palette
        cyberpunkPalette[0] = M5.Display.color565(255, 0, 255);   // Magenta
        cyberpunkPalette[1] = M5.Display.color565(0, 255, 255);   // Cyan
        cyberpunkPalette[2] = M5.Display.color565(255, 255, 0);   // Yellow
        cyberpunkPalette[3] = M5.Display.color565(255, 0, 0);     // Red
        cyberpunkPalette[4] = M5.Display.color565(0, 255, 0);     // Green
        cyberpunkPalette[5] = M5.Display.color565(0, 0, 255);     // Blue
        cyberpunkPalette[6] = M5.Display.color565(255, 128, 0);   // Orange
        cyberpunkPalette[7] = M5.Display.color565(128, 0, 255);   // Purple
        
        // Kawaii palette
        kawaiPalette[0] = M5.Display.color565(255, 182, 193);     // Light Pink
        kawaiPalette[1] = M5.Display.color565(255, 218, 185);     // Peach
        kawaiPalette[2] = M5.Display.color565(221, 160, 221);     // Plum
        kawaiPalette[3] = M5.Display.color565(173, 216, 230);     // Light Blue
        kawaiPalette[4] = M5.Display.color565(144, 238, 144);     // Light Green
        kawaiPalette[5] = M5.Display.color565(255, 255, 224);     // Light Yellow
        kawaiPalette[6] = M5.Display.color565(255, 192, 203);     // Pink
        kawaiPalette[7] = M5.Display.color565(230, 230, 250);     // Lavender
        
        // Matrix palette
        matrixPalette[0] = M5.Display.color565(0, 255, 0);        // Bright Green
        matrixPalette[1] = M5.Display.color565(0, 200, 0);        // Green
        matrixPalette[2] = M5.Display.color565(0, 150, 0);        // Dark Green
        matrixPalette[3] = M5.Display.color565(0, 100, 0);        // Very Dark Green
        
        // Fire palette
        firePalette[0] = M5.Display.color565(255, 255, 0);        // Yellow
        firePalette[1] = M5.Display.color565(255, 200, 0);        // Orange-Yellow
        firePalette[2] = M5.Display.color565(255, 150, 0);        // Orange
        firePalette[3] = M5.Display.color565(255, 100, 0);        // Red-Orange
        firePalette[4] = M5.Display.color565(255, 50, 0);         // Red
        firePalette[5] = M5.Display.color565(200, 0, 0);          // Dark Red
        
        // Water palette
        waterPalette[0] = M5.Display.color565(0, 100, 255);       // Light Blue
        waterPalette[1] = M5.Display.color565(0, 150, 255);       // Blue
        waterPalette[2] = M5.Display.color565(0, 200, 255);       // Sky Blue
        waterPalette[3] = M5.Display.color565(0, 255, 255);       // Cyan
        waterPalette[4] = M5.Display.color565(100, 255, 255);     // Light Cyan
        waterPalette[5] = M5.Display.color565(200, 255, 255);     // Very Light Cyan
    }
    
    uint16_t HSVtoRGB565(int hue, int sat, int val) {
        // Simplified HSV to RGB565 conversion
        hue = hue % 360;
        
        int r, g, b;
        int i = hue / 60;
        int f = hue % 60;
        int p = val * (255 - sat) / 255;
        int q = val * (255 - sat * f / 60) / 255;
        int t = val * (255 - sat * (60 - f) / 60) / 255;
        
        switch (i) {
            case 0: r = val; g = t; b = p; break;
            case 1: r = q; g = val; b = p; break;
            case 2: r = p; g = val; b = t; break;
            case 3: r = p; g = q; b = val; break;
            case 4: r = t; g = p; b = val; break;
            case 5: r = val; g = p; b = q; break;
            default: r = g = b = 0; break;
        }
        
        return M5.Display.color565(r, g, b);
    }
    
    // ==================== UTILITY FUNCTIONS ====================
    void switchEffect() {
        currentEffect = (EffectType)((currentEffect + 1) % 15);
        clearAllEffects();
        startEffect(currentEffect);
        
        String effectNames[] = {"Particles", "Matrix", "Static", "Glitch", "3D Wire", 
                               "Hearts", "Grid", "Pulse", "Rainbow", "Binary", 
                               "Plasma", "Fire", "Water", "Snow", "Stars"};
        Serial.println("🎨 Effect: " + effectNames[currentEffect]);
    }
    
    void startEffect(EffectType effect) {
        currentEffect = effect;
        clearAllEffects();
        
        switch (effect) {
            case EFFECT_PARTICLES:
                // Particles initialize automatically
                break;
            case EFFECT_MATRIX_RAIN:
                initMatrixRain();
                break;
            case EFFECT_WIREFRAME_3D:
                initWireframeCube();
                break;
            case EFFECT_KAWAII_HEARTS:
                initKawaiiElements();
                break;
            // Other effects initialize in their update functions
        }
    }
    
    void clearAllEffects() {
        particles.clear();
        matrixChars.clear();
        glitchBlocks.clear();
        kawaiiElements.clear();
        M5.Display.fillScreen(BLACK);
    }
    
    void togglePhysics() {
        particlePhysics = !particlePhysics;
        Serial.println("🔬 Physics: " + String(particlePhysics ? "ON" : "OFF"));
    }
    
    void toggleEffects() {
        effectsEnabled = !effectsEnabled;
        if (!effectsEnabled) {
            M5.Display.fillScreen(BLACK);
        }
        Serial.println("✨ Effects: " + String(effectsEnabled ? "ON" : "OFF"));
    }
    
    void increaseParticles() {
        maxParticles = min(maxParticles + 10, 100);
        Serial.println("➕ Max Particles: " + String(maxParticles));
    }
    
    void decreaseParticles() {
        maxParticles = max(maxParticles - 10, 10);
        // Remove excess particles
        while (particles.size() > maxParticles) {
            particles.pop_back();
        }
        Serial.println("➖ Max Particles: " + String(maxParticles));
    }
    
    void changeColorPalette() {
        // Rotate color palettes
        static int paletteIndex = 0;
        uint16_t* palettes[] = {cyberpunkPalette, kawaiPalette, firePalette, waterPalette};
        String names[] = {"Cyberpunk", "Kawaii", "Fire", "Water"};
        
        paletteIndex = (paletteIndex + 1) % 4;
        
        // Copy selected palette to kawaii palette for effects
        for (int i = 0; i < 6; i++) {
            if (i < 8) kawaiPalette[i] = palettes[paletteIndex][i % 6];
        }
        
        Serial.println("🎨 Palette: " + names[paletteIndex]);
    }
    
    void toggleDoubleBuffer() {
        useDoubleBuffer = !useDoubleBuffer;
        
        if (useDoubleBuffer && !screenBuffer) {
            screenBuffer = (uint16_t*)malloc(240 * 135 * 2);
            if (!screenBuffer) {
                useDoubleBuffer = false;
                Serial.println("⚠️ Failed to allocate buffer");
            }
        } else if (!useDoubleBuffer && screenBuffer) {
            free(screenBuffer);
            screenBuffer = nullptr;
        }
        
        Serial.println("📺 Double Buffer: " + String(useDoubleBuffer ? "ON" : "OFF"));
    }
    
    void adaptQuality() {
        // Reduce quality if performance is poor
        if (maxParticles > 20) {
            maxParticles -= 5;
            Serial.println("⚡ Performance adaptation: Reduced particles to " + String(maxParticles));
        }
        
        if (matrixColumns > 20) {
            matrixColumns -= 5;
            Serial.println("⚡ Performance adaptation: Reduced matrix columns to " + String(matrixColumns));
        }
    }
    
    // ==================== ANIMATION TRIGGERS ====================
    void triggerAttackAnimation() {
        currentAnimation = ANIM_ATTACK_START;
        addParticleBurst();
        triggerGlitch();
    }
    
    void triggerSuccessAnimation() {
        currentAnimation = ANIM_ATTACK_SUCCESS;
        startEffect(EFFECT_KAWAII_HEARTS);
        
        // Add celebration particles
        for (int i = 0; i < 20; i++) {
            addRandomParticle();
        }
    }
    
    void triggerFailAnimation() {
        currentAnimation = ANIM_ATTACK_FAIL;
        startEffect(EFFECT_TV_STATIC);
    }
    
    void triggerDiscoveryAnimation() {
        currentAnimation = ANIM_DISCOVERY;
        startEffect(EFFECT_RAINBOW_WAVE);
    }
    
    void setSleepMode() {
        currentAnimation = ANIM_SLEEP;
        startEffect(EFFECT_STARS);
    }
    
    // ==================== STATUS FUNCTIONS ====================
    String getEffectName() {
        String effectNames[] = {"Particles", "Matrix Rain", "TV Static", "Glitch", "3D Wireframe", 
                               "Kawaii Hearts", "Cyber Grid", "Neon Pulse", "Rainbow Wave", "Binary Flow", 
                               "Plasma", "Fire", "Water Ripple", "Snow", "Stars"};
        return effectNames[currentEffect];
    }
    
    int getParticleCount() {
        return particles.size();
    }
    
    float getFrameRate() {
        return 1000.0f / max(frameTime, 1UL);
    }
    
    bool isEffectsEnabled() {
        return effectsEnabled;
    }
};

#endif // M5GOTCHI_VISUAL_EFFECTS_H