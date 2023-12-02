#pragma once
#include "app.h"

unsigned int chars_length(const char *val);

struct String{
    char *val = 0;
    unsigned int length = 0;
    bool delete_after_use; // set to true when the function that will use can delete after using

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
    
    String& operator=(String str);
    String& operator+(String str);
    String& operator+(const char *str);
    String& operator+=(String& str);
    String& operator+=(const char *str);
    String& operator+=(char str);
    String& operator+=(float val);
};

struct String Create_String(const char *val, bool delete_after_use);

void DeleteString(String *string);