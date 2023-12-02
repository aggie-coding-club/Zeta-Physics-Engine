#pragma once
#include "app.h"

unsigned int chars_length(const char *val);

struct String{
    char *val = 0;
    unsigned int length = 0;

    // empty constructor
    String(){
    }

    String(const char* str){
        if(str){
            length = chars_length(str);
            this->val = (char *)calloc(length, sizeof(char));

            for(int i = 0; i < length; i++){
                this->val[i] = str[i];
            }
        }
    }
    
    String operator=(String& str);
    String operator+(String& str);
    String operator+(const char *str);
    String operator+=(String& str);
    String operator+=(const char *str);
    String operator+=(char str);
    String operator+=(float val);

    ~String(){
        int x = 0;
        // free(val);
    }
};

struct String Create_String(const char *val);

// void AddCharsToString(String *string, const char *val);
// void AddCharsToString(const char *val , String *string);
// void AddCharsToString(String string, const char *val);
// void AddCharsToString(const char *val , String string);

// void AddToString(String *string_one, String *string_two);
// void AddToString(String *string, char val);
// void AddToString(String *string, float val);

void DeleteString(String *string);