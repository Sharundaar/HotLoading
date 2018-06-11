#pragma once

#include <vector>
#include <string>

#include "basic_types.h"

struct RFBlock
{
    std::string name;
    std::string content;
    uint offset;
};

struct ResourceFile
{
    std::string path;
    std::vector< RFBlock > blocks;
    bool is_valid = false;
};

bool is_number     (char c);
bool is_letter     (char c);
bool is_eof        (char c);
bool is_empty_space(char c);
bool is_new_line   (char c);

const char* chomp_empty_space     (const char* ptr);
const char* chomp_token           (const char* ptr);
const char* chomp_line            (const char* ptr);
const char* chomp_until_separator (const char* ptr);

bool try_parse_to_int ( const std::string& str, int& out );
bool try_parse_to_uint( const std::string& str, uint& out );

ResourceFile parse_resource_file( const std::string& file_path );
