#include "iGraphics.h"

// ==================== Function Prototypes ====================
void iFilledTriangle(double x1, double y1, double x2, double y2, double x3, double y3);
void initializeBossSystem();
void drawMainMenu();
void drawNameInputScreen();
void drawLevelSelectScreen();
void drawSpaceshipSelectScreen();
void checkBulletEnemyCollision();
void updateBullets();
void drawGameScreen();
void spawnPowerUps();
void checkBossActivation();
void spawnEnemy();
void updateEnemies();
void moveEnemies();
void handlePowerUps();
void handleMegaBomb();
void handleBossMechanics();
void drawGameOverScreen();
void loadScores();
void drawScoreboardScreen();
void drawHelpPage();
void iDraw();
void iMouse(int button, int state, int mx, int my);
void iKeyboard(unsigned char key);
void iSpecialKeyboard(unsigned char key);
void iMouseDrag(int mx, int my);
void iMouseMove(int mx, int my);
void iMouseWheel(int button, int dir, int mx);
int main(int argc, char *argv[]);

// Template prototype for moveAndCheckPickup
template <typename Callback>
void moveAndCheckPickup(bool *active, int *x, int *y, int size, const char *label, Callback onPickup);

#include "iGraphics.h"

// ==================== Screen & Menu ====================
int screenWidth = 800;
int screenHeight = 600;
int currentMenu = 0; // 0 = Main Menu, 1 = Name Input

// ==================== Player Info ======================
char playerName[100] = "";
int nameCharIndex = 0;
int selectedLevel = 0;
int selectedShip = -1;
const int totalShips = 3;

// ==================== Player Ship ======================
int playerX = 400;
int playerY = 50;
int playerSpeed = 10;

// ==================== Bullets ==========================
const int maxBullets = 100;
int bulletX[maxBullets], bulletY[maxBullets];
bool bulletActive[maxBullets];
int bulletCount = 0;
int bulletSpeed = 15;

// ==================== Enemies ==========================
const int maxEnemies = 10;
int enemyX[maxEnemies];
int enemyY[maxEnemies];
bool enemyActive[maxEnemies];
int enemySpeed = 2;
enum EnemyType
{
    NORMAL,
    SHOOTER,
    FAST
};
EnemyType enemyType[maxEnemies];

// ==================== Player Stats =====================
int playerLife = 3;
bool isGameOver = false;
int playerHealth = 100; // 100% health
const int maxHealth = 100;

// ==================== UI Buttons =======================
int buttonWidth = 200;
int buttonHeight = 50;
int buttonX = screenWidth / 2 - buttonWidth / 2;
int buttonYStart = 400;
int buttonGap = 70;

// ==================== Score & Pause ====================
int score = 0;
bool isPaused = false;

// ==================== Power-Ups ========================
bool healthPackActive = false;
int healthPackX = 0;
int healthPackY = 0;
int healthPackSize = 30;

bool scoreMultiplierActive = false;
int scoreMultiplierValue = 1; // 1x (normal)
int multiplierTimer = 0;      // Countdown timer
int multiplierX = 0;
int multiplierY = 0;
bool multiplierActive = false;
int multiplierSize = 30;

bool armorActive = false;
int armorTimer = 0;
bool armorPackActive = false;
int armorX = 0;
int armorY = 0;
int armorSize = 30;

bool rapidFireActive = false;
int rapidFireTimer = 0;
bool rapidFirePackActive = false;
int rapidX = 0;
int rapidY = 0;
int rapidSize = 30;

bool megaBombActive = false;
bool megaBombPackActive = false;
int megaBombX = 0;
int megaBombY = 0;
int megaBombSize = 30;

// ==================== Boss =============================
bool bossActive = false;
int bossX = 200;
int bossY = screenHeight - 150;
int bossWidth = 400;
int bossHeight = 100;
int bossHealth = 100; // 100 hits needed (adjustable)
int bossBulletX[10];
int bossBulletY[10];
bool bossBulletActive[10];
int bossFireTimer = 0;

// ==================== Scoreboard =======================
struct ScoreEntry
{
    char name[100];
    int score;
};
ScoreEntry scores[100];
int scoreCount = 0;

const char *menuItems[] = {
    "New Game",
    "Scoreboard",
    "Help",
    "About",
    "Exit"};
const int menuCount = sizeof(menuItems) / sizeof(menuItems[0]);

void iFilledTriangle(double x1, double y1, double x2, double y2, double x3, double y3)
{
    double x[] = {x1, x2, x3};
    double y[] = {y1, y2, y3};
    iFilledPolygon(x, y, 3);
}

void initializeBossSystem()
{
    bossActive = false;
    bossX = 200;
    bossY = screenHeight - 150;
    bossWidth = 400;
    bossHeight = 100;
    bossHealth = 100;
    bossFireTimer = 0;

    for (int i = 0; i < 10; i++)
    {
        bossBulletActive[i] = false;
    }
}

// Function to draw the main menu
void drawMainMenu()
{
    iSetColor(255, 255, 255);
    iText(screenWidth / 2 - 100, buttonYStart + menuCount * buttonGap, "SPACE SHOOTER GAME", GLUT_BITMAP_TIMES_ROMAN_24);

    for (int i = 0; i < menuCount; i++)
    {
        int y = buttonYStart - i * buttonGap;
        iSetColor(50, 50, 255); // Button color
        iFilledRectangle(buttonX, y, buttonWidth, buttonHeight);

        iSetColor(255, 255, 255); // Text color
        iText(buttonX + 60, y + 18, menuItems[i], GLUT_BITMAP_HELVETICA_18);
    }
}

void drawNameInputScreen()
{
    iSetColor(255, 255, 255);
    iText(screenWidth / 2 - 80, 400, "Enter Your Name:", GLUT_BITMAP_TIMES_ROMAN_24);

    iSetColor(0, 0, 0);
    iFilledRectangle(screenWidth / 2 - 150, 300, 300, 50); // Input Box

    iSetColor(255, 255, 255);
    iRectangle(screenWidth / 2 - 150, 300, 300, 50);
    iText(screenWidth / 2 - 140, 320, playerName, GLUT_BITMAP_HELVETICA_18);
}

void drawLevelSelectScreen()
{
    iSetColor(255, 255, 255);
    iText(screenWidth / 2 - 80, 450, "Select a Level", GLUT_BITMAP_TIMES_ROMAN_24);

    for (int i = 1; i <= 5; i++)
    {
        int x = screenWidth / 2 - 150 + ((i - 1) % 3) * 100;
        int y = 350 - ((i - 1) / 3) * 100;

        iSetColor(0, 150, 0);
        iFilledRectangle(x, y, 80, 40);
        iSetColor(255, 255, 255);

        char levelText[10];
        sprintf(levelText, "Level %d", i);
        iText(x + 15, y + 15, levelText, GLUT_BITMAP_HELVETICA_18);
    }
}

void drawSpaceshipSelectScreen()
{
    iSetColor(255, 255, 255);
    iText(screenWidth / 2 - 100, 450, "Choose Your Spaceship", GLUT_BITMAP_TIMES_ROMAN_24);

    for (int i = 0; i < totalShips; i++)
    {
        int x = 200 + i * 150;
        int y = 250;

        iSetColor(100 + i * 50, 100, 200); // Different color for each
        iFilledRectangle(x, y, 100, 100);

        iSetColor(255, 255, 255);
        char label[20];
        sprintf(label, "Ship %d", i + 1);
        iText(x + 25, y - 25, label, GLUT_BITMAP_HELVETICA_18);

        if (selectedShip == i)
        {
            iSetColor(255, 255, 0);
            iRectangle(x - 5, y - 5, 110, 110); // highlight selection
        }
    }

    // "Start Game" button
    if (selectedShip != -1)
    {
        iSetColor(0, 200, 0);
        iFilledRectangle(screenWidth / 2 - 60, 100, 120, 40);
        iSetColor(255, 255, 255);
        iText(screenWidth / 2 - 40, 115, "Start Game", GLUT_BITMAP_HELVETICA_18);
    }
}

void checkBulletEnemyCollision()
{
    for (int i = 0; i < maxBullets; i++)
    {
        if (bulletActive[i])
        {
            for (int j = 0; j < maxEnemies; j++)
            {
                if (enemyActive[j])
                {
                    if (bulletX[i] >= enemyX[j] && bulletX[i] <= enemyX[j] + 40 &&
                        bulletY[i] >= enemyY[j] && bulletY[i] <= enemyY[j] + 40)
                    {
                        bulletActive[i] = false;
                        enemyActive[j] = false;
                        score += 10 * scoreMultiplierValue;
                    }
                }
            }
        }
    }

    if (bossActive)
    {
        for (int i = 0; i < maxBullets; i++)
        {
            if (bulletActive[i])
            {
                if (bulletX[i] >= bossX && bulletX[i] <= bossX + bossWidth &&
                    bulletY[i] >= bossY && bulletY[i] <= bossY + bossHeight)
                {

                    bulletActive[i] = false;
                    bossHealth--;

                    if (bossHealth <= 0)
                    {
                        bossActive = false;
                        score += 500; // Bonus score for killing boss
                        printf("BOSS DEFEATED!\n");
                    }
                }
            }
        }
    }
}

void updateBullets()
{
    if (isPaused)
        return;
    for (int i = 0; i < maxBullets; i++)
    {
        if (bulletActive[i])
        {
            int oldY = bulletY[i];
            bulletY[i] += bulletSpeed;
            printf("[DEBUG] updateBullets: bullet %d moved from %d to %d\n", i, oldY, bulletY[i]);
            if (bulletY[i] > screenHeight)
            {
                bulletActive[i] = false;
                printf("[DEBUG] updateBullets: bullet %d deactivated (out of screen)\n", i);
            }
        }
    }

    checkBulletEnemyCollision();
}

void gameLoop(void)
{
    printf("[DEBUG] gameLoop check: menu=%d, paused=%d, gameOver=%d\n", currentMenu, isPaused, isGameOver);
    if (currentMenu == 4 && !isPaused && !isGameOver)
    {
        printf("[DEBUG] gameLoop running: updating bullets and enemies\n");
        updateBullets();
        updateEnemies();
    }
}

void drawGameScreen()
{

    printf("[DEBUG] drawGameScreen called, selectedLevel: %d\n", selectedLevel);
    if (selectedLevel == 1)
    {
        iSetColor(10, 10, 30);
        iFilledRectangle(0, 0, screenWidth, screenHeight);
    }
    else if (selectedLevel == 2)
    {
        iSetColor(20, 50, 80);
        iFilledRectangle(0, 0, screenWidth, screenHeight);
    }
    else if (selectedLevel == 3)
    {
        iSetColor(50, 20, 50);
        iFilledRectangle(0, 0, screenWidth, screenHeight);
    }
    else if (selectedLevel == 4)
    {
        iSetColor(70, 40, 10);
        iFilledRectangle(0, 0, screenWidth, screenHeight);
    }
    else
    {
        iSetColor(5, 5, 5); // Darkest for Level 5 (Boss Level)
        iFilledRectangle(0, 0, screenWidth, screenHeight);
        iSetColor(255, 0, 0);
        iText(screenWidth / 2 - 50, screenHeight / 2, "BOSS LEVEL", GLUT_BITMAP_HELVETICA_18);
    }

    // // Background
    // iSetColor(10, 10, 30);
    // iFilledRectangle(0, 0, screenWidth, screenHeight);

    // Draw player spaceship
    iSetColor(255, 100, 0);
    iFilledTriangle(playerX, playerY,
                    playerX + 40, playerY + 60,
                    playerX + 80, playerY);

    // Draw bullets
    iSetColor(255, 255, 0);
    for (int i = 0; i < maxBullets; i++)
    {
        if (bulletActive[i])
        {
            printf("[DEBUG] Drawing bullet %d at (%d, %d)\n", i, bulletX[i], bulletY[i]);
            iFilledRectangle(bulletX[i], bulletY[i], 5, 10);
        }
    }

    // Player Info
    iSetColor(255, 255, 255);
    char info[100];
    sprintf(info, "Player: %s   Level: %d", playerName, selectedLevel);
    iText(10, screenHeight - 30, info, GLUT_BITMAP_HELVETICA_18);

    // Draw Enemies
    for (int i = 0; i < maxEnemies; i++)
    {
        if (enemyActive[i])
        {
            printf("[DEBUG] Drawing enemy %d at (%d, %d)\n", i, enemyX[i], enemyY[i]);
            if (enemyType[i] == NORMAL)
                iSetColor(200, 0, 0);
            else if (enemyType[i] == SHOOTER)
                iSetColor(255, 100, 0);
            else if (enemyType[i] == FAST)
                iSetColor(0, 200, 200);

            iFilledRectangle(enemyX[i], enemyY[i], 40, 40);
        }
    }

    if (healthPackActive)
    {
        iSetColor(0, 255, 0); // Green box
        iFilledRectangle(healthPackX, healthPackY, healthPackSize, healthPackSize);

        iSetColor(255, 255, 255);
        iText(healthPackX + 5, healthPackY + 10, "+HP", GLUT_BITMAP_HELVETICA_12);
    }

    char scoreStr[50];
    sprintf(scoreStr, "Score: %d", score);
    iText(10, screenHeight - 50, scoreStr, GLUT_BITMAP_HELVETICA_18);

    // Health Bar
    iSetColor(255, 255, 255);
    iText(screenWidth - 180, screenHeight - 30, "Health:", GLUT_BITMAP_HELVETICA_18);

    iSetColor(0, 255, 0);
    iFilledRectangle(screenWidth - 110, screenHeight - 35, playerHealth, 15);

    // Life Icons (already present)
    iSetColor(255, 255, 255);
    iText(10, screenHeight - 60, "Lives:", GLUT_BITMAP_HELVETICA_18);
    for (int i = 0; i < playerLife; i++)
    {
        iSetColor(255, 0, 0);
        iFilledRectangle(80 + i * 20, screenHeight - 65, 15, 15);
    }

    if (multiplierActive)
    {
        iSetColor(255, 215, 0); // Gold color
        iFilledRectangle(multiplierX, multiplierY, multiplierSize, multiplierSize);

        iSetColor(0, 0, 0);
        iText(multiplierX + 5, multiplierY + 10, "x3", GLUT_BITMAP_HELVETICA_12);
    }

    if (scoreMultiplierActive)
    {
        char multiText[50];
        sprintf(multiText, "Score x%d", scoreMultiplierValue);
        iText(screenWidth / 2 - 50, screenHeight - 30, multiText, GLUT_BITMAP_HELVETICA_18);
    }

    if (armorPackActive)
    {
        iSetColor(0, 200, 255); // Light blue
        iFilledRectangle(armorX, armorY, armorSize, armorSize);
        iSetColor(255, 255, 255);
        iText(armorX + 5, armorY + 10, "ARM", GLUT_BITMAP_HELVETICA_12);
    }

    if (armorActive)
    {
        iText(screenWidth / 2 - 100, screenHeight - 60, "ARMOR ACTIVE", GLUT_BITMAP_HELVETICA_18);
    }

    if (rapidFirePackActive)
    {
        iSetColor(255, 50, 50); // Red
        iFilledRectangle(rapidX, rapidY, rapidSize, rapidSize);
        iSetColor(255, 255, 255);
        iText(rapidX + 2, rapidY + 10, "SPD", GLUT_BITMAP_HELVETICA_12);
    }

    if (rapidFireActive)
    {
        iText(screenWidth / 2 - 100, screenHeight - 80, "RAPID FIRE!", GLUT_BITMAP_HELVETICA_18);
    }

    if (megaBombPackActive)
    {
        iSetColor(255, 0, 255); // Magenta
        iFilledRectangle(megaBombX, megaBombY, megaBombSize, megaBombSize);
        iSetColor(255, 255, 255);
        iText(megaBombX + 2, megaBombY + 10, "BOMB", GLUT_BITMAP_HELVETICA_12);
    }

    if (bossActive)
    {
        // Boss Body
        iSetColor(200, 0, 200);
        iFilledRectangle(bossX, bossY, bossWidth, bossHeight);

        // Boss Health Bar
        iSetColor(255, 255, 255);
        iText(bossX + bossWidth / 2 - 30, bossY + bossHeight + 10, "BOSS", GLUT_BITMAP_HELVETICA_18);

        iSetColor(255, 0, 0);
        iFilledRectangle(bossX, bossY + bossHeight + 30, bossWidth, 10);

        iSetColor(0, 255, 0);
        iFilledRectangle(bossX, bossY + bossHeight + 30, bossHealth * (bossWidth / 100), 10);

        // Boss Bullets
        for (int i = 0; i < 10; i++)
        {
            if (bossBulletActive[i])
            {
                iSetColor(255, 100, 0);
                iFilledRectangle(bossBulletX[i], bossBulletY[i], 5, 15);
            }
        }
    }

    if (isPaused)
    {
        iSetColor(255, 255, 255);
        iText(screenWidth / 2 - 60, screenHeight / 2, "GAME PAUSED", GLUT_BITMAP_TIMES_ROMAN_24);
    }
}

void spawnPowerUps()
{
    if (!healthPackActive && rand() % 100 < 10)
    {
        healthPackX = rand() % (screenWidth - healthPackSize);
        healthPackY = screenHeight + rand() % 300;
        healthPackActive = true;
    }

    if (!multiplierActive && rand() % 100 < 5)
    {
        multiplierX = rand() % (screenWidth - multiplierSize);
        multiplierY = screenHeight + rand() % 300;
        multiplierActive = true;
    }

    if (!armorPackActive && rand() % 100 < 5)
    {
        armorX = rand() % (screenWidth - armorSize);
        armorY = screenHeight + rand() % 300;
        armorPackActive = true;
    }

    if (!rapidFirePackActive && rand() % 100 < 5)
    {
        rapidX = rand() % (screenWidth - rapidSize);
        rapidY = screenHeight + rand() % 300;
        rapidFirePackActive = true;
    }

    if (!megaBombPackActive && rand() % 100 < 3)
    {
        megaBombX = rand() % (screenWidth - megaBombSize);
        megaBombY = screenHeight + rand() % 300;
        megaBombPackActive = true;
    }
}

void checkBossActivation()
{
    if (selectedLevel == 5 && !bossActive && score >= 200)
    {
        bossActive = true;
        bossHealth = 100;
        printf("BOSS ACTIVATED!\n");
    }
}

void spawnEnemy()
{
    for (int i = 0; i < maxEnemies; i++)
    {
        if (!enemyActive[i])
        {
            enemyX[i] = rand() % (screenWidth - 40);
            enemyY[i] = screenHeight + rand() % 300;
            enemyActive[i] = true;

            int typeChance = rand() % 100;
            if (selectedLevel >= 5)
                enemyType[i] = (typeChance < 30) ? SHOOTER : (typeChance < 70) ? FAST
                                                                               : NORMAL;
            else if (selectedLevel >= 3)
                enemyType[i] = (typeChance < 50) ? NORMAL : FAST;
            else
                enemyType[i] = NORMAL;

            printf("[DEBUG] spawnEnemy: enemy %d spawned at (%d, %d), type: %d\n", i, enemyX[i], enemyY[i], enemyType[i]);
            break;
        }
    }

    spawnPowerUps();
    checkBossActivation();
}

void moveEnemies()
{
    for (int i = 0; i < maxEnemies; i++)
    {
        if (enemyActive[i])
        {
            int thisSpeed = (enemyType[i] == FAST) ? enemySpeed + 2 : enemySpeed;
            enemyY[i] -= thisSpeed;

            if (enemyY[i] < 0)
            {
                enemyActive[i] = false;
                if (!armorActive)
                    playerHealth -= 20;

                if (playerHealth <= 0)
                {
                    playerLife--;
                    playerHealth = maxHealth;
                    if (playerLife <= 0)
                    {
                        isGameOver = true;
                        currentMenu = 5;
                    }
                }
            }
        }
    }
}
void handlePowerUps()
{
    moveAndCheckPickup(&healthPackActive, &healthPackX, &healthPackY, healthPackSize, "+HP", [&]()
                       {
        playerHealth += 30;
        if (playerHealth > maxHealth) playerHealth = maxHealth; });

    moveAndCheckPickup(&multiplierActive, &multiplierX, &multiplierY, multiplierSize, "x3", [&]()
                       {
        scoreMultiplierActive = true;
        scoreMultiplierValue = (rand() % 2 == 0) ? 3 : 5;
        multiplierTimer = 300; });

    moveAndCheckPickup(&armorPackActive, &armorX, &armorY, armorSize, "ARM", [&]()
                       {
        armorActive = true;
        armorTimer = 300; });

    moveAndCheckPickup(&rapidFirePackActive, &rapidX, &rapidY, rapidSize, "SPD", [&]()
                       {
        rapidFireActive = true;
        rapidFireTimer = 200; });

    moveAndCheckPickup(&megaBombPackActive, &megaBombX, &megaBombY, megaBombSize, "BOMB", [&]()
                       { megaBombActive = true; });

    if (scoreMultiplierActive && --multiplierTimer <= 0)
    {
        scoreMultiplierActive = false;
        scoreMultiplierValue = 1;
    }

    if (armorActive && --armorTimer <= 0)
        armorActive = false;
    if (rapidFireActive && --rapidFireTimer <= 0)
        rapidFireActive = false;
}
template <typename Callback>
void moveAndCheckPickup(bool *active, int *x, int *y, int size, const char *label, Callback onPickup)
{
    if (*active)
    {
        *y -= 3;
        if (*y < 0)
        {
            *active = false;
            return;
        }

        if (playerX < *x + size &&
            playerX + 80 > *x &&
            playerY < *y + size &&
            playerY + 60 > *y)
        {
            *active = false;
            onPickup();
        }
    }
}
void handleMegaBomb()
{
    if (megaBombActive)
    {
        for (int i = 0; i < maxEnemies; i++)
        {
            if (enemyActive[i])
            {
                enemyActive[i] = false;
                score += 10 * scoreMultiplierValue;
            }
        }
        megaBombActive = false;
    }
}

void handleBossMechanics()
{
    if (!bossActive)
        return;

    // Move boss bullets
    for (int i = 0; i < 10; i++)
    {
        if (bossBulletActive[i])
        {
            bossBulletY[i] -= 5;
            if (bossBulletY[i] < 0)
                bossBulletActive[i] = false;
        }
    }

    // Boss fires bullet every 60 frames (~2s)
    bossFireTimer++;
    if (bossFireTimer >= 60)
    {
        bossFireTimer = 0;
        for (int i = 0; i < 10; i++)
        {
            if (!bossBulletActive[i])
            {
                bossBulletX[i] = bossX + rand() % bossWidth;
                bossBulletY[i] = bossY;
                bossBulletActive[i] = true;
                break;
            }
        }
    }

    // Collision with player
    for (int i = 0; i < 10; i++)
    {
        if (bossBulletActive[i] &&
            playerX < bossBulletX[i] + 5 &&
            playerX + 80 > bossBulletX[i] &&
            playerY < bossBulletY[i] + 15 &&
            playerY + 60 > bossBulletY[i])
        {

            if (!armorActive)
                playerHealth -= 20;

            bossBulletActive[i] = false;

            if (playerHealth <= 0)
            {
                playerLife--;
                playerHealth = maxHealth;
                if (playerLife <= 0)
                {
                    isGameOver = true;
                    currentMenu = 5;
                }
            }
        }
    }
}

void updateEnemies()
{
    if (isPaused)
        return;
    printf("[DEBUG] updateEnemies called\n");
    moveEnemies();
    handlePowerUps();
    handleMegaBomb();
    handleBossMechanics();
}

void drawGameOverScreen()
{
    iSetColor(255, 0, 0);
    iText(screenWidth / 2 - 60, screenHeight / 2 + 40, "GAME OVER", GLUT_BITMAP_TIMES_ROMAN_24);

    iSetColor(255, 255, 255);
    char msg[100];
    sprintf(msg, "Player: %s   Level: %d", playerName, selectedLevel);
    iText(screenWidth / 2 - 80, screenHeight / 2, msg);

    iText(screenWidth / 2 - 90, screenHeight / 2 - 40, "Press 'R' to Restart or 'Q' to Quit", GLUT_BITMAP_HELVETICA_18);
}

void loadScores()
{
    FILE *fp = fopen("scoreboard.txt", "r");
    scoreCount = 0;
    if (fp != NULL)
    {
        while (fscanf(fp, "%s %d", scores[scoreCount].name, &scores[scoreCount].score) == 2)
        {
            scoreCount++;
        }
        fclose(fp);
    }

    // Sort scores descending
    for (int i = 0; i < scoreCount - 1; i++)
    {
        for (int j = i + 1; j < scoreCount; j++)
        {
            if (scores[i].score < scores[j].score)
            {
                ScoreEntry temp = scores[i];
                scores[i] = scores[j];
                scores[j] = temp;
            }
        }
    }
}

void drawScoreboardScreen()
{
    iSetColor(255, 255, 255);
    iText(screenWidth / 2 - 60, 450, "TOP SCORES", GLUT_BITMAP_TIMES_ROMAN_24);

    for (int i = 0; i < scoreCount && i < 5; i++)
    {
        char line[100];
        sprintf(line, "%d. %s - %d", i + 1, scores[i].name, scores[i].score);
        iText(screenWidth / 2 - 100, 400 - i * 40, line, GLUT_BITMAP_HELVETICA_18);
    }

    iText(screenWidth / 2 - 100, 100, "Press 'B' to go back", GLUT_BITMAP_HELVETICA_18);
}

void drawHelpPage()
{
    iSetColor(255, 255, 255);
    iText(50, 550, "Help / Controls", GLUT_BITMAP_TIMES_ROMAN_24);

    iText(50, 500, "Arrow Keys / WASD: Move Ship", GLUT_BITMAP_HELVETICA_18);
    iText(50, 470, "Space: Shoot", GLUT_BITMAP_HELVETICA_18);
    iText(50, 440, "P: Pause / Resume", GLUT_BITMAP_HELVETICA_18);
    iText(50, 410, "M: Toggle Sound", GLUT_BITMAP_HELVETICA_18);

    iText(50, 370, "Power-Ups:");
    iText(70, 340, "+HP: Restore Health");
    iText(70, 320, "x3/x5: Score Multiplier");
    iText(70, 300, "ARM: Temporary Invincibility");
    iText(70, 280, "SPD: Rapid Fire");
    iText(70, 260, "BOMB: Mega Bomb (Destroys all enemies)");

    iText(50, 100, "Press 'B' to return to Menu", GLUT_BITMAP_HELVETICA_18);
}

void iDraw()
{
    iClear();

    if (currentMenu == 0)
    {
        drawMainMenu();
    }

    else if (currentMenu == 1)
    {
        drawNameInputScreen();
    }

    else if (currentMenu == 2)
    {
        drawLevelSelectScreen();
    }
    else if (currentMenu == 3)
    {
        drawSpaceshipSelectScreen();
    }

    else if (currentMenu == 4)
    {
        drawGameScreen();
    }

    else if (currentMenu == 5)
    {
        drawGameOverScreen();
    }

    else if (currentMenu == 6)
    {
        drawScoreboardScreen();
    }

    else if (currentMenu == 7)
    {
        drawHelpPage();
    }

    // future: else if (currentMenu == 1) --> Game Page etc.
}

void iMouse(int button, int state, int mx, int my)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {

        // Main Menu
        if (currentMenu == 0)
        {
            for (int i = 0; i < menuCount; i++)
            {
                int y = buttonYStart - i * buttonGap;
                if (mx >= buttonX && mx <= buttonX + buttonWidth &&
                    my >= y && my <= y + buttonHeight)
                {

                    printf("Clicked on: %s\n", menuItems[i]);

                    if (i == 0)
                    {
                        // New Game
                        currentMenu = 1;
                        nameCharIndex = 0;
                        playerName[0] = '\0';
                    }

                    else if (i == 1)
                    {
                        // Scoreboard
                        loadScores();
                        currentMenu = 6;
                    }

                    else if (i == 2)
                    { // Help
                        currentMenu = 7;
                    }

                    else if (i == 4)
                    {
                        // Exit
                        exit(0);
                    }
                }
            }
        }

        // Level Select
        else if (currentMenu == 2)
        {
            for (int i = 1; i <= 5; i++)
            {
                int x = screenWidth / 2 - 150 + ((i - 1) % 3) * 100;
                int y = 350 - ((i - 1) / 3) * 100;

                if (mx >= x && mx <= x + 80 && my >= y && my <= y + 40)
                {
                    selectedLevel = i;
                    currentMenu = 3;
                    printf("Level %d selected\n", selectedLevel);
                }
            }
        }

        // Ship Select
        else if (currentMenu == 3)
        {
            for (int i = 0; i < totalShips; i++)
            {
                int x = 200 + i * 150;
                int y = 250;
                if (mx >= x && mx <= x + 100 && my >= y && my <= y + 100)
                {
                    selectedShip = i;
                    printf("Selected Ship: %d\n", selectedShip + 1);
                }
            }

            // ✅ এখানে enemy & bullet reset করো
            for (int j = 0; j < maxEnemies; j++)
                enemyActive[j] = false;
            for (int j = 0; j < maxBullets; j++)
                bulletActive[j] = false;
            bulletCount = 0;

            // Start Game
            if (selectedShip != -1)
            {
                int x = screenWidth / 2 - 60;
                int y = 100;
                if (mx >= x && mx <= x + 120 && my >= y && my <= y + 40)
                {
                    currentMenu = 4;
                    enemySpeed = 2 + selectedLevel;
                    initializeBossSystem();
                    printf("Game starting...\n");
                }
            }
        }
    }
}

void iKeyboard(unsigned char key)
{
    // Name Input Screen
    if (currentMenu == 1)
    {
        if (key == '\r')
        {
            currentMenu = 2;
            printf("Player Name: %s\n", playerName);
        }
        else if (key == '\b')
        {
            if (nameCharIndex > 0)
            {
                nameCharIndex--;
                playerName[nameCharIndex] = '\0';
            }
        }
        else
        {
            if (nameCharIndex < 99)
            {
                playerName[nameCharIndex] = key;
                nameCharIndex++;
                playerName[nameCharIndex] = '\0';
            }
        }
    }

    // Game screen (shoot)
    else if (currentMenu == 4)
    {
        if (key == ' ')
        {
            int bulletsPerShot = rapidFireActive ? 3 : 1;
            printf("[DEBUG] Space pressed. Bullets per shot: %d\n", bulletsPerShot);

            int bulletsFired = 0;
            for (int i = 0; i < maxBullets && bulletsFired < bulletsPerShot; i++)
            {
                if (!bulletActive[i])
                {
                    bulletX[i] = playerX + 37 + (bulletsFired * 5);
                    bulletY[i] = playerY + 60;
                    bulletActive[i] = true;
                    printf("[DEBUG] Bullet fired at (%d, %d) [slot: %d]\n", bulletX[i], bulletY[i], i);
                    bulletsFired++;
                }
            }
        }

        if (key == 'p')
        {
            isPaused = !isPaused; // Toggle Pause
        }
    }

    // Game Over screen
    else if (currentMenu == 5)
    {
        // Save score to file
        FILE *fp = fopen("scoreboard.txt", "a");
        if (fp != NULL)
        {
            fprintf(fp, "%s %d\n", playerName, score);
            fclose(fp);
        }

        if (key == 'r')
        {
            currentMenu = 0;
            playerLife = 3;
            isGameOver = false;
            bulletCount = 0;
            selectedLevel = 0;
            selectedShip = -1;
            score = 0;

            for (int i = 0; i < maxEnemies; i++)
                enemyActive[i] = false;
            for (int i = 0; i < maxBullets; i++)
                bulletActive[i] = false;
        }

        else if (key == 'q')
        {
            exit(0);
        }
    }

    // Scoreboard back
    else if (currentMenu == 6)
    {
        if (key == 'b')
        {
            currentMenu = 0;
        }
    }

    else if (currentMenu == 7)
    {
        if (key == 'b' || key == 'B')
        {
            currentMenu = 0;
        }
    }

    // Global Quit
    if (key == 'q')
    {
        exit(0);
    }

    // Global Quit
    if (key == 'q')
    {
        exit(0);
    }
}

void iSpecialKeyboard(unsigned char key)
{
    if (currentMenu == 4)
    {
        if (key == GLUT_KEY_LEFT && playerX > 0)
            playerX -= playerSpeed;
        if (key == GLUT_KEY_RIGHT && playerX < screenWidth - 80)
            playerX += playerSpeed;
        if (key == GLUT_KEY_UP && playerY < screenHeight - 100)
            playerY += playerSpeed;
        if (key == GLUT_KEY_DOWN && playerY > 0)
            playerY -= playerSpeed;
    }
}

// Dummy implementations to satisfy linker
void iMouseDrag(int mx, int my) {}
void iMouseMove(int mx, int my) {}
void iMouseWheel(int button, int dir, int mx) {}

int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    iInitialize(screenWidth, screenHeight, "Space Shooter - Main Menu");
    iSetTimer(16, gameLoop); 
    iSetTimer(500, spawnEnemy);   
    printf("[DEBUG] Main initialized. Timers set.\n");
    return 0;
}

