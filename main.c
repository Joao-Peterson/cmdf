/**
 * Main application example to demonstrate usage of the library CMD Friend.
 * 
 * Created by: João Peterson Scheffer - 2020.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include "modules/cmd_friend/cmd_friend.h"

// keys for options that don't have alphabetical short keys 
#define VSCODE_KEY 2 


//struct containing the program options of the command line.
cmdf_options options[] = 
{
    {"where",   'w', 0,                                             1, "Where to create the project"},
    {"file",    'f', OPTION_ALIAS },
    {"tags",    't', OPTION_OPTIONAL,                              -1, "Tags to put in"},
    {"verbose", 'v', OPTION_OPTIONAL | OPTION_NO_LONG_KEY,          0, "Verbose mode"},
    {"Wall",    'W', OPTION_OPTIONAL,                               0, "Wall error mode"},
    {"vscode",   VSCODE_KEY, OPTION_OPTIONAL | OPTION_NO_CHAR_KEY,  0, "Visual studio code .vscode folder with .json configuration files"},
    {0}
};


//struct that contains variables related to options and used in the main program.
struct arguments_info
{
    char *filepath_project;         /**< filepath to generte template project*/
    int tags;                       /**< int that has bitwise tags*/
    int verbose_enable;             /**< flag that indicates if the program will print to console*/
    int wall_enable;                /**< flag that indicates if the program will use wall commands*/
    int generate_vscode_folder;     /**< flag that indicates to generate a vscode folder with .json configuration files*/
};


//parser function to process option on command line 
int parse_options(char key, char *arg, int arg_pos, void *extern_user_variables_struct)
{
    struct arguments_info *myvariables = (struct arguments_info*)extern_user_variables_struct; // retrieving custom struct by casting
    long int tag_bit;

    switch (key)
    {
        case 'w': case 'f':                             // --where, --output, --folder 
            myvariables->filepath_project = arg;
            printf("[ARG] Argument \"%s\" for key -%c .\n",arg,key);
            break;

        case 't':                                       // --tags
            if((tag_bit = strtol(arg, NULL, 10)) > INT_MAX)
            {
                printf("[ARG] Argument \"%s\" for key -%c needs to be smaller than %ld.\n",arg,key,INT_MAX);
                exit(1);
            }
            else
                myvariables->tags |= tag_bit;                
            
            printf("[ARG] Number \"%X\" for key -%c .\n",tag_bit,key);
            
            break;

        case 'v':                                       // --verbose
            printf("[ARG] verbose\n");
            myvariables->verbose_enable = 1;
            break;
        
        case 'W':                                       // --Wall
            myvariables->wall_enable = 1;
            printf("[ARG] Wall\n");
            break;

        case VSCODE_KEY:                                // --vscode
            myvariables->generate_vscode_folder = 1;
            printf("[ARG] VSCODE!\n");
            break;

        case 0:                                         
            printf("[ARG] Case 0 hapenned! Key: -%c Arg: %s\n",key,arg);
            break;                          

        default: 
            printf("[ARG] Case default hapenned! Key: -%c Arg: %s\n",key,arg);
            break;
    }

    return 0;
}


/* Main ----------------------------------------------------------------------------------- */


int main(int argc, char **argv)
{
    // set information for the default options
    set_cmdf_default_info_usage("Usage: [-w,-o,-f] for project [FILEPATH] [OPTIONS...]");
    set_cmdf_default_info_version("v1.0 - 10/08/2020");
    set_cmdf_default_info_contact_info("Repo: https://github.com/Joao-Peterson/CMD-Friend - Email: joco_zx@hotmail.com");

    //default values
    struct arguments_info myvars = { 
        .filepath_project = NULL, 
        .generate_vscode_folder = 0,
        .tags = 0,
        .verbose_enable = 0,
        .wall_enable = 0
    };

    // parse options
    cdmf_parse_options(options, parse_options, argc, argv, PARSER_FLAG_PRINT_ERRORS_STDOUT | PARSER_FLAG_USE_PREDEFINED_OPTIONS | PARSER_FLAG_DONT_IGNORE_NON_REGISTERED_OPTIONS, &myvars);

    return 0;
}