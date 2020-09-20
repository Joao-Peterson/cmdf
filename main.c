/**
 * Main application example to demonstrate usage of the library CMD Friend.
 * 
 * Created by: João Peterson Scheffer - 2020.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "modules/cmd_friend/cmd_friend.h"

// keys for options that don't have alphabetical short keys 
#define VSCODE_KEY 2 

//struct containing the program options of the command line.
cmdf_options options[] = 
{
    {"where",   'w', OPTION_OPTIONAL, 1, "Where to create the project"},
    {"output",  'o', OPTION_ALIAS},
    {"folder",  'f', OPTION_ALIAS},
    {"module",  'M', OPTION_OPTIONAL, 1, "To generate a module template in the folder specified"},
    {"mod",     'm', OPTION_ALIAS},
    {"verbose", 'v', OPTION_OPTIONAL, 0, "Verbose mode"},
    {"verbose+",'V', OPTION_OPTIONAL, 0, "Verbose plusmode"},
    {"Wall",    'W', OPTION_OPTIONAL, 0, "Wall error mode"},
    {"vscode",   VSCODE_KEY, OPTION_OPTIONAL | OPTION_NO_CHAR_KEY, 0, "Visual studio code .vscode folder with .json configuration files"},
    {0}
};

//struct that contains variables related to options and used in the main program.
struct arguments_info
{
    char *filepath_project;       /**< filepath to generte template project*/
    char *filepath_module;        /**< filepath to generte template module*/
    int generate_vscode_folder;   /**< flag that indicates to generate a vscode folder with .json configuration files*/
    int verbose_enable;           /**< flag that indicates if the program will print to stdout*/
};

//parser function to process option on command line 
int parse_options(char key, char *arg, int arg_pos, void *extern_user_variables_struct)
{
    struct arguments_info *myvariables = (struct arguments_info*)extern_user_variables_struct; // retrieving custom struct

    switch (key)
    {
        case 'w': case 'o': case 'f':                   // --where, --output, --folder 
            myvariables->filepath_project = arg;
            printf("Argument \"%s\" for key -%c .\n",arg,key);
            break;

        case 'M': case 'm':                             // --module, -- mod
            myvariables->filepath_module = arg;
            break;

        case 'v':                                       // --verbose
            printf("verbose\n");
            myvariables->verbose_enable = 1;
            break;
        
        case 'V':                                       // --Verbose+
            printf("Verbose+\n");
            break;
        
        case 'W':                                       // --Wall
            printf("Wall\n");
            break;

        case VSCODE_KEY:                                // --vscode
            myvariables->generate_vscode_folder = 1;
            break;

        case 0:                                         // case 0, if a list of arguments is given before any option, or zero argumetns passed
            printf("Case 0 hapenned!\n");
            break;                          

        default:                                        // uhandled cases of registered options
            printf("Case default hapenned!\n");
            break;
    }

    return 0;
}

/* Main ----------------------------------------------------------------------------------- */


int main(int argc, char **argv)
{
    // set information for the default options
    set_cmdf_default_info_usage("Usage: [-w,-o,-f] for project or [-M,-m] for module [FILEPATH] [OPTIONS...]");
    set_cmdf_default_info_version("v1.0 - 10/08/2020");
    set_cmdf_default_info_contact_info("Repo: https://github.com/Joao-Peterson/CMD-Friend - Email: joco_zx@hotmail.com");

    struct arguments_info myvars;
    //default values
    myvars.filepath_project = NULL;
    myvars.filepath_module = NULL;
    myvars.generate_vscode_folder = 0;
    myvars.verbose_enable = 0;

    // parse options
    cdmf_parse_options(options, parse_options, argc, argv, PARSER_FLAG_PRINT_ERRORS_STDOUT | PARSER_FLAG_USE_PREDEFINED_OPTIONS, &myvars);

    return 0;
}