#ifndef UTILS_H
#define UTILS_H

#define SIZE(vec) (sizeof(vec)/sizeof(vec[0]))

#define CMDDEF(cmd, key) printf("\033[0;38;41m%s\033[0;38;0m %s ", cmd, key)

#define cursorup(x)    printf("\033[%dA", x)
#define cursordown(x)  printf("\033[%dB", x)
#define cursorright(x) printf("\033[%dC", x)
#define cursorleft(x)  printf("\033[%dD", x)
#define cursorto(x, y) printf("\033[%d;%dH", y + 1, x + 1)

void initwin();
void endwin();

int colorhash(char *str, int n);

#endif
