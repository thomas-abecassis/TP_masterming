#ifndef SHARED_H
#define SHARED_H

#include "wchar.h"

typedef struct
{
	char* name;
	int length;
	int nb_colors;
	int history_length;
}variation;

variation variations[] = {
	{"Original", 4, 6, 12},
	{"Deluxe", 8, 5, 14},
	{"Kids", 5, 3, 10},
	{"Binary", 4, 2, 5},
	{"Extreme", 8, 8, 18}
};

#define COL_RED "\x1b[38;2;255;32;32m"
#define COL_GREEN "\x1b[38;2;32;255;32m"
#define COL_BLUE "\x1b[38;2;32;64;255m"
#define COL_YELLOW "\x1b[38;2;255;255;32m"
#define COL_PINK "\x1b[38;2;255;128;128m"
#define COL_PURPLE "\x1b[38;2;192;32;255m"
#define COL_ORANGE "\x1b[38;2;255;128;32m"
#define COL_WHITE "\x1b[38;2;255;255;255m"
#define COL_RESET "\e[0m"

#define COL_BLACK_W 0
#define COL_RED_W 1
#define COL_GREEN_W 2
#define COL_BLUE_W 3
#define COL_YELLOW_W 4
#define COL_PINK_W 5
#define COL_PURPLE_W 6
#define COL_WHITE_W 7
#define COL_ORANGE_W 8

wchar_t* CODE_STATES[] = {L"⭘", L"⬤"};
char* CODE_COLORS[] = {COL_RED, COL_GREEN, COL_BLUE, COL_YELLOW, COL_PURPLE, COL_ORANGE, COL_PINK, COL_WHITE};
int CODE_COLORS_W[] = {COL_RED_W, COL_GREEN_W, COL_BLUE_W, COL_YELLOW_W, COL_PURPLE_W, COL_ORANGE_W, COL_PINK_W, COL_WHITE_W};

wchar_t* HINT_STATES[] = {L"•", L"⭗", L"⭗"};
char* HINT_COLORS[] = {COL_WHITE, COL_WHITE, COL_RED};
int HINT_COLORS_W[] = {COL_WHITE_W, COL_WHITE_W, COL_RED_W};


void print_code(char* state, char* color, int size){
	char c=0;
	char s=0;
	for(int i=0; i < size; i++){
		c = (color[i/2] >> 4*(i%2)) & 0xF;
		s = (state[i/8] >> (i%8)) & 1;
		printf("%s", CODE_COLORS[c]);
		wprintf(L" %s ", CODE_STATES[s]);
		printf(COL_RESET);
	}
}

void print_hint(char* state, int size){
	char s=0;
	for(int i=0; i < size; i++){
		s = (state[i/4] >> 2*(i%4)) & 3;
		printf("%s", HINT_COLORS[s]);
		wprintf(L" %s ", HINT_STATES[s]);
		printf(COL_RESET);
	}
}

void print_line(char* code_state, char* code_color, char* hint_state, int size){
	print_code(code_state, code_color, size);
	printf(" ");
	print_hint(hint_state, size);
	printf("\n");
}
#endif