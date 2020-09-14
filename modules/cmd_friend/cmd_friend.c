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


/* -------------------------------------------- Private Definitions ---------------------------------------------------- */

/**
 * @brief Default options array lenght.
 */
#define DEFAULT_OPTIONS_LENGHT 3


/* -------------------------------------------- Private Globals -------------------------------------------------------- */

/**
 * String defining the usage info when using default option --help
 */
char *cmdf_default_info_usage = NULL;


/**
 * String defining the version info when using default option --version
 */
char *cmdf_default_info_version = NULL;


/**
 * String defining the contact info when using default option --info
 */
char *cmdf_default_info_contact_info = NULL;


/**
 * @brief Default options array, every option shall always have zero arguments.
 * Modify if necessary.
 */
cmdf_options default_options[] = 
{
    {"help",    _HELP_KEY,    OPTION_OPTIONAL, 0, "Shows this help menu"},
    {"info",    _INFO_KEY,    OPTION_ALIAS,    0, "Shows information about he program"},
    {"version", _VERSION_KEY, OPTION_ALIAS,    0, "Shows program version"},
    {0}
}; 


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
    char *buffer = malloc(sizeof(*buffer));

    buffer[0] = (char)character;

    strcat(destination_string, buffer);

    free(buffer);
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
            if(cmdf_default_info_usage!=NULL)
                printf("%s\n\n",cmdf_default_info_usage);

            while(i < user_options_len) // User defined options
            {
                if(!(user_options[i].parameters & OPTION_HIDDEN)) // if not hidden, (OPTION_HIDDEN)
                {
                    if(is_letter(user_options[i].key))
                        printf("\t-%c ( --%s ): %s.\n", user_options[i].key, user_options[i].long_name, user_options[i].description);
                    else
                        printf("\t--%s : %s.\n", user_options[i].long_name, user_options[i].description);
                }

                i++;
            }

            for(i = 0; i < DEFAULT_OPTIONS_LENGHT; i++) // Default options
            {
                if(!(default_options[i].parameters & OPTION_HIDDEN)) // if not hidden, (OPTION_HIDDEN)
                {
                    if(is_letter(default_options[i].key))
                        printf("\t-%c ( --%s ): %s.\n", default_options[i].key, default_options[i].long_name, default_options[i].description);
                    else
                        printf("\t--%s : %s.\n", default_options[i].long_name, default_options[i].description);
                }

                i++;
            }

            break;

        case _VERSION_KEY:  // --version
            if(cmdf_default_info_version!=NULL)
                printf("%s\n",cmdf_default_info_version);
            break;

        case _INFO_KEY:     // --info
            if(cmdf_default_info_contact_info!=NULL)
                printf("%s\n",cmdf_default_info_contact_info);
            break;
    }

    return;
}


/**
 * @brief Check if key is registered.
 * @param key: Char key to be verified.
 * @param options_array: Options array to check against.
 */
int is_option_registered(char key, char *options_registered_array)
{    
    if(strchr(options_registered_array, key)!=NULL)
        return 1;
    else
        return 0; // no key found return 0
}


/**
 * @brief Check for program required options.
 * @param user_keys_array: Array of user command line keys.
 * @param options_required_matrix: Options aliases matrix to check against.
 * @param flags: Parse flags used for error handling.
 */
int check_required_options(char* user_keys_array, char **options_required_matrix, cdmf_PARSER_FLAGS_Typedef flags)
{
    int i = 0;
    while(options_required_matrix[i][0] != '\0') // until a aliases string is empty
    {
        if(strpbrk(user_keys_array,options_required_matrix[i]) == NULL)
        {
            error_handler_parse_options_internal(flags, "The option -%c needs to be specified.\n", options_required_matrix[i][0]);
            return 0;
        }

        i++;
    }

    return 1;
}


/**
 * @brief Runs trought an array of options looking for the correponding option for a given long name.
 * @param long_name: The full name of the options.
 * @param options_array: Options array to check against.
 * @return Returns a pointer to the option struct.
 */
cmdf_options *get_option_long_name(char *long_name, cmdf_options *options_array)
{
    int i = 0;

    while(options_array[i].long_name != NULL)
    {
        if(!strcmp(long_name, options_array[i].long_name))
            return &(options_array[i]);

        i++;
    }

    return NULL;
}


/**
 * @brief Runs trought an array of options looking for the correponding option for a given char key.
 * @param long_name: The full name of the options.
 * @param options_array: Options array to check against.
 * @return Returns a pointer to the option struct.
 */
cmdf_options *get_option_long_name(char key, cmdf_options *options_array)
{
    int i = 0;

    while(options_array[i].long_name != NULL)
    {
        if(options_array[i].key == key)
            return &(options_array[i]);

        i++;
    }

    return NULL;
}


/**
 * @brief Tweak options array, substituting aliases and duplicates, etc.
 * @param options_array: Options array to check against.
 * @param flags: Parser flags to be used in error handling inside function.
 * @return Returns pointer to 
 */
void parse_registered_options(cmdf_options **options_array_ptr, cdmf_PARSER_FLAGS_Typedef flags, char **options_required_ptr, char *options_registered_ptr, char *options_default_ptr)
{
    int i;
    int j;

    cmdf_options *options_array = *options_array_ptr;

    options_required_ptr = calloc(sizeof(**options_required_ptr)*(0xFF*0xFF+0xFF));
    for(i = 0; i < 0xFF; i++)
        options_required_ptr[i] = (char*)options_required_ptr + (0xFF*(i+1))+1; // assign pointers

    options_registered_ptr = calloc(sizeof(*options_registered_ptr)*0xFF);
    options_default_ptr = calloc(sizeof(*options_registered_ptr)*0xFF);

    // to check for not optional options    
    int flag_option_was_non_alias_and_required = 0;

    // to check for duplicate keys
    char *all_keys = calloc(sizeof(*all_keys)*0xFF);

    // to check for aliases
    cmdf_options last_option = {0};




    /* Create new array with default and user defined options to be parsed */

    int options_len = 0;
    int default_options_len = 0;

    while(default_options[default_options_len].long_name != NULL) // default options lenght
        default_options_len++;
    
    while(default_options[options_len].long_name != NULL)
        options_len++;

    realloc(default_options, sizeof(cmdf_options)*(default_options_len+options_len+1)); //realloc to acomodate new options

    options_len = 0;
    while(default_options_len < (default_options_len + options_len)) // copy elements
    {
        default_options[default_options_len] = options_array[options_len];
        default_options_len++;
    }
    default_options[default_options_len].long_name = NULL; // last elemnt shall be zero

    free(options_array); // free original user options

    options_array = default_options;        // make it point to new array
    *options_array_ptr = default_options;   // make the passed pointer point to new array




    /* Run trought options, incrementing the size and substituting aliases, (OPTION_ALIAS). */

    options_len = 0;
    i = -1;
    j = 0;

    while(options_array[options_len].long_name != NULL)
    {
        // Duplicates
        if(strchr(all_keys, options_array[options_len].key) == NULL) // if the key is not a DUPLICATE
            strcat_char(all_keys, options_array[options_len].key); // cat key
        else
            error_handler_parse_options_internal(flags, "The key -%c from option --%s is already registered by another option.\n", options_array[options_len].key, options_array[options_len].long_name);



        // Key ascii check
        if( (options_array[options_len].parameters & OPTION_NO_CHAR_KEY) && is_letter(options_array[options_len].key)) // if no char key, then it must check to see if the key is a non assci letter
            error_handler_parse_options_internal(flags, "An option with (OPTION_NO_CHAR_KEY) specified must be a non ascii alphabetical character.\n");

        if( !(options_array[options_len].parameters & OPTION_NO_CHAR_KEY) && !is_letter(options_array[options_len].key)) // if char key, then it must check to see if the key is a asci letter
            error_handler_parse_options_internal(flags, "An option with a specified char key must be a ascii alphabetical character.\n");

        

        // Check for 0 key, reserved by the program
        if(options_array[options_len].key == 0)
            error_handler_parse_options_internal(flags, "No defined option should use the '0' key, it is reserved. The option --%s has key '0'\n",options_array[options_len].long_name);



        // Optional options
        if(!(options_array[options_len].parameters & OPTION_OPTIONAL) && !(options_array[options_len].parameters & OPTION_ALIAS))    // if not optional and non alias, !(OPTION_OPTIONAL)
        {
            flag_option_was_non_alias_and_required = 1;
            j = 0;            
            i++;
            strcat_char(options_required_ptr[i][j],options_array[options_len].key);
            j++;
        }
        else if((options_array[options_len].parameters & OPTION_ALIAS) && flag_option_was_non_alias_and_required == 1)   // if this is a alias and last option was non optional
        {
            strcat_char(options_required_ptr[i],options_array[options_len].key);
            j++;
        }
        else // if a new optional option appears, then stop adding aliases
        {
            flag_option_was_non_alias_and_required = 0;
        }
        


        // Aliases
        if(options_array[options_len].parameters & OPTION_ALIAS) // check for aliases, (OPTION_ALIAS)
        {
            if(last_option.long_name == NULL) // if the first registered option is an alias, then its obvisually ilegal
                error_handler_parse_options_internal(flags, "The first option must be a non alias option, an alias must be declared below a non alias option.\n");

            options_array[options_len].parameters = last_option.parameters;
            options_array[options_len].argq = last_option.argq;
            options_array[options_len].description = &("Alias for option above ↑");
        }
        else
        {
            last_option = options_array[options_len];
        }
        

        // Save default and registered options in strings
        if(options_len < default_options_len)
            strcat_char(options_default_ptr,options_array[options_len].key);
        else
            strcat_char(options_registered_ptr,options_array[options_len].key);



        options_len++;
    }

}



/* -------------------------------------------- Functions Implementations ---------------------------------------------- */


/**
 * @brief Set default value of cmdf_default_info_usage
 */
void set_cmdf_default_info_usage(const char *info_string)
{
    cmdf_default_info_usage = info_string;
}


/**
 * @brief Set default value of cmdf_default_info_version
 */
void set_cmdf_default_info_version(const char *info_string)
{
    cmdf_default_info_version = info_string;
}


/**
 * @brief Set default value of cmdf_default_info_contact
 */
void set_cmdf_default_info_contact_info(const char *info_string)
{
    cmdf_default_info_contact_info = info_string;
}


void option_parser(char **argv, int *count, cmdf_options *current_option, option_parse_function user_parse_function, void *extern_user_variables_struct, cdmf_PARSER_FLAGS_Typedef flags)
{
    int arguments_to_take = current_option->argq;
    char *current_argument = NULL;
    int arg_counter = 0;

    if(arguments_to_take == 0)
    {
        user_parse_function(current_option->key, NULL, arg_counter, extern_user_variables_struct);
        return;
    }
    else if(arguments_to_take == -1)
    {
        (*count)++;
        current_argument = argv[*count];

        if(current_argument[0] == '-') // and option was given after the current one
        {
            error_handler_parse_options_internal(flags, "The option -%c needs at least one valid argument.", current_option->key);
            return;
        }

        while(current_argument[0] != '-') // until another option comes
        {
            user_parse_function(current_option->key, current_argument, arg_counter, extern_user_variables_struct);

            arg_counter++; // each new argument to the option has a index given by arg_counter
            (*count)++; // read next on argv
            current_argument = argv[*count];
        }

        (*count)--; // goes back by one, because we expect a new option to end the loop

        return;
    }
    else if(arguments_to_take > 0)
    {
        while(arguments_to_take > 0) // by the times of expected arguments
        {
            //TERMINAR AQUI

            arguments_to_take--;
        }
    }
    else
    {
        error_handler_parse_options_internal(flags, "The option --%s was registered with invalid number of argument: (%i). It should be, -1, 0 or bigger than 0.", arguments_to_take);
        return;
    }

}


/**
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
    if(argc >= MAX_CMD_ARGUMENTS)
        error_handler_parse_options_internal(flags, "The maximum number of (%d) arguments was passed.\n",MAX_CMD_ARGUMENTS);

    int i = 1, j = 0, k= 0;
    int x = -1, y = 0;

    char current_key = 0;
    char* current_argument;
    int current_key_arg_counter = -1; // in case of first coming arguments with no set key, they will be processed until a key arrives
    
    char **options_required_matrix = NULL;
    char *options_default_array = NULL;
    char *options_registered_array = NULL;

    parse_registered_options(registered_options, flags, options_required_matrix, options_registered_array, options_default_array);

    cmdf_options *current_option = NULL;

    // run for every argument passed on cmd
    while(i < argc) 
    {
        current_argument = argv[i];

        if(current_argument[0]=='-' && current_argument[1]=='-')            // ------------- long name option
        {
            current_argument += 2; // remove the "--" in the beginning
            current_option = get_option_long_name(current_argument,registered_options);

            if(current_option == NULL) // option is not registered
            {
                if (flags & PARSER_FLAG_DONT_IGNORE_NON_REGISTERED_OPTIONS)
                    error_handler_parse_options_internal(flags, "The option --%s is invalid!", current_argument);
                
                i++;
                continue; // ignore if the error handler above doesn't exit the program
            }

            if(current_option->parameters & OPTION_NO_LONG_KEY) // long name of the option is not to be used
            {
                i++;
                continue;
            }

            option_parser(argv, &i, current_option, parse_function, extern_user_variables_struct, flags);
        }
        else if(current_argument[0]=='-' && strlen(current_argument)>=2)    // ------------- nested char key option
        {

        }
        else if(current_argument[0]=='-')                                   // ------------- single char key option
        {

        }
        else                                                                // ------------- floating argument
        {

        }



        // verify for default expressions -------------------------

        if(flags & PARSER_FLAG_USE_PREDEFINED_OPTIONS)
        {
            if(current_argument[0]=='-' && current_argument[1]=='-' && current_key_arg_counter<=0 ) // ---------------------------default long options
            {
                current_argument += 2; // jump over "--"

                j = 0;
                while(j < DEFAULT_OPTIONS_LENGHT) // verify if its registered
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
                while(j < DEFAULT_OPTIONS_LENGHT) // verify if its registered
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
                    current_argument -= 1; // goes back the "-" so the next else-if case can process

            }
        }


        //verify for registered arguments --------------------------------------------------------------------------
        if(current_argument[0]=='-' && current_argument[1]=='-' && current_key_arg_counter<=0 )         // ---------------------------long option
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
        else if(current_argument[0]=='-' && current_key_arg_counter<=0 && strlen(current_argument)<=2)  // ---------------------------single key option
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
        else if(current_argument[0]=='-' && current_key_arg_counter<=0 && strlen(current_argument)>2)   // ---------------------------nested key options
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
        else if(current_argument[0]=='-' && current_key_arg_counter > 0)                                // ---------------------------found a new option "-" or "--" while waiting for other arguments
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

    // if there are pending arguments for a option
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