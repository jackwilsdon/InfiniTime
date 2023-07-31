#include "displayapp/screens/TicTacToe.h"
#include <cstdint>
#include <cstring>
#include <lvgl/lvgl.h>

using namespace Pinetime::Applications::Screens;

namespace {
  void event_handler(lv_obj_t *object, lv_event_t event) {
    if (event != LV_EVENT_VALUE_CHANGED) {
      return;
    }

    const uint32_t* eventData = static_cast<const uint32_t*>(lv_event_get_data());
    static_cast<TicTacToe*>(object->user_data)->Play(*eventData);
  }
}

TicTacToe::TicTacToe() {
  buttons = lv_btnmatrix_create(lv_scr_act(), nullptr);
  lv_btnmatrix_set_map(buttons, buttonMap);
  lv_obj_set_size(buttons, LV_HOR_RES, LV_VER_RES);
  lv_obj_align(buttons, nullptr, LV_ALIGN_CENTER, 0, 0);

  buttons->user_data = this;
  lv_obj_set_event_cb(buttons, event_handler);

  gameOverLabel = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_hidden(gameOverLabel, true);

  UpdateButtons();
}

TicTacToe::~TicTacToe() {
  lv_obj_clean(lv_scr_act());
}

void TicTacToe::Play(uint32_t index) {
  state[index] = 'X';
  UpdateButtons();
  int possibleMoves = GetPossibleMoves(state, nullptr);
  char winner = GetWinner(state);
  if (possibleMoves == 0 || winner != ' ') {
    GameOver(winner);
    return;
  }

  MinMax(state, 'O', 0);
  UpdateButtons();
  possibleMoves = GetPossibleMoves(state, nullptr);
  winner = GetWinner(state);
  if (possibleMoves == 0 || winner != ' ') {
    GameOver(winner);
  }
}

bool TicTacToe::OnButtonPushed() {
  if (lv_obj_get_hidden(gameOverLabel)) {
    // Game is still playing.
    return false;
  }

  // Reset game.
  lv_obj_set_hidden(gameOverLabel, true);
  for (int i = 0; i < 9; i++) {
    state[i] = ' ';
  }
  UpdateButtons();
  lv_obj_set_style_local_bg_opa(buttons, LV_BTNMATRIX_PART_BTN, LV_STATE_DEFAULT, LV_OPA_100);
  lv_obj_set_style_local_text_opa(buttons, LV_BTNMATRIX_PART_BTN, LV_STATE_DEFAULT, LV_OPA_100);
  return true;
}

char TicTacToe::GetWinner(char state[9]) {
  // Horizontal and vertical
  for (int i = 0; i < 3; i++) {
    int rowIndex = i * 3;
    if (state[rowIndex] != ' ' && state[rowIndex] == state[rowIndex + 1] && state[rowIndex] == state[rowIndex + 2]) {
      // Row
      return state[rowIndex];
    } else if (state[i] != ' ' && state[i] == state[i + 3] && state[i] == state[i + 6]) {
      // Column
      return state[i];
    }
  }

  // Diagonal
  if (
    state[4] != ' ' &&
    (
      // Top-left to bottom-right
      (state[4] == state[0] && state[4] == state[8]) ||
      // Top-right to bottom-left
      (state[4] == state[2] && state[4] == state[6])
    )
   ) {
    return state[4];
  }

  return ' ';
}

int TicTacToe::GetPossibleMoves(char state[9], int moves[9]) {
  int numberOfMoves = 0;
  for (int i = 0; i < 9; i++) {
    if (state[i] == ' ') {
      if (moves != nullptr) {
        moves[numberOfMoves] = i;
      }
      numberOfMoves++;
    }
  }
  return numberOfMoves;
}

int TicTacToe::MinMax(char state[9], char player, int turn) {
  int possibleMoves[9];
  int possibleMoveCount = GetPossibleMoves(state, possibleMoves);
  char winner = GetWinner(state);
  if (possibleMoveCount == 0 || winner != ' ') {
    if (winner == ' ') {
      return 0;
    } else {
      return winner == 'X' ? turn - 10 : 10 - turn;
    }
  }

  int bestMove = 0;
  int bestMoveScore = player == 'X' ? 1 : -1;
  for (int i = 0; i < possibleMoveCount; i++) {
    int possibleMove = possibleMoves[i];
    state[possibleMove] = player;
    int possibleScore = MinMax(state, player == 'X' ? 'O' : 'X', turn + 1);
    state[possibleMove] = ' ';
    if (
      (player == 'X' && possibleScore < bestMoveScore) ||
      (player == 'O' && possibleScore > bestMoveScore)
    ) {
      bestMove = possibleMove;
      bestMoveScore = possibleScore;
    }
  }
  if (turn == 0) {
    state[bestMove] = player;
  }
  return bestMoveScore;
}

void TicTacToe::UpdateButtons() {
  for (int i = 0; i < 9; i++) {
    // buttonMap has a newline after every third button. We don't want to
    // update these, so we add 1 for every row to skip them.
    int offset = i / 3;
    switch (state[i]) {
    case ' ':
      buttonMap[i + offset] = " ";
      lv_btnmatrix_clear_btn_ctrl(buttons, i, LV_BTNMATRIX_CTRL_DISABLED);
      break;
    case 'X':
      buttonMap[i + offset] = "X";
      lv_btnmatrix_set_btn_ctrl(buttons, i, LV_BTNMATRIX_CTRL_DISABLED);
      break;
    case 'O':
      buttonMap[i + offset] = "O";
      lv_btnmatrix_set_btn_ctrl(buttons, i, LV_BTNMATRIX_CTRL_DISABLED);
      break;
    }
  }
}

void TicTacToe::GameOver(char winner) {
  lv_obj_set_style_local_bg_opa(buttons, LV_BTNMATRIX_PART_BTN, LV_STATE_DEFAULT, LV_OPA_50);
  lv_obj_set_style_local_text_opa(buttons, LV_BTNMATRIX_PART_BTN, LV_STATE_DEFAULT, LV_OPA_50);
  if (winner == ' ') {
    lv_label_set_text(gameOverLabel, "Draw!");
  } else {
    lv_label_set_text_fmt(gameOverLabel, "%c wins!", winner);
  }
  lv_obj_align(gameOverLabel, nullptr, LV_ALIGN_CENTER, 0, 0);
  lv_obj_set_hidden(gameOverLabel, false);
}
