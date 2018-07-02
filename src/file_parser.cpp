#include "file_parser.h"

#include "basics.h"
#include <fstream>

ResourceFile parse_resource_file( const std::string& file_path )
{
    ResourceFile file;
    file.path = file_path;

    std::ifstream reader( file_path );
    if(!reader.is_open())
    {
        println("Failed to open resource file %", file_path);
        file.is_valid = false;
        return file;
    }

    std::string line;
    RFBlock current_block;

    auto register_block = [] ( RFBlock& block, ResourceFile& file ) {
        if( !block.name.empty() )
        {
            file.blocks.emplace_back( block );
            block.name.clear();
            block.content.clear();
        }
    };

    uint line_count = 0;
    while( std::getline( reader, line ) )
    {
        ++line_count;

        const char* ptr = line.c_str();
        ptr = chomp_empty_space( ptr );
        if( *ptr == ':' )
        {
            register_block( current_block, file );

            const char* start = ++ptr;
            const char* end = chomp_token( start );

            current_block.name.assign( start, end );
            current_block.offset = line_count;
        }
        else
        {
            if( current_block.name.empty() )
            {
                println( "WARNING: Line found outside any block." );
            }
            else
            {
                current_block.content += line;
                current_block.content.push_back( '\n' );
            }
        }
    }

    register_block( current_block, file );

    file.is_valid = true;
    return file;
}

bool is_number(char c)
{
    return '0' <= c && c <= '9';
}

bool is_letter(char c)
{
    return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z');
}

bool is_eof(char c)
{
    return c == '\0';
}

bool is_empty_space(char c)
{
    return c == ' ' || c == '\t';
}

bool is_new_line(char c)
{
    return c == '\n';
}

const char* chomp_empty_space(const char* ptr)
{
    while(is_empty_space(*ptr)) ptr++;
    return ptr;
}

const char* chomp_token (const char* ptr)
{
    while( ! ( is_empty_space(*ptr) 
            || is_eof(*ptr) 
            || is_new_line(*ptr) 
            || (*ptr == '\r' && *(ptr+1) == '\n') ) ) ptr++;
    return ptr;
}

const char* chomp_line (const char* ptr)
{
    while( true )
    {
        if( is_eof(*ptr) )
            return ptr;
        if( *ptr == '\r' && *(ptr+1) == '\n' )
            return ptr;
        if( is_new_line(*ptr) )
            return ptr;
        ptr++;
    }
    return ptr;
}

bool try_parse_to_int( const std::string& str, int& out )
{
    try
    {
        out = std::stoi( str );
        return true;
    }
    catch(std::invalid_argument e)
    {
        return false;
    }
}

bool try_parse_to_uint( const std::string& str, uint& out )
{
    try
    {
        out = static_cast<uint>( std::stoi( str ) );
        return true;
    }
    catch(std::invalid_argument e)
    {
        return false;
    }
}

const char* extract_file_name( const char* file, char* buffer, uint buffer_length )
{
    uint last_slash_pos = 0;
    uint ext_pos = 0;
    uint offset = 0;
    while( !is_eof( file[offset] ) )
    {
        if( file[offset] == '\\' || file[offset] == '/' )
            last_slash_pos = offset;
        if( file[offset] == '.' && ext_pos == 0 )
            ext_pos = offset;

        ++offset;
    }

    strncpy_s( buffer, buffer_length, file + last_slash_pos + 1, (ext_pos-1) - last_slash_pos );
    return buffer;
}
