#pragma once

#include "displayapp/screens/Screen.h"
#include <cstdint>
#include <lvgl/lvgl.h>

namespace Pinetime {
  namespace Applications {
    namespace Screens {

      class TicTacToe : public Screen {
      public:
        TicTacToe();
        ~TicTacToe() override;

        void Play(uint32_t index);
        bool OnButtonPushed() override;

      private:
        char state[9] = {
          ' ', ' ', ' ',
          ' ', ' ', ' ',
          ' ', ' ', ' ',
        };
        lv_obj_t *buttons;
        const char* buttonMap[12] = {
          " ", " ", " ", "\n",
          " ", " ", " ", "\n",
          " ", " ", " ", "",
        };
        lv_obj_t *gameOverLabel;
        static char GetWinner(char state[9]);
        static int GetPossibleMoves(char state[9], int moves[9]);
        static int MinMax(char state[9], char player, int turn, int alpha, int beta);
        void UpdateButtons();
        void GameOver(char winner);
      };
    }
  }
}
