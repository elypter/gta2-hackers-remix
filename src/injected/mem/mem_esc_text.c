#include "mem.h"
#include "mem_gta2_addresses.h"
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <windows.h> // IsBadReadPtr, Sleep
// documentation: http://git.io/g2hr-esc-text

bool mem_wait_for_string_table() {
  for (int i = 0; i < 100; i++) {
    Sleep(100);

    char *table = GTA2_ADDR_STRING_TABLE;
    if (IsBadReadPtr(table, 10))
      continue;

    char *offset = GTA2_ADDR_STRING_OFFSET;
    if (IsBadReadPtr(offset, 20))
      continue;
    if (offset[0] == 'y' && offset[1] == '!' && offset[3] == 0x00 &&
        offset[5] == 0x00 && offset[7] == 0x00)
      return true;
  }
  printf("ERROR: couldn't find the string table!\n");
  return false;
}

char GLOBAL_DEBUG_EMPTY[] = {'\0', '\0', '\0'};

void mem_prepare_esc_text(mem_t *mem) {
  if (!mem_wait_for_string_table())
    return;

  for (int i = 0; i < 3; i++) {
    mem->text[i] = calloc(0, G2HR_ESC_TEXT_MAXLEN_LINE);
  }

  char *pos = GTA2_ADDR_STRING_TABLE;
  while (pos <= GTA2_ADDR_STRING_OFFSET) {
    char *name = pos + 4;

    if (strncmp(name, "quit1", 8) == 0 || strncmp(name, "quit2", 8) == 0 ||
        strncmp(name, "quit3", 8) == 0) {
      printf("%s: ", name);
      char *addr = *(char **)pos;

      while (*addr) {
        printf("%c", *addr);
        addr += 2;
      }
      printf("\n");

      SuspendThread(mem->main_thread);
      *(char **)pos = GLOBAL_DEBUG_EMPTY;

      // prevent GCC from optimizing this away.
      printf("replaced with: %s\n", GLOBAL_DEBUG_EMPTY);

      ResumeThread(mem->main_thread);
    }
    pos += 12;
  }
}
