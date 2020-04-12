/**
* Conway's Game of Life
*
* Controls:
*
*   'S' = start/stop simulation
*   'Z' = slow down simulation
*   'X' = speed up simulation
*   'C' = clear the board
*   Left Mouse Button = Set a cell -- make a cell alive
*   Right Mouse Button  = Clear a cell -- kill a cell -- make it dead
*/
#include <stdbool.h>
#include "raylib.h"

typedef enum {
    CellDead,
    CellAlive,
    CellAliveDeadNext,
    CellDeadAliveNext
} Cell;

int main() {
    const unsigned int screenWidth = 800;
    const unsigned int screenHeight = 640;
    const unsigned int cellSize = 10;
    const unsigned int brdWidth = screenWidth / cellSize;
    const unsigned int brdHeight = screenHeight / cellSize;
    double timeBetweenUpdatesSec = 0.25;
    const double slowestUpdateSec = 2.0;
    const double fastestUpdateSec = 0.020;
    const double updateStepSec = 0.040;
    bool updateRunning = true;          // Toggled using the 'S' key
    bool mouseButtonPressed = false;
    
    Cell brd[brdWidth][brdHeight];
    
    for (int c = 0; c < brdWidth; c++) {
        for (int r = 0; r < brdHeight; r++) {
           brd[c][r] = CellDead;
        }
    }
    
    brd[5][5] = CellAlive;
    brd[5][6] = CellAlive;
    brd[5][7] = CellAlive;

    brd[10][10] = CellAlive;
    brd[11][10] = CellAlive;
    brd[12][10] = CellAlive;
    brd[9][11] = CellAlive;
    brd[10][11] = CellAlive;
    brd[11][11] = CellAlive;

    brd[1][1] = CellAlive;
    brd[2][2] = CellAlive;
    brd[2][3] = CellAlive;
    brd[1][3] = CellAlive;
    brd[0][3] = CellAlive;

    InitWindow(screenWidth, screenHeight, "Conway's Game of Life ('C' clear, 'X' faster, 'Z' slower, 'S' stop/start, 'LB' set, 'RB' clear)");
    SetTargetFPS(60);

    Vector2 mousePoint;

    double prevUpdateTime = 0;
    double nowTime = 0;
    
    // cell rectangle where the mouse pointer currently is
    Rectangle mouseRect;
    mouseRect.width = cellSize;
    mouseRect.height = cellSize;
    
    // rectangle for drawing cells
    Rectangle cellRect;
    cellRect.width = cellSize;
    cellRect.height = cellSize;

    // Main game loop
    while (!WindowShouldClose()) {
        // Update
        mousePoint = GetMousePosition();
        mouseRect.x = (unsigned int )((unsigned int )mousePoint.x / cellSize) * cellSize;
        mouseRect.y = (unsigned int )((unsigned int )mousePoint.y / cellSize) * cellSize;

        // Slow down simulation update
        if (IsKeyPressed(KEY_Z)) {
            timeBetweenUpdatesSec += updateStepSec;
            if (timeBetweenUpdatesSec > slowestUpdateSec) {
                timeBetweenUpdatesSec = slowestUpdateSec;
            }
        }
        // Speed up simulation update
        if (IsKeyPressed(KEY_X)) {
            timeBetweenUpdatesSec -= updateStepSec;
            if (timeBetweenUpdatesSec < fastestUpdateSec) {
                timeBetweenUpdatesSec = fastestUpdateSec;
            }
        }
        // Clear the board
        if (IsKeyPressed(KEY_C)) {
            for (int c = 0; c < brdWidth; c++) {
                for (int r = 0; r < brdHeight; r++) {  
                    brd[c][r] = CellDead;
                }
            }
        }
        // Start/stop simulation
        if (IsKeyPressed(KEY_S)) {
            updateRunning = !updateRunning;
        }

        // Set/Clear cells. While mouse buttons are pressed simulation is paused.
        mouseButtonPressed = false;
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) || IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) {
            unsigned int c = mouseRect.x / cellSize;
            unsigned int r = mouseRect.y / cellSize;
            mouseButtonPressed = true;
            brd[c][r] = IsMouseButtonDown(MOUSE_LEFT_BUTTON) ? CellAlive : CellDead;
        }

        nowTime = GetTime();
        if (updateRunning && !mouseButtonPressed && nowTime - prevUpdateTime >= timeBetweenUpdatesSec) {
            prevUpdateTime = nowTime;
            for (int c = 0; c < brdWidth; c++) {
                for (int r = 0; r < brdHeight; r++) {  
                    unsigned int numAlive = 0;
                    unsigned int rp = ((r - 1) < 0)                 ? brdHeight - 1 : r - 1;    // previous row
                    unsigned int rn = ((r + 1) > (brdHeight - 1))   ? 0             : r + 1;    // next row
                    unsigned int cp = ((c - 1) < 0)                 ? brdWidth - 1  : c - 1;    // previous column
                    unsigned int cn = ((c + 1) > (brdWidth - 1))    ? 0             : c + 1;    // next column
                    if (CellAlive == brd[cp][rp] || CellAliveDeadNext == brd[cp][rp]) {
                        numAlive++;
                    }
                    if (CellAlive == brd[c][rp] || CellAliveDeadNext == brd[c][rp]) {
                        numAlive++;
                    }
                    if (CellAlive == brd[cn][rp] || CellAliveDeadNext == brd[cn][rp]) {
                        numAlive++;
                    }
                    if (CellAlive == brd[cp][r] || CellAliveDeadNext == brd[cp][r]) {
                        numAlive++;
                    }
                    if (CellAlive == brd[cn][r] || CellAliveDeadNext == brd[cn][r]) {
                        numAlive++;
                    }
                    if (CellAlive == brd[cp][rn] || CellAliveDeadNext == brd[cp][rn]) {
                        numAlive++;
                    }
                    if (CellAlive == brd[c][rn] || CellAliveDeadNext == brd[c][rn]) {
                        numAlive++;
                    }
                    if (CellAlive == brd[cn][rn] || CellAliveDeadNext == brd[cn][rn]) {
                        numAlive++;
                    }
                    
                    if (CellAlive == brd[c][r] && (2 == numAlive || 3 == numAlive)) {
                        // Cell with two or three live neighbors survives
                    }
                    else if (CellDead == brd[c][r] && (3 == numAlive)) {
                        // Cell with three live neighbors becomes a live cell
                        brd[c][r] = CellDeadAliveNext;
                    }
                    else {
                        // All other live cells die in the next generation
                        if (CellAlive == brd[c][r]) {
                            brd[c][r] = CellAliveDeadNext;
                        }
                    }
                }
            }
        }

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing(); {
            ClearBackground(RAYWHITE);
            for (int c = 0; c < brdWidth; c++) {
                for (int r = 0; r < brdHeight; r++) {  
                    if (CellAlive == brd[c][r] || CellDeadAliveNext == brd[c][r]) {
                        // Mark as alive for the next upate
                        brd[c][r] = CellAlive;
                        cellRect.x = c * cellSize;
                        cellRect.y = r * cellSize;
                        DrawRectangleRec(cellRect, DARKGRAY);
                    }
                    else {
                        brd[c][r] = CellDead;
                    }
                }
            }
            if (mouseRect.x < screenWidth && mouseRect.y < screenHeight)
            {
                DrawRectangleLinesEx(mouseRect, 1, LIGHTGRAY);
            }
        }
        EndDrawing();
    }

    // De-Initialization
    CloseWindow();                // Close window and OpenGL context
    return 0;
}