#include <graphics.h>
#include <conio.h>
#include <dos.h>  // For delay function
#include <windows.h>  // For mouse functions and key states
#include <stdio.h>
#include <time.h>

#define SHOOTER_WIDTH 50
#define SHOOTER_HEIGHT 20
#define BULLET_RADIUS 5
#define BULLET_SPEED 10
#define SHOOTER_SPEED 8
#define WALL_WIDTH 100
#define WALL_HEIGHT 20
#define NUM_JUMBIES 3
#define JUMBY_WIDTH 30
#define JUMBY_HEIGHT 30
#define JUMBY_SPEED 2
#define JUMBY_ROUND2_SPEED 4
#define JUMBY_ROUND3_SPEED 6
#define ATTACK_RADIUS 10
#define JUMBY_HEALTH 100
#define DAMAGE_DISPLAY_RADIUS 10
#define GAME_TIME 60  // Game duration in seconds

typedef struct {
    int x, y;
    int width, height;
    int isActive; // 0 - inactive, 1 - active
    int direction; // 0 - left, 1 - right
    int health; // Health of the jumby
} Jumby;

void drawShooter(int x, int y) {
    setfillstyle(SOLID_FILL, RED);
    bar(x, y, x + SHOOTER_WIDTH, y + SHOOTER_HEIGHT);
}

void drawBullet(int x, int y) {
    setfillstyle(SOLID_FILL, YELLOW);
    fillellipse(x, y, BULLET_RADIUS, BULLET_RADIUS);
}

void eraseBullet(int x, int y) {
    setfillstyle(SOLID_FILL, BLACK);
    fillellipse(x, y, BULLET_RADIUS, BULLET_RADIUS);
}

void drawJumby(Jumby jumby) {
    if (jumby.isActive) {
        setfillstyle(SOLID_FILL, GREEN);
        bar(jumby.x, jumby.y, jumby.x + jumby.width, jumby.y + jumby.height);

        // Display jumby health
        char healthText[20];
        sprintf(healthText, "HP: %d", jumby.health);
        setcolor(WHITE);
        outtextxy(jumby.x, jumby.y - 20, healthText);
    }
}

void displayScore(int score) {
    char scoreText[30];
    sprintf(scoreText, "Score: %d", score);
    setcolor(WHITE);
    outtextxy(10, 10, scoreText);
}

void displayTime(int timeLeft) {
    char timeText[30];
    sprintf(timeText, "Time: %d", timeLeft);
    setcolor(WHITE);
    outtextxy(getmaxx() - 100, 10, timeText);
}

void drawGameOver() {
    setcolor(RED);
    settextstyle(DEFAULT_FONT, HORIZ_DIR, 2);
    outtextxy(getmaxx() / 2 - 80, getmaxy() / 2, "GAME OVER!");
}

void drawWin() {
    setcolor(YELLOW);
    settextstyle(DEFAULT_FONT, HORIZ_DIR, 2);
    outtextxy(getmaxx() / 2 - 80, getmaxy() / 2 - 20, "YOU WIN! 🎉");
    outtextxy(getmaxx() / 2 - 80, getmaxy() / 2 + 20, "Press Enter to start Round 2");
}

void drawRound2Start() {
    setcolor(YELLOW);
    settextstyle(DEFAULT_FONT, HORIZ_DIR, 2);
    outtextxy(getmaxx() / 2 - 80, getmaxy() / 2 - 20, "ROUND 2 START!");
    outtextxy(getmaxx() / 2 - 80, getmaxy() / 2 + 20, "Press Enter to continue");
}

void drawRound3Start() {
    setcolor(YELLOW);
    settextstyle(DEFAULT_FONT, HORIZ_DIR, 2);
    outtextxy(getmaxx() / 2 - 80, getmaxy() / 2 - 20, "ROUND 3 START!");
    outtextxy(getmaxx() / 2 - 80, getmaxy() / 2 + 20, "Press Enter to continue");
}

int checkCollision(int bulletX, int bulletY, Jumby jumbies[], int numJumbies, int *score) {
    for (int i = 0; i < numJumbies; i++) {
        if (jumbies[i].isActive &&
            bulletX > jumbies[i].x && bulletX < jumbies[i].x + jumbies[i].width &&
            bulletY > jumbies[i].y && bulletY < jumbies[i].y + jumbies[i].height) {
            jumbies[i].health -= 20; // Example damage amount
            if (jumbies[i].health <= 0) {
                jumbies[i].isActive = 0; // Deactivate jumby if health is 0 or below
                (*score) += 10; // Increase score
            }
            return 1; // Hit
        }
    }
    return 0; // No hit
}

int isInAttackRange(int shooterX, int shooterY, Jumby jumby) {
    return (jumby.isActive &&
            abs(shooterX - (jumby.x + jumby.width / 2)) < ATTACK_RADIUS &&
            abs(shooterY - (jumby.y + jumby.height / 2)) < ATTACK_RADIUS);
}

void updateJumbies(Jumby jumbies[], int numJumbies, int shooterX, int shooterY, int round) {
    for (int i = 0; i < numJumbies; i++) {
        if (jumbies[i].isActive) {
            // Move jumbies
            int speed;
            switch (round) {
                case 1:
                    speed = JUMBY_SPEED;
                    break;
                case 2:
                    speed = JUMBY_ROUND2_SPEED;
                    break;
                case 3:
                    speed = JUMBY_ROUND3_SPEED;
                    break;
                default:
                    speed = JUMBY_SPEED;
            }
            if (jumbies[i].direction == 0) { // Moving left
                jumbies[i].x -= speed;
                if (jumbies[i].x < 0) jumbies[i].direction = 1; // Change direction
            } else { // Moving right
                jumbies[i].x += speed;
                if (jumbies[i].x > getmaxx() - jumbies[i].width) jumbies[i].direction = 0; // Change direction
            }

            // Check for attack
            if (isInAttackRange(shooterX, shooterY, jumbies[i])) {
                // Game Over logic or decrease player health
                drawGameOver();
                delay(2000); // Wait for 2 seconds before closing
                closegraph();
                printf("Game Over! You were attacked by a jumby.\n");
                exit(0);
            }
        }
    }
}

int allJumbiesDefeated(Jumby jumbies[], int numJumbies) {
    for (int i = 0; i < numJumbies; i++) {
        if (jumbies[i].isActive) {
            return 0; // Not all jumbies are defeated
        }
    }
    return 1; // All jumbies are defeated
}

void initializeJumbiesForRound(Jumby jumbies[], int numJumbies, int round) {
    for (int i = 0; i < numJumbies; i++) {
        jumbies[i].x = getmaxx() / (i + 2);
        jumbies[i].y = getmaxy() / (i + 2);
        jumbies[i].isActive = 1;
        jumbies[i].health = JUMBY_HEALTH;
        jumbies[i].direction = i % 2; // Alternate direction
    }

    if (round == 2) {
        // Round 2 specific initialization
    } else if (round == 3) {
        // Round 3 specific initialization
        // Example: New positions or behaviors for Round 3
        for (int i = 0; i < numJumbies; i++) {
            jumbies[i].x = getmaxx() / (i + 1) + 50; // New positions
            jumbies[i].y = getmaxy() / (i + 2) + 50; // New positions
            jumbies[i].direction = (i % 2 == 0) ? 0 : 1; // New movement patterns
        }
    }
}

void startRound(int round, Jumby jumbies[], int numJumbies) {
    int shooterX = getmaxx() / 2;
    int shooterY = getmaxy() - SHOOTER_HEIGHT - 10;
    int bulletX = -1, bulletY = -1;
    int score = 0;
    int timeLeft;
    time_t startTime = time(NULL);

    initializeJumbiesForRound(jumbies, numJumbies, round);

    while (1) {
        cleardevice();

        drawShooter(shooterX, shooterY);

        // Draw jumbies
        for (int i = 0; i < numJumbies; i++) {
            drawJumby(jumbies[i]);
        }

        // Update bullet
        if (bulletY >= 0) {
            eraseBullet(bulletX, bulletY);
            bulletY -= BULLET_SPEED;
            if (bulletY < 0 || checkCollision(bulletX, bulletY, jumbies, numJumbies, &score)) {
                bulletY = -1; // Hide bullet
            }
            drawBullet(bulletX, bulletY);
        }

        // Update jumbies' positions
        updateJumbies(jumbies, numJumbies, shooterX, shooterY, round);

        // Check for win condition
        if (allJumbiesDefeated(jumbies, numJumbies)) {
            if (round == 1) {
                drawWin();
                delay(2000); // Wait for 2 seconds to display win message
                // Wait for Enter key to start Round 2
                while (!(GetAsyncKeyState(VK_RETURN) & 0x8000)) {
                    delay(10); // Wait for user input
                }
                startRound(2, jumbies, numJumbies); // Start Round 2
                return;
            } else if (round == 2) {
                drawWin();
                delay(2000); // Wait for 2 seconds to display win message
                // Wait for Enter key to start Round 3
                while (!(GetAsyncKeyState(VK_RETURN) & 0x8000)) {
                    delay(10); // Wait for user input
                }
                startRound(3, jumbies, numJumbies); // Start Round 3
                return;
            } else {
                drawWin();
                delay(2000); // Wait for 2 seconds to display win message
                closegraph();
                printf("Congratulations! You won Round 3!\n");
                return;
            }
        }

        // Update the countdown timer
        time_t currentTime = time(NULL);
        timeLeft = GAME_TIME - (currentTime - startTime);
        if (timeLeft <= 0) {
            drawGameOver();
            delay(2000); // Wait for 2 seconds before closing
            closegraph();
            printf("Time's up! Game Over!\n");
            return;
        }

        // Display score and time
        displayScore(score);
        displayTime(timeLeft);

        // Movement controls
        if (GetAsyncKeyState('W') & 0x8000) { // Move up
            if (shooterY > 0) {
                shooterY -= SHOOTER_SPEED;
            }
        }
        if (GetAsyncKeyState('S') & 0x8000) { // Move down
            if (shooterY < getmaxy() - SHOOTER_HEIGHT) {
                shooterY += SHOOTER_SPEED;
            }
        }
        if (GetAsyncKeyState('A') & 0x8000) { // Move left
            if (shooterX > 0) {
                shooterX -= SHOOTER_SPEED;
            }
        }
        if (GetAsyncKeyState('D') & 0x8000) { // Move right
            if (shooterX < getmaxx() - SHOOTER_WIDTH) {
                shooterX += SHOOTER_SPEED;
            }
        }

        // Shoot with the space bar or 'K' key
        if (GetAsyncKeyState(VK_SPACE) & 0x8000 || GetAsyncKeyState('K') & 0x8000) {
            if (bulletY < 0) { // Only allow a new bullet if no bullet is on screen
                bulletX = shooterX + SHOOTER_WIDTH / 2;
                bulletY = shooterY;
                delay(200); // Short delay to prevent rapid shooting
            }
        }

        delay(10); // Control the frame rate for smooth movement
    }
}

int main() {
    // Initialize the graphics mode
    int gd = DETECT, gm;
    initgraph(&gd, &gm, "");

    Jumby jumbies[NUM_JUMBIES] = {
        {getmaxx() / 3, getmaxy() / 4, JUMBY_WIDTH, JUMBY_HEIGHT, 1, 1, JUMBY_HEALTH},
        {getmaxx() / 2, getmaxy() / 3, JUMBY_WIDTH, JUMBY_HEIGHT, 1, 0, JUMBY_HEALTH},
        {getmaxx() * 2 / 3, getmaxy() / 2, JUMBY_WIDTH, JUMBY_HEIGHT, 1, 1, JUMBY_HEALTH}
    };

    startRound(1, jumbies, NUM_JUMBIES);

    // Close the graphics mode
    closegraph();
    return 0;
}
