#include<bits/stdc++.h>
#include "sg-scanner.hpp"
#define token std::pair<std::string, std::string> // {token type, token content}

// Scanner class for the sg compiler

// The Scanner class doesn't do the lexing, but is the interface for the
// actual lexer to the file. It keeps track of the line and character
// number, and lets the lexer fetch words or lines, and provides a simple
// form of backtracking.

// Pascal Sommer, November 2016


// some utility functions
bool is_whitespace(char c){
    return isspace(c);
}
bool is_not_newline(char c){
    return (c!='\n' && c!='\r');
}
bool is_alphanumeric(char c){
    return isalnum(c);
}
bool is_digit(char c){
    return isdigit(c);
}



// ******************* Public functions **********************

Scanner::Scanner(std::string file_path): file_path(file_path)
{
    infile.open(file_path, std::ifstream::in);
    line_number = 1;
    line_character_number = 0;
    buffer = "";
    eof = false;
}

void Scanner::skip_whitespace(){
    get_while(&is_whitespace);
}

// is eof reached?
bool Scanner::reached_eof(){
    return buffer.empty() && eof;
}

// if string matches, consume input, otherwise don't
// consume anything. Skips any leading whitespace.
bool Scanner::match_string(std::string pattern){
    skip_whitespace();
    std::string in = peek_str(pattern.size());

    if(in.size() < pattern.size()){
	return false;
    }

    if( in == pattern ){
	// if string matches, consume it.
	get_str(pattern.size());
	return true;
    }else{
	return false;
    }
}

// consumes word made from letters and numbers. Returns
// without leading or trailing whitespace.
std::string Scanner::get_alphanum(){
    skip_whitespace();
    return get_while(&is_alphanumeric);
}

// consumes number. Returns  without leading or trailing
// whitespace.
std::string Scanner::get_number(){
    skip_whitespace();
    return get_while(&is_digit);
}

// get until newline, and consume input.
std::string Scanner::get_rest_of_line(){
    return get_while(&is_not_newline);
}

// return the file path, line number and character number
std::string Scanner::get_position(){
    return file_path + ", " +
	std::to_string(line_number) + ":" +
	std::to_string(line_character_number);
}



// ***************** Private functions *************************

bool Scanner::fill_buffer(int len){	
    while (buffer.size() < len){
	if(infile.eof()){
	    return false;
	}

	char next;
	if( !infile.get(next)){
	    eof = true;
	    return false;
	}else{
	    buffer+=next;
	}
    }
    return true;
}


// return substring from buffer and then remove it. If len is
// greater than the buffer length, the entire buffer will be
// returned quietly. len -1 will return the entire buffer as
// well.
// this function updates the line and character number, thus
// every function that clears something from the buffer must
// do such through this function.
std::string Scanner::clear_from_buffer(int len){
    if(len == -1){
	len = buffer.size();
    }
    len = std::min(len, (int)buffer.size());	
    std::string out = buffer.substr(0, len);
    for(auto c:out){
	if(is_not_newline(c)){
	    ++line_character_number;
	}else{
	    line_character_number = 0;
	    ++line_number;
	}
    }
    buffer.erase(buffer.begin(), buffer.begin()+len);
    return out;
}

// consumes the input while the next character fulfills (*cond).
// usually leaves a single character in the buffer, unless eof
// was reached.
std::string Scanner::get_while(bool (*cond)(char)){
	
    for(int i = 0; i < buffer.size(); ++i){
	if( ! (*cond)(buffer[i])){
	    return clear_from_buffer(i);
	}
    }

    std::string out = clear_from_buffer(-1);

    for(;;){
	if( ! fill_buffer(1)) break;
	if( ! (*cond)(buffer[0])) break;
	out += clear_from_buffer(1);
    }

    return out;
}

std::string Scanner::peek_str(int len){
    if( ! fill_buffer(len)){
	// return as much as possible if eof is reached
	len = buffer.size();
    }

    std::string out = buffer.substr(0, len);
    return out;
}

std::string Scanner::get_str(int len){
    std::string out = peek_str(len);
    // this might be shorter than the original len
    len = out.size();
    clear_from_buffer(len);
    return out;
}
