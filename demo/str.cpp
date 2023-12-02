#include <stdio.h>
#include <stdlib.h>
#include "str.h"

// Copies from string_two to string_one, overwriting string_one contents. 
// Increases\Decreases size of string one as needed
void Copy_To_String(String *string_one, String *string_two){

    if(string_one->length != string_two->length){
        free(string_one->val);
        string_one->val = (char *)calloc(string_two->length, sizeof(char));
        string_one->length = string_two->length;
    }
        
    for(int i = 0; i < string_one->length; i++){
        string_one->val[i] = string_two->val[i];
    }
}

String& String::operator=(String str){
    if(val){
        free(val);
    }
    Copy_To_String(this, &str);
    return *this;
}

String& String::operator+(String str){
    char *temp = this->val;
    this->val = (char *)calloc(this->length + str.length, sizeof(char));

    for(int i = 0; i < this->length; i++){
        this->val[i] = temp[i];
    }
    free(temp);
    
    int index = 0;
    for(int i = this->length; i < (this->length + str.length); i++){
        this->val[i] = str.val[index++];
    }

    this->length += str.length;

    return *this;
}

String& String::operator+(const char *str){
    String s = Create_String(str, false);
    *this += s;
    free(s.val);

    return *this;
}

String& String::operator+=(char str){
    char *temp = this->val;
    this->val = (char *)calloc(this->length + 1, sizeof(char));

    for(int i = 0; i < this->length; i++){
        this->val[i] = temp[i];
    }
    free(temp);

    this->val[this->length] = str;
    this->length += 1;
    return *this;
}

String& String::operator+=(String& str){
    *this += str.val;
    
    return *this;
}

String& String::operator+=(const char *str){
    if(!str){
        Assert(!"invalid value");
    }
    unsigned int chars_to_add_len = chars_length(str);
    
    char *temp = this->val;

    this->val = (char *)calloc((this->length + chars_to_add_len), sizeof(char));
    
    for(int i = 0; i < this->length; i++){
        this->val[i] = temp[i];
    }
    free(temp);

    int index = 0;
    for(int i = this->length; i < this->length + chars_to_add_len; i++){
        this->val[i] = str[index++];
    }

    this->length += chars_to_add_len;
    return *this;
}

String& String::operator+=(float val){
    char temp_char[8] = {};
    snprintf(temp_char, 8, "%f", val);
    *this += temp_char;

    return *this;
}

unsigned int chars_length(const char *val){
    unsigned int result = 0;
    
    // TODO: Safeguard for overflow
    while(1){
        if(val[result] == 0){
            break;
        }

        result++;
    }

    return result;
}

// null terminated string
String Create_String(const char *val, bool delete_after_use){
    String result = val;
    result.delete_after_use;

    return result;
}

void AddCharsToString(String *string, const char *val){
    if(!val){
        Assert(!"invalid value");
    }
    unsigned int chars_to_add_len = chars_length(val);
    
    char *temp = string->val;

    string->val = (char *)calloc((string->length + chars_to_add_len), sizeof(char));
    

    for(int i = 0; i < string->length; i++){
        string->val[i] = temp[i];
    }

    free(temp);

    int index = 0;
    for(int i = string->length; i < string->length + chars_to_add_len; i++){
        string->val[i] = val[index++];
    }

    string->length += chars_to_add_len;
}

void DeleteString(String *string){
    free(string->val);
}