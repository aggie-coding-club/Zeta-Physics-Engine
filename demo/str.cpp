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

String String::operator=(String& str){
    String result = {};
    Copy_To_String(&result, &str);
	this->val = result.val;
	this->length = result.length;
    return result;
}

String String::operator+(String& str){
    String result = {};
    result.val = str.val;
    result.length = str.length;

    char *temp = result.val;
    result.val = (char *)calloc(result.length + str.length, sizeof(char));

    for(int i = 0; i < result.length; i++){
        result.val[i] = temp[i];
    }
    free(temp);
    
    int index = 0;
    for(int i = result.length; i < (result.length + str.length); i++){
        result.val[i] = str.val[index++];
    }

    result.length += str.length;

    return result;
}

String String::operator+(const char *str){
    String result = Create_String(str);

    return result;
}

String String::operator+=(char str){
    String result = {};
    char *temp = this->val;

    this->val = (char *)calloc(this->length + 1, sizeof(char));
    result.val = this->val;
    result.length = this->length + 1; 

    for(int i = 0; i < this->length; i++){
        this->val[i] = temp[i];
    }
    this->val[this->length] = str;
    this->length += 1;
    free(temp);
    return result;
}

String String::operator+=(String& str){
    *this += str.val;
    
    return *this;
}

String String::operator+=(const char *str){
    String result = {};
    result.val = this->val;
    result.length = this->length;
    if(!str){
        Assert(!"invalid value");
    }
    unsigned int chars_to_add_len = chars_length(str);
    
    char *temp = result.val;

    result.val = (char *)calloc((result.length + chars_to_add_len), sizeof(char));
    this->val = result.val;

    for(int i = 0; i < result.length; i++){
        result.val[i] = temp[i];
    }

    // free(temp);

    int index = 0;
    for(int i = result.length; i < result.length + chars_to_add_len; i++){
        result.val[i] = str[index++];
    }

    result.length += chars_to_add_len;
    return result;
}

String String::operator+=(float val){
    String result = {};
    result.val = this->val;
    result.length = this->length;
    char tempChar[8] = {};
    snprintf(tempChar, 8, "%f", val);
    result+= tempChar;

    return result;
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
String Create_String(const char *val){
    String result = val;

    // result.length = chars_length(val);
    // result.val = (char *)calloc(result.length, sizeof(char));

    // for(int i = 0; i < result.length; i++){
    //     result.val[i] = val[i];
    // }

    return result;
}

// void AddCharsToString(const char *val , String *string){
//     AddCharsToString(string, val);
// }

// void AddCharsToString(String string, const char *val){
//     AddCharsToString(&string, val);
// }

// void AddCharsToString(const char *val , String string){
//     AddCharsToString(string, val);
// }


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

// Adds string_two to string_two
void AddToString(String *string_one,String *string_two){
    char *temp = string_one->val;
    string_one->val = (char *)calloc(string_one->length + string_two->length, sizeof(char));

    for(int i = 0; i < string_one->length; i++){
        string_one->val[i] = temp[i];
    }
    
    int index = 0;
    for(int i = string_one->length; i < (string_one->length + string_two->length); i++){
        string_one->val[i] = string_two->val[index++];
    }

    string_one->length += string_two->length;
    free(temp);
}


// Add char to string
void AddToString(String *string, char val){
    char *temp = string->val;
    string->val = (char *)calloc(string->length + 1, sizeof(char));

    for(int i = 0; i < string->length; i++){
        string->val[i] = temp[i];
    }
    string->val[string->length] = val;

    string->length += 1;
    free(temp);
}


// Add float to string
void AddToString(String *string, float val){
    char tempChar[8] = {};
    snprintf(tempChar, 8, "%f", val);
    AddCharsToString(string, tempChar);
}

void DeleteString(String *string){
    // free(string->val);
}