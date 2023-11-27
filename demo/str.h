#pragma once

struct String{
    char *val;
    unsigned int length;
};

unsigned int chars_length(const char *val);
struct String Create_String(const char *val);

void AddCharsToString(struct String *string, const char *val);

void AddToString(struct String *string_one, struct String *string_two);
void AddToString(struct String *string, char val);
void AddToString(struct String *string, float val);

void DeleteString(String *string);