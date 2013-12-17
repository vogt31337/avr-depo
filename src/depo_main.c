#include "depo_main.h"

#include "util.h"
#include "display.h"
#include "ui.h"

#include <stdint.h>
#include <stdlib.h>

static const uint8_t MIN_ROWS = 1;
static const uint8_t MIN_COLS = 8;
static const uint8_t PASS_MAX_LEN = 128;

static uint8_t g_aborted = 0;

static void err(const char *msg) {
  ADP_debug_print(msg);
  ADP_debug_print("\r\n");
  g_aborted = 1;
}

int generate_key(uint8_t *key) {
  uint8_t pass[PASS_MAX_LEN];
  size_t len;

  while(1) {
    ADP_display_clear();
    display_print(0, 0, "key:");
    len = ui_input(4, 0, pass, sizeof(pass), UI_INPUT_OPTS_HIDE);
    if(len < 10) {
      ADP_display_clear();
      display_print_chunk(0, 0, "pass < 10 chars");
      ADP_delay(1500);
      continue;
    }

    break;
  }


  return 0;
}

int check_display_dims() {
  uint16_t rows, cols;

  display_dims(&cols, &rows);
  if(rows < 1 || cols < 8)
    return 1;

  return 0;
}

void depo_main() {
  uint8_t key[64];

  if(g_aborted != 0)
    return;

  if(check_display_dims() != 0) {
    err("dims too small. abort");
    return;
  }

  if(generate_key(key) != 0) {
    err("error in password entry");
    return;
  }

  err("unknown failure");
}
