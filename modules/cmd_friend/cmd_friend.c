/**
 * @file cmd_friend.c
 * 
 * CMD_Friend - The Command line friend library!
 * 
 * This library is made for C and C++ programs to parse command line options in the easiest way possible.
 * Based heavily on the argp library of the GNU project.
 * 
 * Created by: João Peterson Scheffer - 2020.
 * 
 * This is the source file, please don't modify, except when stated in documentation.
 * 
 */

#ifdef __cplusplus  
extern "C" { 
#endif 

/* Includes and dependencies */

#include "cmd_friend.h"

/* -------------------------------------------- Private Globals -------------------------------------------------------- */

/**
 * @brief Default options array, every option shall always have zero arguments.
 * Modify if necessary.
 */
cmdf_options default_options[] = 
{
    {"help",   _HELP_KEY, OPTION_OPTIONAL, 0, "Shows this help menu"},
    {"info",   _INFO_KEY, OPTION_ALIAS, 0, "Shows information about he program"},
    {"version",   _VERSION_KEY, OPTION_ALIAS, 0, "Shows program version"},
    {0}
}; 


/**
 * @brief Default options array lenght.
 */
int default_options_lenght = 3;

/* -------------------------------------------- Private Functions ------------------------------------------------------ */

/**
 * @brief Function to check if a character is a ascii letter.
 * @param character: Char character.
 */
int is_letter(int character)
{
    if( (character >= 'a' && character <= 'z') || (character >= 'A' && character <= 'Z') )
        return 1;
    else
        return 0;
}


/**
 * @brief Concatenates a single char into a string.
 * @param destination_string: String to receive character.
 * @param character: Char character.
 */
void strcat_char(char *destination_string ,int character)
{
    char *buffer = malloc(sizeof(*buffer)*2);

    buffer[0] = (char)character;
    buffer[1] = '\0';

    strcat(destination_string, buffer);
}


/**
 * @brief Internal error handler.
 * Called when option logic error and exceptions occur.
 * @param flags: receives flags defined by user in the main function "cdmf_parse_options" call.
 * @param format_string: Format text string jsut as in printf, also the variable arguments parameters.
 */
void error_handler_parse_options_internal(cdmf_PARSER_FLAGS_Typedef flags, const char* format_string, ... )
{
    FILE *out = NULL;
    if(flags & PARSER_FLAG_PRINT_ERRORS_STDOUT)
        out = stdout;
    else if(flags & PARSER_FLAG_PRINT_ERRORS_STDERR)
        out = stderr;
    else 
        out = stdout; // just in case 

    va_list valist;
    va_start(valist, format_string);

    vfprintf(out, format_string, valist);

    va_end(valist);

    if(!(flags & PARSER_FLAG_NOT_EXIT_ON_ERROR))
        exit(1);
    else
        return;
}


/**
 * @brief Default option parser.
 * @param key: Char key of defined function.
 * @param user_options: User define options array, used for printing them in --help option.
 */
void default_options_parser(char key, cmdf_options user_options[])
{
    int user_options_len = 0;
    while(user_options[user_options_len].long_name != NULL)
        user_options_len++;

    int i = 0;

    switch (key)
    {
        case _HELP_KEY:     // --help
            i = 0;
            printf("%s\n\n",cmdf_default_info_usage);
            while(i < user_options_len)
            {
                if(!(user_options[i].parameters & OPTION_HIDDEN)) // if not hidden, (OPTION_HIDDEN)
                    if(is_letter(user_options[i].key))
                        printf("\t-%c ( --%s ): %s.\n", user_options[i].key, user_options[i].long_name, user_options[i].description);
                    else
                        printf("\t--%s : %s.\n", user_options[i].long_name, user_options[i].description);

                i++;
            }
            break;

        case _VERSION_KEY:  // --version
            printf("%s\n",cmdf_default_info_version);
            break;

        case _INFO_KEY:     // --info
            printf("%s\n",cmdf_default_info_contact_info);
            break;
    }

    return;
}

/* -------------------------------------------- Functions Implementations ---------------------------------------------- */

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
int cdmf_parse_options(cmdf_options registered_options[], option_parse_function parse_function, int argc, char **argv, cdmf_PARSER_FLAGS_Typedef flags, void *extern_user_variables_struct)
{
    char current_key = 0;
    char* current_argument;
    int i = 1, j = 0, k= 0;
    int x = -1, y = 0;
    int current_key_arg_counter = -1; // in case of first coming arguments with no set key, they will be processed until a key arrives
    int flag_found_default = 0;

    // to check for not optional options    
    typedef struct{ char* alias; }options_matrix;
    options_matrix *options_not_optional = malloc(sizeof(*options_not_optional)*0xFF+1); // 0xFF, biggest number in char, hence the vector should be able to hold that many
    memset(options_not_optional, 0, sizeof(*options_not_optional)*0xFF); // just in case
    char *options_passed = malloc(sizeof(*options_passed)*0xFF);
    options_passed[0]='\0';
    int flag_last_non_alias_was_optional_option = 0;

    // to check for duplicate keys
    char *all_keys = malloc(sizeof(*all_keys)*0xFF);
    all_keys[0]='\0';
    strcat_char(all_keys, _INFO_KEY);
    strcat_char(all_keys, _VERSION_KEY);
    strcat_char(all_keys, _HELP_KEY);

    // to check for aliases
    int cmdf_options_len = 0;
    cmdf_options last_option = {0};


    // run trought options, incrementing the size and substituting aliases, (OPTION_ALIAS). 
    while(registered_options[cmdf_options_len].long_name != NULL)
    {
        if(strchr(all_keys, registered_options[cmdf_options_len].key) == NULL) // if the key is not a DUPLICATE
            strcat_char(all_keys, registered_options[cmdf_options_len].key); // cat key
        else
            error_handler_parse_options_internal(flags, "The key -%c from option --%s is already registered by another option.\n", registered_options[cmdf_options_len].key, registered_options[cmdf_options_len].long_name);



        if( (registered_options[cmdf_options_len].parameters & OPTION_NO_CHAR_KEY) && is_letter(registered_options[cmdf_options_len].key)) // if no char key, then it must check to see if the key is a non assci letter
            error_handler_parse_options_internal(flags, "An option with (OPTION_NO_CHAR_KEY) specified must be a non ascii alphabetical character.\n");


        if( !(registered_options[cmdf_options_len].parameters & OPTION_NO_CHAR_KEY) && !is_letter(registered_options[cmdf_options_len].key)) // if char key, then it must check to see if the key is a assci letter
            error_handler_parse_options_internal(flags, "An option with a specified char key must be a ascii alphabetical character.\n");



        if(!(registered_options[cmdf_options_len].parameters & OPTION_OPTIONAL) && !(registered_options[cmdf_options_len].parameters & OPTION_ALIAS))    // if not optional, !(OPTION_OPTIONAL)
        {
            flag_last_non_alias_was_optional_option = 0;
            x++;
            strcat_char((char*)(&options_not_optional[x].alias),registered_options[cmdf_options_len].key);
        }
        else if((registered_options[cmdf_options_len].parameters & OPTION_ALIAS) && flag_last_non_alias_was_optional_option == 0)   // if last option was non optional and this is a alias
        {
            strcat_char((char*)(&options_not_optional[x].alias),registered_options[cmdf_options_len].key);
        }
        else // if a new optional option appears, then stop adding aliases
        {
            flag_last_non_alias_was_optional_option = 1;
        }
        

        if(registered_options[cmdf_options_len].parameters & OPTION_ALIAS) // check for aliases, (OPTION_ALIAS)
        {
            if(last_option.long_name == NULL) // if the first registered option is an alias, then its obvisually ilegal
                error_handler_parse_options_internal(flags, "The first option must be a non alias option, an alias must be declared below a non alias option.\n");

            registered_options[cmdf_options_len].parameters = last_option.parameters;
            registered_options[cmdf_options_len].argq = last_option.argq;
            registered_options[cmdf_options_len].description = malloc(strlen(last_option.description)+1); // allocate for empty pointer then copy
            strcpy(registered_options[cmdf_options_len].description,last_option.description);
        }
        else
        {
            last_option = registered_options[cmdf_options_len];
        }
        
        cmdf_options_len++;
    }

    // run for every argument passed on cmd
    while(i < argc) 
    {
        current_argument = argv[i];


        // verify for default expressions -------------------------

        if(flags & PARSER_FLAG_USE_PREDEFINED_OPTIONS)
        {
            if(current_argument[0]=='-' && current_argument[1]=='-' && current_key_arg_counter<=0 ) // ---------------------------default long options
            {
                current_argument += 2; // jump over "--"

                j = 0;
                while(j < default_options_lenght) // verify if its registered
                {
                    if(!strcmp(default_options[j].long_name,current_argument))
                    {
                        flag_found_default = 1;
                        default_options_parser(default_options[j].key, registered_options);
                        i++; // jumps to next argv
                        break; // found!
                    }

                    j++;
                }

                if(!flag_found_default) // if no default option found
                    current_argument -= 2; // goes back the "--" so the next else-if case can process

            }
            else if(current_argument[0]=='-' && current_key_arg_counter<=0 && strlen(current_argument)<=2) // ---------------------------default single key options
            {
                current_argument += 1; // jump over "-"

                j = 0;
                while(j < default_options_lenght) // verify if its registered
                {
                    if(current_argument[0]==default_options[j].key) // compare keys and verify if registered
                    {
                        flag_found_default = 1;
                        default_options_parser(default_options[j].key, registered_options);
                        i++; // jumps to next argv
                        break; // found!
                    }

                    j++;
                }

                if(!flag_found_default) // if no default option found
                    current_argument -= 1; // goes back the "--" so the next else-if case can process

            }
        }


        //if default option is found, continue the loop -----------------------------------------------------------
        if(flag_found_default)
        {
            flag_found_default = 0;
            continue;
        }


        //verify for registered arguments --------------------------------------------------------------------------
        if(current_argument[0]=='-' && current_argument[1]=='-' && current_key_arg_counter<=0 ) // -----------------------------------long option
        {
            current_argument += 2; // jump over "--"

            j = 0;
            while(j < cmdf_options_len) // verify if its registered
            {
                if(!strcmp(registered_options[j].long_name,current_argument))
                {
                    current_key = registered_options[j].key; // pick key
                    current_key_arg_counter = registered_options[j].argq; // arguments to receive
                    break; // found!
                }

                j++;
            }

            if(j == cmdf_options_len-1) // if the while was exited by expression and not by the continue, indicating that the option was not found
                error_handler_parse_options_internal(flags, "Option --%s is not valid.\n", current_argument);

            strcat_char(options_passed, current_key); // register the key used
            i++;
            continue;

        }
        else if(current_argument[0]=='-' && current_key_arg_counter<=0 && strlen(current_argument)<=2) // ---------------------------single key option
        {
            current_argument += 1; // jump over "-"

            j = 0;
            while(j < cmdf_options_len) // verify if its registered
            {
                if(current_argument[0]==registered_options[j].key) // compare keys and verify if registered
                {
                    current_key = registered_options[j].key; // pick key
                    current_key_arg_counter = registered_options[j].argq; // arguments to receive
                    break; // found!
                }

                j++;
            }

            if(j == cmdf_options_len-1) // if the while was exited by expression and not by the continue, indicating that the option was not found
                error_handler_parse_options_internal(flags, "Option --%s is not valid.\n", current_argument);

            strcat_char(options_passed, current_key); // register the key used
            i++;
            continue;

        }
        else if(current_argument[0]=='-' && current_key_arg_counter<=0 && strlen(current_argument)>2) // ---------------------------nested key options
        {
            current_argument += 1; // jump over "-"

            k = 0;
            while(k < strlen(current_argument)) // the last element should be handled exiting this loop and leaving to the function outside else-if case as normal
            {
                j = 0;
                while(j < cmdf_options_len) // verify if its registered
                {
                    if(current_argument[k]==registered_options[j].key) // compare keys and verify if registered
                    {
                        current_key = registered_options[j].key; // pick key
                        current_key_arg_counter = registered_options[j].argq; // arguments to receive
                        if(current_key_arg_counter != 0) // only no arguments options can be nested
                            error_handler_parse_options_internal(flags, "Only options that do not receive arguments can be nested. Option called: -%c .\n",current_key);
                        break; 
                    }

                    j++;
                }

                if(j == cmdf_options_len-1) // if the while was exited by expression and not by the continue, indicating that the option was not found
                    error_handler_parse_options_internal(flags, "Option -%c is not valid.\n", current_argument[k]);

                if(k == strlen(current_argument)-1) // shall break in the last argument, so it can be handled as normal, as if this block never saw the first options
                    break;

                strcat_char(options_passed, current_key); // register the key used

                (*parse_function)(current_key, current_argument, i, extern_user_variables_struct); // call parser function on each nested element

                k++;
            }

            strcat_char(options_passed, current_key); // register the key used
            i++;
            continue;

        }
        else if(current_argument[0]=='-' && current_key_arg_counter > 0) // ---------------------------found a new option "-" or "--" while waiting for other arguments
        {
            error_handler_parse_options_internal(flags, "Too few arguments for option -%c .\n", current_key);
        }
        else // ------------------------------------------------------floating argument
        {
            if(current_key_arg_counter > 0) // if argument found then count as a possible argument for the current_option
            {
                current_key_arg_counter--;
            }
            else if(current_key_arg_counter == 0)
            {
                error_handler_parse_options_internal(flags, "Too many arguments for option -%c . Extra argument: %s .\n", current_key, current_argument);
            }

            i++;
        }

        // call parse funtion if the argument has already been received
        (*parse_function)(current_key, current_argument, i, extern_user_variables_struct); 
    }

    // if there are pending argumetns for a option
    if(current_key_arg_counter > 0)
        error_handler_parse_options_internal(flags, "Too few arguments for option -%c .\n", current_key);

    // check for not optional options
    i = 0;
    while(((char*)(&options_not_optional[i].alias))[0] != '\0') // until a alias string is empty
    {
        if(strpbrk(options_passed,(char*)(&options_not_optional[i].alias)) == NULL)
            error_handler_parse_options_internal(flags, "The option -%c needs to be specified.\n", options_not_optional[i]);

        i++;
    }

    return 0;
}

#ifdef __cplusplus 
} 
#endif 