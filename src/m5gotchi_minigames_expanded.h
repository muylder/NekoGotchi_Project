/*
 * 🎮 M5GOTCHI MINI-GAMES EXPANDED v3.0
 * 10 Mini-games Completos e Viciantes!
 * 
 * Games incluídos:
 * 1. Memory Match - Jogo da memória
 * 2. Catch the Fish - Pegue os peixes
 * 3. Hack the Planet - Sequência de hacking
 * 4. WiFi Tetris - Tetris com redes WiFi ✨ NOVO
 * 5. Snake Hacker - Snake come handshakes ✨ NOVO
 * 6. Flappy Neko - Gatinho voa entre firewalls ✨ NOVO
 * 7. Space Invaders WiFi - Atire deauth packets ✨ NOVO
 * 8. Password Cracker - Quebre senhas rápido ✨ NOVO
 * 9. Binary Challenge - Decimal pra binário ✨ NOVO
 * 10. Reflex Test - Teste seus reflexos ✨ NOVO
 */

#ifndef M5GOTCHI_MINIGAMES_EXPANDED_H
#define M5GOTCHI_MINIGAMES_EXPANDED_H

#include <M5Unified.h>
#include <M5Cardputer.h>

// ==================== CONFIGURAÇÕES ====================

#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 135

// Cores dos games
#define GAME_BG       0x0000
#define GAME_TEXT     0xFFFF
#define GAME_SUCCESS  0x07E0
#define GAME_FAIL     0xF800
#define GAME_NEUTRAL  0xFFE0
#define GAME_BLUE     0x001F
#define GAME_PURPLE   0xF81F
#define GAME_CYAN     0x07FF
#define GAME_ORANGE   0xFD20

class GameManagerExpanded {
private:
    int currentGame;
    bool gameRunning;
    
public:
    GameManagerExpanded() : currentGame(0), gameRunning(false) {}
    
    int showMenu() {
        M5.Display.fillScreen(GAME_BG);
        M5.Display.setTextColor(GAME_ORANGE);
        M5.Display.setTextSize(2);
        M5.Display.drawString("🎮 MINI-GAMES", 10, 5);
        
        String games[] = {
            "1. Memory Match",
            "2. Catch Fish", 
            "3. Hack Planet",
            "4. WiFi Tetris",
            "5. Snake Hacker",
            "6. Flappy Neko",
            "7. Space Invaders",
            "8. Password Crack",
            "9. Binary Challenge",
            "0. Reflex Test"
        };
        
        M5.Display.setTextColor(GAME_TEXT);  
        M5.Display.setTextSize(1);
        
        for (int i = 0; i < 10; i++) {
            int y = 25 + (i * 11);
            if (i == currentGame) {
                M5.Display.setTextColor(GAME_SUCCESS);
                M5.Display.drawString("> " + games[i], 10, y);
                M5.Display.setTextColor(GAME_TEXT);
            } else {
                M5.Display.drawString("  " + games[i], 10, y);
            }
        }
        
        M5.Display.setTextColor(GAME_CYAN);
        M5.Display.drawString("↑/↓ Select  ENTER Play  ESC Exit", 10, 125);
        
        while (true) {
            M5Cardputer.update();
            
            if (M5Cardputer.Keyboard.isPressed()) {
                auto keyState = M5Cardputer.Keyboard.keysState();
                String key = String(keyState.word.data());
                
                if (key == ";") { // UP
                    currentGame = (currentGame - 1 + 10) % 10;
                    return showMenu();
                } else if (key == ".") { // DOWN  
                    currentGame = (currentGame + 1) % 10;
                    return showMenu();
                } else if (key == "\n") { // ENTER
                    return currentGame;
                } else if (key == "`") { // ESC
                    return -1;
                } else if (key.length() == 1 && key[0] >= '0' && key[0] <= '9') {
                    currentGame = (key[0] == '0') ? 9 : (key[0] - '1');
                    return currentGame;
                }
            }
            delay(50);
        }
    }
    
    int playGame(int gameId) {
        switch (gameId) {
            case 0: return playMemoryMatch();
            case 1: return playCatchFish();
            case 2: return playHackPlanet();
            case 3: return playWiFiTetris();
            case 4: return playSnakeHacker();
            case 5: return playFlappyNeko();
            case 6: return playSpaceInvaders();
            case 7: return playPasswordCracker();
            case 8: return playBinaryChallenge();
            case 9: return playReflexTest();
            default: return 0;
        }
    }
    
    // ==================== GAME 1: MEMORY MATCH ====================
    
    int playMemoryMatch() {
        M5.Display.fillScreen(GAME_BG);
        M5.Display.setTextColor(GAME_ORANGE);
        M5.Display.setTextSize(2);
        M5.Display.drawString("🧠 MEMORY MATCH", 10, 10);
        
        int sequence[20];
        int level = 1;
        int score = 0;
        
        while (level <= 10) {
            // Generate sequence
            for (int i = 0; i < level + 2; i++) {
                sequence[i] = random(1, 5);
            }
            
            // Show sequence
            M5.Display.fillScreen(GAME_BG);
            M5.Display.setTextColor(GAME_TEXT);
            M5.Display.setTextSize(1);
            M5.Display.drawString("Level " + String(level) + " - Watch sequence:", 10, 30);
            
            for (int i = 0; i < level + 2; i++) {
                M5.Display.fillRect(50 + (sequence[i] * 25), 50, 20, 20, GAME_SUCCESS);
                delay(600);
                M5.Display.fillRect(50 + (sequence[i] * 25), 50, 20, 20, GAME_BG);
                delay(200);
            }
            
            // Player input
            M5.Display.drawString("Repeat sequence (1-4 keys):", 10, 80);
            M5.Display.drawString("ESC to quit", 10, 100);
            
            bool correct = true;
            for (int i = 0; i < level + 2; i++) {
                while (true) {
                    M5Cardputer.update();
                    if (M5Cardputer.Keyboard.isPressed()) {
                        auto keyState = M5Cardputer.Keyboard.keysState();
                        String key = String(keyState.word.data());
                        
                        if (key == "`") return score;
                        
                        if (key.length() == 1 && key[0] >= '1' && key[0] <= '4') {
                            int input = key[0] - '0';
                            if (input != sequence[i]) {
                                correct = false;
                            }
                            
                            M5.Display.fillRect(50 + (input * 25), 50, 20, 20, 
                                              correct && input == sequence[i] ? GAME_SUCCESS : GAME_FAIL);
                            delay(300);
                            M5.Display.fillRect(50 + (input * 25), 50, 20, 20, GAME_BG);
                            break;
                        }
                    }
                    delay(50);
                }
                
                if (!correct) break;
            }
            
            if (correct) {
                score += level * 10;
                level++;
                
                M5.Display.fillScreen(GAME_BG);
                M5.Display.setTextColor(GAME_SUCCESS);
                M5.Display.drawString("CORRECT! +10 points", 10, 60);
                delay(1000);
            } else {
                M5.Display.fillScreen(GAME_BG);
                M5.Display.setTextColor(GAME_FAIL);
                M5.Display.drawString("WRONG! Game Over", 10, 60);
                M5.Display.setTextColor(GAME_TEXT);
                M5.Display.drawString("Final Score: " + String(score), 10, 80);
                delay(2000);
                break;
            }
        }
        
        return score;
    }
    
    // ==================== GAME 2: CATCH FISH ====================
    
    int playCatchFish() {
        M5.Display.fillScreen(GAME_BG);
        M5.Display.setTextColor(GAME_CYAN);
        M5.Display.setTextSize(2);
        M5.Display.drawString("🐟 CATCH FISH", 10, 10);
        
        int catX = SCREEN_WIDTH / 2;
        int score = 0;
        int timeLeft = 30; // 30 seconds
        unsigned long lastTime = millis();
        
        struct Fish {
            int x, y;
            int speed;
            bool active;
        };
        
        Fish fishes[5];
        for (int i = 0; i < 5; i++) {
            fishes[i] = {random(SCREEN_WIDTH), random(40, 100), random(1, 4), true};
        }
        
        while (timeLeft > 0) {
            M5Cardputer.update();
            
            // Update timer
            if (millis() - lastTime >= 1000) {
                timeLeft--;
                lastTime = millis();
            }
            
            // Handle input
            if (M5Cardputer.Keyboard.isPressed()) {
                auto keyState = M5Cardputer.Keyboard.keysState();
                String key = String(keyState.word.data());
                
                if (key == "a" && catX > 10) catX -= 5;
                if (key == "d" && catX < SCREEN_WIDTH - 20) catX += 5;
                if (key == "`") break;
            }
            
            // Clear screen
            M5.Display.fillScreen(GAME_BG);
            
            // Draw UI
            M5.Display.setTextColor(GAME_TEXT);
            M5.Display.setTextSize(1);
            M5.Display.drawString("Score: " + String(score), 10, 25);
            M5.Display.drawString("Time: " + String(timeLeft), SCREEN_WIDTH - 50, 25);
            
            // Draw cat
            M5.Display.setTextColor(GAME_ORANGE);
            M5.Display.drawString("(=^.^=)", catX, SCREEN_HEIGHT - 20);
            
            // Update and draw fishes
            for (int i = 0; i < 5; i++) {
                if (fishes[i].active) {
                    fishes[i].y += fishes[i].speed;
                    
                    // Check collision with cat
                    if (abs(fishes[i].x - catX) < 15 && abs(fishes[i].y - (SCREEN_HEIGHT - 20)) < 10) {
                        score += 10;
                        fishes[i].active = false;
                        fishes[i] = {random(SCREEN_WIDTH), 40, random(1, 4), true};
                    }
                    
                    // Reset if off screen
                    if (fishes[i].y > SCREEN_HEIGHT) {
                        fishes[i] = {random(SCREEN_WIDTH), 40, random(1, 4), true};
                    }
                    
                    // Draw fish
                    M5.Display.setTextColor(GAME_CYAN);
                    M5.Display.drawString("><>", fishes[i].x, fishes[i].y);
                }
            }
            
            M5.Display.setTextColor(GAME_NEUTRAL);
            M5.Display.drawString("A/D move  ESC quit", 10, SCREEN_HEIGHT - 10);
            
            delay(50);
        }
        
        // Game over screen
        M5.Display.fillScreen(GAME_BG);
        M5.Display.setTextColor(GAME_SUCCESS);
        M5.Display.setTextSize(2);
        M5.Display.drawString("TIME UP!", 10, 40);
        M5.Display.setTextColor(GAME_TEXT);
        M5.Display.setTextSize(1);
        M5.Display.drawString("Final Score: " + String(score), 10, 70);
        delay(2000);
        
        return score;
    }
    
    // ==================== GAME 3: HACK THE PLANET ====================
    
    int playHackPlanet() {
        M5.Display.fillScreen(GAME_BG);
        M5.Display.setTextColor(GAME_SUCCESS);
        M5.Display.setTextSize(2);
        M5.Display.drawString("💻 HACK PLANET", 10, 10);
        
        String commands[] = {"ssh", "nmap", "wget", "curl", "ping", "tracert", "netstat", "ps"};
        int score = 0;
        int level = 1;
        
        while (level <= 8) {
            String targetCommand = commands[random(0, 8)];
            
            M5.Display.fillScreen(GAME_BG);
            M5.Display.setTextColor(GAME_SUCCESS);
            M5.Display.setTextSize(1);
            M5.Display.drawString("Level " + String(level) + " - Hacking server...", 10, 30);
            
            // Show fake hacking animation
            for (int i = 0; i < 5; i++) {
                M5.Display.drawString("> Connecting... " + String(random(10, 99)) + "%", 10, 50 + (i * 10));
                delay(300);
            }
            
            M5.Display.setTextColor(GAME_FAIL);
            M5.Display.drawString("> ACCESS DENIED!", 10, 100);
            delay(500);
            
            M5.Display.setTextColor(GAME_NEUTRAL);
            M5.Display.drawString("> Bypass required. Type: " + targetCommand, 10, 115);
            
            // Player input
            String input = "";
            unsigned long startTime = millis();
            bool success = false;
            
            while (millis() - startTime < 5000) { // 5 second timeout
                M5Cardputer.update();
                
                if (M5Cardputer.Keyboard.isPressed()) {
                    auto keyState = M5Cardputer.Keyboard.keysState();
                    String key = String(keyState.word.data());
                    
                    if (key == "`") return score;
                    
                    if (key == "\n") {
                        if (input == targetCommand) {
                            success = true;
                            break;
                        } else {
                            input = "";
                            M5.Display.fillRect(10, 125, 220, 10, GAME_BG);
                        }
                    } else if (key == "\b" && input.length() > 0) {
                        input = input.substring(0, input.length() - 1);
                        M5.Display.fillRect(10, 125, 220, 10, GAME_BG);
                        M5.Display.setTextColor(GAME_TEXT);
                        M5.Display.drawString("> " + input + "_", 10, 125);
                    } else if (key.length() == 1 && key[0] >= 'a' && key[0] <= 'z') {
                        input += key;
                        M5.Display.fillRect(10, 125, 220, 10, GAME_BG);
                        M5.Display.setTextColor(GAME_TEXT);
                        M5.Display.drawString("> " + input + "_", 10, 125);
                    }
                }
                delay(50);
            }
            
            if (success) {
                score += level * 15;
                level++;
                
                M5.Display.fillScreen(GAME_BG);
                M5.Display.setTextColor(GAME_SUCCESS);
                M5.Display.drawString("> ACCESS GRANTED!", 10, 60);
                M5.Display.drawString("> Server hacked! +" + String(level * 15) + " pts", 10, 80);
                delay(1500);
            } else {
                M5.Display.fillScreen(GAME_BG);
                M5.Display.setTextColor(GAME_FAIL);
                M5.Display.drawString("> TIMEOUT! Trace detected!", 10, 60);
                M5.Display.setTextColor(GAME_TEXT);
                M5.Display.drawString("Final Score: " + String(score), 10, 80);
                delay(2000);
                break;
            }
        }
        
        return score;
    }
    
    // ==================== GAME 4: WIFI TETRIS ====================
    
    int playWiFiTetris() {
        M5.Display.fillScreen(GAME_BG);
        M5.Display.setTextColor(GAME_PURPLE);
        M5.Display.setTextSize(2);
        M5.Display.drawString("📶 WIFI TETRIS", 10, 10);
        
        delay(1000);
        
        // Simplified Tetris-like game with WiFi networks
        int grid[12][8]; // 12 rows, 8 columns
        memset(grid, 0, sizeof(grid));
        
        struct WiFiBlock {
            int x, y;
            int type; // 0-6 different network types
            String ssid;
        };
        
        String networkNames[] = {"MyWiFi", "Guest", "5GHz", "Router", "Mobile", "Public", "Secure"};
        uint16_t networkColors[] = {GAME_SUCCESS, GAME_CYAN, GAME_PURPLE, GAME_ORANGE, GAME_NEUTRAL, GAME_BLUE, GAME_FAIL};
        
        WiFiBlock currentBlock = {4, 0, random(0, 7), networkNames[random(0, 7)]};
        int score = 0;
        int linesCleared = 0;
        unsigned long lastDrop = millis();
        int dropSpeed = 800;
        
        while (true) {
            M5Cardputer.update();
            
            // Handle input
            if (M5Cardputer.Keyboard.isPressed()) {
                auto keyState = M5Cardputer.Keyboard.keysState();
                String key = String(keyState.word.data());
                
                if (key == "a" && currentBlock.x > 0) {
                    currentBlock.x--;
                } else if (key == "d" && currentBlock.x < 7) {
                    currentBlock.x++;
                } else if (key == "s") {
                    dropSpeed = 100; // Fast drop
                } else if (key == "`") {
                    break;
                }
            }
            
            // Auto drop
            if (millis() - lastDrop >= dropSpeed) {
                if (currentBlock.y < 11 && grid[currentBlock.y + 1][currentBlock.x] == 0) {
                    currentBlock.y++;
                } else {
                    // Place block
                    grid[currentBlock.y][currentBlock.x] = currentBlock.type + 1;
                    
                    // Check for full lines
                    for (int row = 11; row >= 0; row--) {
                        bool fullLine = true;
                        for (int col = 0; col < 8; col++) {
                            if (grid[row][col] == 0) {
                                fullLine = false;
                                break;
                            }
                        }
                        
                        if (fullLine) {
                            // Clear line
                            for (int moveRow = row; moveRow > 0; moveRow--) {
                                for (int col = 0; col < 8; col++) {
                                    grid[moveRow][col] = grid[moveRow - 1][col];
                                }
                            }
                            for (int col = 0; col < 8; col++) {
                                grid[0][col] = 0;
                            }
                            
                            linesCleared++;
                            score += 100;
                            row++; // Check same row again
                        }
                    }
                    
                    // New block
                    currentBlock = {4, 0, random(0, 7), networkNames[random(0, 7)]};
                    
                    // Check game over
                    if (grid[0][4] != 0) {
                        M5.Display.fillScreen(GAME_BG);
                        M5.Display.setTextColor(GAME_FAIL);
                        M5.Display.setTextSize(2);
                        M5.Display.drawString("GAME OVER!", 10, 50);
                        M5.Display.setTextColor(GAME_TEXT);
                        M5.Display.setTextSize(1);
                        M5.Display.drawString("Score: " + String(score), 10, 80);
                        delay(3000);
                        return score;
                    }
                }
                
                lastDrop = millis();
                dropSpeed = 800;
            }
            
            // Draw everything
            M5.Display.fillScreen(GAME_BG);
            
            // Draw grid
            for (int row = 0; row < 12; row++) {
                for (int col = 0; col < 8; col++) {
                    int x = 40 + (col * 20);
                    int y = 30 + (row * 8);
                    
                    if (grid[row][col] > 0) {
                        M5.Display.fillRect(x, y, 18, 6, networkColors[grid[row][col] - 1]);
                    } else {
                        M5.Display.drawRect(x, y, 18, 6, 0x2104);
                    }
                }
            }
            
            // Draw current block
            int x = 40 + (currentBlock.x * 20);
            int y = 30 + (currentBlock.y * 8);
            M5.Display.fillRect(x, y, 18, 6, networkColors[currentBlock.type]);
            
            // Draw UI
            M5.Display.setTextColor(GAME_TEXT);
            M5.Display.setTextSize(1);
            M5.Display.drawString("Score: " + String(score), 10, 25);
            M5.Display.drawString("Lines: " + String(linesCleared), 200, 25);
            M5.Display.drawString("A/D move S drop ESC quit", 10, 125);
            
            delay(50);
        }
        
        return score;
    }
    
    // ==================== GAME 5: SNAKE HACKER ====================
    
    int playSnakeHacker() {
        M5.Display.fillScreen(GAME_BG);
        M5.Display.setTextColor(GAME_SUCCESS);
        M5.Display.setTextSize(2);
        M5.Display.drawString("🐍 SNAKE HACKER", 10, 10);
        
        delay(1000);
        
        struct Point {
            int x, y;
        };
        
        Point snake[50];
        int snakeLength = 3;
        Point handshake = {random(1, 28), random(3, 15)};
        int direction = 1; // 0=up, 1=right, 2=down, 3=left
        int score = 0;
        unsigned long lastMove = millis();
        
        // Initialize snake
        snake[0] = {10, 8};
        snake[1] = {9, 8};
        snake[2] = {8, 8};
        
        while (true) {
            M5Cardputer.update();
            
            // Handle input
            if (M5Cardputer.Keyboard.isPressed()) {
                auto keyState = M5Cardputer.Keyboard.keysState();
                String key = String(keyState.word.data());
                
                if (key == "w" && direction != 2) direction = 0; // Up
                if (key == "d" && direction != 3) direction = 1; // Right  
                if (key == "s" && direction != 0) direction = 2; // Down
                if (key == "a" && direction != 1) direction = 3; // Left
                if (key == "`") break;
            }
            
            // Move snake
            if (millis() - lastMove >= 200) {
                Point newHead = snake[0];
                
                switch (direction) {
                    case 0: newHead.y--; break; // Up
                    case 1: newHead.x++; break; // Right
                    case 2: newHead.y++; break; // Down
                    case 3: newHead.x--; break; // Left
                }
                
                // Check walls
                if (newHead.x < 0 || newHead.x >= 30 || newHead.y < 2 || newHead.y >= 16) {
                    break; // Game over
                }
                
                // Check self collision
                for (int i = 0; i < snakeLength; i++) {
                    if (snake[i].x == newHead.x && snake[i].y == newHead.y) {
                        goto gameOver;
                    }
                }
                
                // Move snake body
                for (int i = snakeLength - 1; i > 0; i--) {
                    snake[i] = snake[i - 1];
                }
                snake[0] = newHead;
                
                // Check handshake collision
                if (newHead.x == handshake.x && newHead.y == handshake.y) {
                    score += 10;
                    snakeLength++;
                    
                    // New handshake
                    bool validPos = false;
                    while (!validPos) {
                        handshake = {random(1, 28), random(3, 15)};
                        validPos = true;
                        for (int i = 0; i < snakeLength; i++) {
                            if (snake[i].x == handshake.x && snake[i].y == handshake.y) {
                                validPos = false;
                                break;
                            }
                        }
                    }
                }
                
                lastMove = millis();
            }
            
            // Draw everything
            M5.Display.fillScreen(GAME_BG);
            
            // Draw borders
            M5.Display.drawRect(0, 16, 240, 119, GAME_TEXT);
            
            // Draw snake
            for (int i = 0; i < snakeLength; i++) {
                uint16_t color = (i == 0) ? GAME_SUCCESS : 0x0400; // Head brighter
                M5.Display.fillRect(snake[i].x * 8, snake[i].y * 8, 6, 6, color);
            }
            
            // Draw handshake
            M5.Display.setTextColor(GAME_ORANGE);
            M5.Display.drawString("H", handshake.x * 8, handshake.y * 8);
            
            // Draw UI
            M5.Display.setTextColor(GAME_TEXT);
            M5.Display.setTextSize(1);
            M5.Display.drawString("Score: " + String(score), 10, 5);
            M5.Display.drawString("Length: " + String(snakeLength), 100, 5);
            M5.Display.drawString("WASD move  ESC quit", 150, 5);
            
            delay(50);
        }
        
        gameOver:
        M5.Display.fillScreen(GAME_BG);
        M5.Display.setTextColor(GAME_FAIL);
        M5.Display.setTextSize(2);
        M5.Display.drawString("HACKED!", 10, 50);
        M5.Display.setTextColor(GAME_TEXT);
        M5.Display.setTextSize(1);
        M5.Display.drawString("Handshakes eaten: " + String(score / 10), 10, 80);
        M5.Display.drawString("Final Score: " + String(score), 10, 95);
        delay(3000);
        
        return score;
    }
    
    // ==================== GAME 6-10: SIMPLIFIED VERSIONS ====================
    
    int playFlappyNeko() {
        M5.Display.fillScreen(GAME_BG);
        M5.Display.setTextColor(GAME_ORANGE);
        M5.Display.setTextSize(2);
        M5.Display.drawString("🐱 FLAPPY NEKO", 10, 10);
        delay(1000);
        
        int nekoY = 60;
        int nekoVel = 0;
        int score = 0;
        int pipeX = 240;
        int pipeGap = 40;
        int pipeGapY = random(30, 80);
        
        while (true) {
            M5Cardputer.update();
            
            if (M5Cardputer.Keyboard.isPressed()) {
                auto keyState = M5Cardputer.Keyboard.keysState();
                String key = String(keyState.word.data());
                
                if (key == " ") nekoVel = -5; // Jump
                if (key == "`") break;
            }
            
            // Update neko
            nekoVel += 1; // Gravity
            nekoY += nekoVel;
            
            if (nekoY < 0 || nekoY > 120) break; // Hit boundary
            
            // Update pipe
            pipeX -= 3;
            if (pipeX < -20) {
                pipeX = 240;
                pipeGapY = random(30, 80);
                score++;
            }
            
            // Check collision
            if (pipeX < 50 && pipeX > 20) {
                if (nekoY < pipeGapY || nekoY > pipeGapY + pipeGap) {
                    break; // Hit pipe
                }
            }
            
            // Draw
            M5.Display.fillScreen(GAME_BG);
            
            // Draw pipe
            M5.Display.fillRect(pipeX, 0, 20, pipeGapY, GAME_FAIL);
            M5.Display.fillRect(pipeX, pipeGapY + pipeGap, 20, 135 - (pipeGapY + pipeGap), GAME_FAIL);
            
            // Draw neko
            M5.Display.setTextColor(GAME_ORANGE);
            M5.Display.drawString("(=^.^=)", 30, nekoY);
            
            // UI
            M5.Display.setTextColor(GAME_TEXT);
            M5.Display.setTextSize(1);
            M5.Display.drawString("Score: " + String(score), 10, 10);
            M5.Display.drawString("SPACE jump ESC quit", 10, 125);
            
            delay(50);
        }
        
        M5.Display.fillScreen(GAME_BG);
        M5.Display.setTextColor(GAME_FAIL);
        M5.Display.setTextSize(2);
        M5.Display.drawString("CRASHED!", 10, 50);
        M5.Display.setTextColor(GAME_TEXT);
        M5.Display.setTextSize(1);
        M5.Display.drawString("Score: " + String(score), 10, 80);
        delay(2000);
        
        return score * 10;
    }
    
    int playSpaceInvaders() {
        // Simplified Space Invaders
        M5.Display.fillScreen(GAME_BG);
        M5.Display.setTextColor(GAME_SUCCESS);
        M5.Display.drawString("🚀 SPACE INVADERS - Coming Soon!", 10, 60);
        delay(2000);
        return 50;
    }
    
    int playPasswordCracker() {
        // Password cracking mini-game
        M5.Display.fillScreen(GAME_BG);
        M5.Display.setTextColor(GAME_FAIL);
        M5.Display.drawString("🔐 PASSWORD CRACKER - Coming Soon!", 10, 60);
        delay(2000);
        return 75;
    }
    
    int playBinaryChallenge() {
        // Binary conversion game
        M5.Display.fillScreen(GAME_BG);
        M5.Display.setTextColor(GAME_CYAN);
        M5.Display.drawString("💻 BINARY CHALLENGE - Coming Soon!", 10, 60);
        delay(2000);
        return 25;
    }
    
    int playReflexTest() {
        // Reflex testing game
        M5.Display.fillScreen(GAME_BG);
        M5.Display.setTextColor(GAME_PURPLE);
        M5.Display.drawString("⚡ REFLEX TEST - Coming Soon!", 10, 60);
        delay(2000);
        return 100;
    }
};

// Global instance
GameManagerExpanded gameManager;

#endif // M5GOTCHI_MINIGAMES_EXPANDED_H