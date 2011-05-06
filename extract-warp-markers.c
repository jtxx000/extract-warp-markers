#include <endian.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

int rot(int head, int size) {
  if (++head>=size) return head-size;
  return head;
}

int find_str(FILE* f, const char* string) {
  const int size = strlen(string);
  char buffer[size];
  if (fread(buffer, 1, size, f) != size) return 0;
  int head=0, c;

  for (;;) {
    int rHead = head;
    for (int i=0; i<size; i++) {
      if (buffer[rHead] != string[i]) goto next;
      rHead = rot(rHead,size);
    }
    return 1;

 next:
    if ((c=getc(f)) == EOF) break;
    buffer[head] = c;
    head = rot(head,size);
  }

  return 0;
}

int read_double(FILE* f, double* x) {
  if (fread(x, 1, 8, f) != 8) return 0;
  *(uint64_t*)x = le64toh(*(uint64_t*)x);
  return 1;
}

int read_warp_marker(FILE* f, double* pos, double* beat) {
  return
    find_str(f, "WarpMarker") &&
    !fseek(f, 4, SEEK_CUR) &&
    read_double(f, pos) &&
    read_double(f, beat);
}

int main(int argc, char* argv[]) {
  if (argc != 2) {
    printf("usage: %s file.asd\n", argv[0]);
    return 1;
  }

  FILE* f = fopen(argv[1], "r");
  if (!f) {
    printf("couldn't open file\n");
    return 2;
  }

  double p1, b1, p2, b2;
  if (!(find_str(f, "WarpMarker") &&
        read_warp_marker(f, &p1, &b1) &&
        read_warp_marker(f, &p2, &b2))) {
    printf("couldn't find warp markers\n");
    return 3;
  }

  printf("%.17g %.17g\n", p1, (b2-b1)/(p2-p1)*60.0);
}
