#include "../general.h"
#include "includes/games.h"
#include "../input/includes/input.h"
#include "../display/includes/tft.h"

const int BOARD_WIDTH  = 10;
const int BOARD_HEIGHT = 14;
const int CELL_SIZE    = 9;

const int FIGURES[7][4][4][4] = {
    {{{1,1,0,0},{1,1,0,0},{0,0,0,0},{0,0,0,0}},
     {{1,1,0,0},{1,1,0,0},{0,0,0,0},{0,0,0,0}},
     {{1,1,0,0},{1,1,0,0},{0,0,0,0},{0,0,0,0}},
     {{1,1,0,0},{1,1,0,0},{0,0,0,0},{0,0,0,0}}},
    {{{0,0,0,0},{1,1,1,1},{0,0,0,0},{0,0,0,0}},
     {{0,1,0,0},{0,1,0,0},{0,1,0,0},{0,1,0,0}},
     {{0,0,0,0},{1,1,1,1},{0,0,0,0},{0,0,0,0}},
     {{0,1,0,0},{0,1,0,0},{0,1,0,0},{0,1,0,0}}},
    {{{0,1,0,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}},
     {{1,0,0,0},{1,1,0,0},{1,0,0,0},{0,0,0,0}},
     {{1,1,1,0},{0,1,0,0},{0,0,0,0},{0,0,0,0}},
     {{0,1,0,0},{1,1,0,0},{0,1,0,0},{0,0,0,0}}},
    {{{1,0,0,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}},
     {{1,1,0,0},{1,0,0,0},{1,0,0,0},{0,0,0,0}},
     {{1,1,1,0},{0,0,1,0},{0,0,0,0},{0,0,0,0}},
     {{0,1,0,0},{0,1,0,0},{1,1,0,0},{0,0,0,0}}},
    {{{0,0,1,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}},
     {{1,0,0,0},{1,0,0,0},{1,1,0,0},{0,0,0,0}},
     {{1,1,1,0},{1,0,0,0},{0,0,0,0},{0,0,0,0}},
     {{1,1,0,0},{0,1,0,0},{0,1,0,0},{0,0,0,0}}},
    {{{0,1,1,0},{1,1,0,0},{0,0,0,0},{0,0,0,0}},
     {{1,0,0,0},{1,1,0,0},{0,1,0,0},{0,0,0,0}},
     {{0,1,1,0},{1,1,0,0},{0,0,0,0},{0,0,0,0}},
     {{1,0,0,0},{1,1,0,0},{0,1,0,0},{0,0,0,0}}},
    {{{1,1,0,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}},
     {{0,1,0,0},{1,1,0,0},{1,0,0,0},{0,0,0,0}},
     {{1,1,0,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}},
     {{0,1,0,0},{1,1,0,0},{1,0,0,0},{0,0,0,0}}},
};

void tetris() {
    uint8_t board[BOARD_HEIGHT][BOARD_WIDTH];
    memset(board, 0, sizeof(board));

    int fx = BOARD_WIDTH/2 - 2, fy = -1;
    int prevFx = -99, prevFy = -99, prevRot = -1;
    int fig = random(0, 7), rot = 0;

    auto hit = [&](int f, int r, int x, int y) -> bool {
        for (int py = 0; py < 4; py++)
            for (int px = 0; px < 4; px++) {
                if (!FIGURES[f][r][py][px]) continue;
                int nx = x+px, ny = y+py;
                if (nx < 0 || nx >= BOARD_WIDTH || ny >= BOARD_HEIGHT) return true;
                if (ny >= 0 && board[ny][nx]) return true;
            }
        return false;
    };

    auto drawCell = [&](int x, int y, bool filled) {
        if (filled) {
            drawTetrisFigure(x*CELL_SIZE, y*CELL_SIZE, CELL_SIZE, CELL_SIZE, C_TERM_DIM);
        } else {
            drawTetrisFigure(x*CELL_SIZE, y*CELL_SIZE, CELL_SIZE, CELL_SIZE, 0x0000);
            drawTetrisMatrix(x*CELL_SIZE, y*CELL_SIZE, CELL_SIZE, CELL_SIZE, C_TERM_LINE);
        }
    };

    auto erasePrev = [&]() {
        if (prevRot < 0) return;
        for (int py = 0; py < 4; py++)
            for (int px = 0; px < 4; px++) {
                if (!FIGURES[fig][prevRot][py][px]) continue;
                int ox = prevFx+px, oy = prevFy+py;
                if (ox < 0 || ox >= BOARD_WIDTH || oy < 0 || oy >= BOARD_HEIGHT) continue;
                bool coveredByNew = false;
                for (int ny2 = 0; ny2 < 4 && !coveredByNew; ny2++)
                    for (int nx2 = 0; nx2 < 4 && !coveredByNew; nx2++)
                        if (FIGURES[fig][rot][ny2][nx2] && (fx+nx2)==ox && (fy+ny2)==oy)
                            coveredByNew = true;
                if (!coveredByNew)
                    drawCell(ox, oy, board[oy][ox]);
            }
    };

    auto drawFig = [&]() {
        for (int py = 0; py < 4; py++)
            for (int px = 0; px < 4; px++) {
                if (!FIGURES[fig][rot][py][px]) continue;
                int nx = fx+px, ny = fy+py;
                if (nx < 0 || nx >= BOARD_WIDTH || ny < 0 || ny >= BOARD_HEIGHT) continue;
                drawTetrisFigure(nx*CELL_SIZE, ny*CELL_SIZE, CELL_SIZE, CELL_SIZE, C_TERM_ON);
            }
    };

    tftClear();
    for (int y = 0; y < BOARD_HEIGHT; y++)
        for (int x = 0; x < BOARD_WIDTH; x++)
            drawCell(x, y, false);

    unsigned long lastDrop = millis();

    while (true) {
        erasePrev();
        drawFig();
        prevFx = fx; prevFy = fy; prevRot = rot;

        if (setMapJoyYMenuModeSimple() == 1) break;

        if (setMapJoyXMenuModeSimple() == 1 && !hit(fig, rot, fx-1, fy)) { 
            fx--; 
            delay(100); 
        }
        if (getClickJoyButton() && !hit(fig, rot, fx+1, fy)) { 
            fx++; 
            delay(100); 
        }

        if (setMapJoyYMenuModeSimple() < 0) {
            int nr = (rot+1) % 4;
            if (!hit(fig, nr, fx, fy)) rot = nr;
            delay(150);
        }
        if (setMapJoyYMenuModeSimple() > 0 && !hit(fig, rot, fx, fy+1)) { fy++; delay(60); }

        if (millis() - lastDrop > 400) {
            lastDrop = millis();
            if (!hit(fig, rot, fx, fy+1)) {
                fy++;
            } else {
                for (int py = 0; py < 4; py++)
                    for (int px = 0; px < 4; px++)
                        if (FIGURES[fig][rot][py][px] && (fy+py) >= 0) {
                            board[fy+py][fx+px] = 1;
                            drawCell(fx+px, fy+py, true);
                        }

                for (int y = BOARD_HEIGHT-1; y >= 0; y--) {
                    bool full = true;
                    for (int x = 0; x < BOARD_WIDTH; x++)
                        if (!board[y][x]) { full = false; break; }
                    if (full) {
                        for (int ny = y; ny > 0; ny--)
                            for (int x = 0; x < BOARD_WIDTH; x++)
                                board[ny][x] = board[ny-1][x];
                        memset(board[0], 0, BOARD_WIDTH);
                        for (int ny = 0; ny <= y; ny++)
                            for (int x = 0; x < BOARD_WIDTH; x++)
                                drawCell(x, ny, board[ny][x]);
                        y++;
                    }
                }

                fig = random(0, 7); rot = 0;
                fx = BOARD_WIDTH/2 - 2; fy = -1;
                prevRot = -1;

                if (hit(fig, rot, fx, fy)) {
                    tftClear();
                    while (setMapJoyYMenuModeSimple() != 1) delay(100);
                    break;
                }
            }
        }

        delay(20);
    }
}