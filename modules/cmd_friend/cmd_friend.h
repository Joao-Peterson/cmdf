/**
 * @mainpage
 * CMD_Friend - The Command line friend library!
 * 
 * This library is made for C and C++ programs to parse command line options in the easiest way possible.
 * Based heavily on the argp library of the GNU project.
 * 
 * Created by: João Peterson Scheffer - 2020.
 * Version: v1.0
 * 
 * To compile this library simply compile the source with the header, no dependencies.
 * Arquive if static compile or compile as shared and you can use in your projects, or as i like, compile the source into a object
 * and link it with your main application in compilation or linking step.
 * 
 * To use simplify define strings to these globals if wanted:
 * 
 *      const char cmdf_default_info_contact_info[];
 *      const char cmdf_default_info_version[];
 *      const char cmdf_default_info_usage[];
 * 
 * Then make a array of options in your main aplication outside main(). Eg:
 * 
 *  
 *      cmdf_options options[] = 
 *      {
 *          {"where",   'w', OPTION_OPTIONAL, 1, "Where to create the project"},
 *          {"output",  'o', OPTION_ALIAS},
 *          {"folder",  'f', OPTION_ALIAS},
 *          {"module",  'M', OPTION_OPTIONAL, 1, "To generate a module template in the folder specified"},
 *          {"mod",     'm', OPTION_ALIAS},
 *          {"verbose", 'v', OPTION_OPTIONAL, 0, "Verbose mode"},
 *          {"verbose+",'V', OPTION_OPTIONAL, 0, "Verbose plusmode"},
 *          {"Wall",    'W', OPTION_OPTIONAL, 0, "Wall error mode"},
 *          {"vscode",   VSCODE_KEY, OPTION_OPTIONAL | OPTION_NO_CHAR_KEY, 0, "Visual studio code .vscode folder with .json configuration files"},
 *          {0}
 *      };
 *  
 * 
 * Define a struct of your flavor that hold flags and variables associated with your options, these will be used later in your main aplication. Eg:
 * 
 *  
 *      struct arguments_info
 *      {
 *          char *filepath_project;       
 *          char *filepath_module;        
 *          int generate_vscode_folder;
 *          int verbose_enable;           
 *      };
 *  
 * 
 * Then define a parser function for your options. It should contain a cast to your custom struct and
 * a switch case with cases for your options. Eg:
 * 
 *  
 *      int parse_options(char key, char *arg, int arg_pos, void *extern_user_variables_struct)
 *      {
 *          struct arguments_info *myvariables = (struct arguments_info*)extern_user_variables_struct; // retrieving custom struct
 *      
 *          switch (key)
 *          {
 *              case 'w': case 'o': case 'f':                   // --where, --output, --folder 
 *                  myvariables->filepath_project = arg;
 *                  break;
 *      
 *              case 'M': case 'm':                             // --module, -- mod
 *                  myvariables->filepath_module = arg;
 *                  break;
 *      
 *              case 'v':                                       // --verbose
 *                  printf("verbose\n");
 *                  myvariables->verbose_enable = 1;
 *                  break;
 *              
 *              case 'V':                                       // --Verbose+
 *                  printf("Verbose+\n");
 *                  break;
 *              
 *              case 'W':                                       // --Wall
 *                  printf("Wall\n");
 *                  break;
 *      
 *              case VSCODE_KEY:                                // --vscode
 *                  myvariables->generate_vscode_folder = 1;
 *                  break;
 *      
 *              case 0:                                         // case 0, if a list of arguments is given before any option, or zero argumetns passed
 *                  printf("Case 0 hapenned!\n");
 *                  break;                          
 *      
 *              default:                                        // uhandled cases of registered options
 *                  printf("Case default hapenned!\n");
 *                  break;
 *          }
 *      
 *          return 0;
 *      }
 *  
 * 
 * Finally you can call the main library function and then use your processed flags and variables in your custom struct
 * as you desire. Eg:
 * 
 *  
 *      cdmf_parse_options(options, parse_options, argc, argv, PARSER_FLAG_PRINT_ERRORS_STDOUT | PARSER_FLAG_USE_PREDEFINED_OPTIONS, &myvars);
 *  
 */

/**
 * @file cmd_friend.h
 * 
 * CMD_Friend - The Command line friend library!
 * 
 * This library is made for C and C++ programs to parse command line options in the easiest way possible.
 * Based heavily on the argp library of the GNU project.
 * 
 * Created by: João Peterson Scheffer - 2020.
 * 
 * This is the header file, please don't modify, except when stated in documentation.
 * 
 * Refer to this header as means to use the library and get informations about the variables and functions.
 * 
 */


#ifdef __cplusplus  
extern "C" { 
#endif 

#ifndef __CMD_FRIEND
#define __CMD_FRIEND

/* Includes and dependencies */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

/* -------------------------------------------- Definitions ---------------------------------------------------- */

#define _INFO_KEY 123       // default info option key
#define _VERSION_KEY 124    // default version option key
#define _HELP_KEY 'h'       // default help option key, in case of the 'h' key be desired i suggest using 125 as a key instead

/* -------------------------------------------- Enumerators ---------------------------------------------------- */

/**
 * @brief Parameters for registering cmd options.
 * 
 * To apply multiple options, just use the or bitwise operator pipe "|".
 */
typedef enum
{
    OPTION_ALIAS            = 0x01,   /**< Specifies that this option is an alias of the above option */
    OPTION_OPTIONAL         = 0x02,   /**< This option is not trivial for the program */
    OPTION_NO_CHAR_KEY      = 0x04,   /**< This option can't be called by a alphabetical key, just the long version */
    OPTION_HIDDEN           = 0x08    /**< This option doesn't appear on help menu */
}cdmf_OPTIONS_Typedef;


/**
 * @brief Flags for when calling cmdf_parse_options.
 * 
 * To apply multiple flags, just use the or bitwise operator pipe "|".
 */
typedef enum
{
    PARSER_FLAG_NOT_EXIT_ON_ERROR       = 0x01,     /**< When an exception occurs don't exit program */
    PARSER_FLAG_PRINT_ERRORS_STDOUT     = 0x02,     /**< Print error messages to stdout */
    PARSER_FLAG_PRINT_ERRORS_STDERR     = 0x04,     /**< Print error messages to stderr */
    PARSER_FLAG_USE_PREDEFINED_OPTIONS  = 0x08      /**< Create automatic --help (-h), --info, --version */
}cdmf_PARSER_FLAGS_Typedef;

/* -------------------------------------------- Structures and typedefs----------------------------------------- */

/**
 * @brief Typedef from user parse option function.
 * The "cdmf_parse_options" function will call the user defined function to handle actions as desired.
 * Each call will have a key and argument, where each argument must have be assigned to only one key.
 * @param key: Receives the char key of the option.
 * @param arg: Receives the argument passed to that option.
 * @param arg_pos: The command line argument index where the user inserted the argumment.
 * @param arg_pos: Opaque pointer to user define struct in main program, containing variables that can be acesses inside this function. (Cast is necessary!).
 */
typedef int (*option_parse_function)(char key, char *arg, int arg_pos, void *extern_user_variables_struct);


/**
 * @brief defines a single option in command line.
 * 
 * An array of this struct is used when registering new entries using the default constructor
 * making the process of adding more options more easy.
 * 
 * @note The last member must be always 0, just as a string is a array of char that ends in 0.
 */
typedef struct
{
    char *long_name;                                /**< The name of the option, also used when calling with "--" */
    char key;                                       /**< Small char key correponding to the name, often just the first character. Called with "-". If the option doesn't need a alphabetical key then specify a integer value different from 0 as means to idetification */
    cdmf_OPTIONS_Typedef parameters;                /**< Parameters to enhance functionality */
    int argq;                                       /**< The number of arguments that this option takes, can be >0, if 0 then expects none, if -1 accepts any quantity */
    char *description;                              /**< Small description of the option*/
}cmdf_options;

/* -------------------------------------------- Public prototypes ---------------------------------------------- */

/**
 * @fn
 * @brief Main library function, used to parse options in main program.
 * The "cdmf_parse_options" function will call the user defined function to handle actions as desired.
 * Each call will have a key and argument, where each argument must have be assigned to only one key.
 * @param registered_options: Receives pointer to struct "cmdf_options" array that shall contain user define options.
 * @param parse_function: User defined parse function pointer.
 * @param argc: Main function parameter containing number of passed parameters in command line.
 * @param argv: Main function parameter containing the array of string containing the actual parameters.
 * @param flags: Flags used to customize the function "cdmf_parse_options" behavior, flags shall be located on "cdmf_PARSER_FLAGS_Typedef" enumerator.
 * @param extern_user_variables_struct: Opaque pointer to user define struct in main program, used to be accessed in the also user define parser function.
 */
int cdmf_parse_options(cmdf_options registered_options[], option_parse_function parse_function, int argc, char **argv, cdmf_PARSER_FLAGS_Typedef flags, void *extern_user_variables_struct);

/**
 * @brief Default option parser.
 * @param key: Char key of defined function.
 * @param user_options: User define options array, used for printing them in --help option.
 */
void default_options_parser(char key, cmdf_options user_options[]);

/* -------------------------------------------- Globals -------------------------------------------------------- */

/**
 * @brief Global string, shall be defined in main aplication and assined the usage instruction of the program.
 */
extern const char cmdf_default_info_usage[];


/**
 * @brief Global string, shall be defined in main aplication and assined the version of the program.
 */
extern const char cmdf_default_info_version[];


/**
 * @brief Global string, shall be defined in main aplication and assined the contact info and other relevant information about the application.
 */
extern const char cmdf_default_info_contact_info[];


/**
 * @brief Default options array, every option shall always have zero arguments.
 */
extern cmdf_options default_options[];

/**
 * @brief Default options array lenght.
 */
extern int default_options_lenght;

#endif

#ifdef __cplusplus 
} 
#endif 