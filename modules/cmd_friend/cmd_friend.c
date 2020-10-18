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


#include "cmd_friend.h"


/**
 * @brief Default options array lenght.
 */
#define DEFAULT_OPTIONS_LENGTH 3


/* -------------------------------------------- Private Globals -------------------------------------------------------- */

/**
 * String defining the usage info when using default option --help
 */
const char *cmdf_default_info_usage = NULL;



/**
 * String defining the version info when using default option --version
 */
const char *cmdf_default_info_version = NULL;



/**
 * String defining the contact info when using default option --info
 */
const char *cmdf_default_info_contact_info = NULL;



/**
 * @brief Default options array, every option shall always have zero arguments.
 * Modify if necessary.
 */
const cmdf_options default_options[] = 
{
    {"help",    __HELP_KEY,    OPTION_OPTIONAL | OPTION_NO_CHAR_KEY , 0, "Shows this help menu"},
    {"info",    __INFO_KEY,    OPTION_OPTIONAL | OPTION_NO_CHAR_KEY , 0, "Shows information about the program"},
    {"version", __VERSION_KEY, OPTION_OPTIONAL | OPTION_NO_CHAR_KEY , 0, "Shows program version"},
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
void strcat_char(char *destination_string, int character)
{
    char *buffer = malloc(sizeof(*buffer)+1);

    buffer[0] = (char)character;
    buffer[1] = '\0';

    strcat(destination_string, buffer);

    free(buffer);
}



/**
 * @brief Internal error handler.
 * Called when option logic error and exceptions occur.
 * @param flags: receives flags defined by user in the main function "cdmf_parse_options" call.
 * @param format_string: Format text string jsut as in printf, also the variable arguments parameters.
 */
void error_handler_parse_options_internal(PARSER_FLAGS_Typedef flags, const char* format_string, ... )
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

    exit(1);

}



/**
 * @brief Default option parser.
 * @param key: Char key of defined function.
 * @param user_options: User define options array, used for printing them in --help option.
 */
void default_options_parser(char key, cmdf_options user_options[], PARSER_FLAGS_Typedef flags)
{
    int user_options_len = 0;
    while(user_options[user_options_len].long_name != NULL)
        user_options_len++;

    int i = 0;

    switch (key)
    {
        case __HELP_KEY:     // --help
            i = 0;
            if(cmdf_default_info_usage!=NULL)
                printf("%s\n\n",cmdf_default_info_usage);

            if(flags | PARSER_FLAG_USE_PREDEFINED_OPTIONS) // if using default options
            {
                for(i = 0; i < DEFAULT_OPTIONS_LENGTH; i++) // Default options
                {
                    if(!(user_options[i].parameters & OPTION_HIDDEN)) // if not hidden, (OPTION_HIDDEN)
                    {
                        if(is_letter(user_options[i].key))
                            printf("\t-%c ( --%s ): %s.\n", user_options[i].key, user_options[i].long_name, user_options[i].description);
                        else
                            printf("\t--%s : %s.\n", user_options[i].long_name, user_options[i].description);
                    }
                }


                while(i < user_options_len) // User defined options
                {
                    if(!(user_options[i].parameters & OPTION_HIDDEN)) // if not hidden, (OPTION_HIDDEN)
                    {
                        if(is_letter(user_options[i].key))
                            printf("\t-%c ( --%s ): %s. Takes \"%c\" arguments.\n", user_options[i].key, user_options[i].long_name, user_options[i].description, ( (user_options[i].argq == -1) ? 'n' : (user_options[i].argq + 48) ));
                        else
                            printf("\t--%s : %s. Takes \"%c\" arguments.\n", user_options[i].long_name, user_options[i].description, ( (user_options[i].argq == -1) ? 'n' : (user_options[i].argq + 48) ));
                    }

                    i++;
                }
            }
            else
            {
                while(i < user_options_len) // User defined options
                {
                    if(!(user_options[i].parameters & OPTION_HIDDEN)) // if not hidden, (OPTION_HIDDEN)
                    {
                        if(is_letter(user_options[i].key))
                            printf("\t-%c ( --%s ): %s. Takes \"%c\" arguments.\n", user_options[i].key, user_options[i].long_name, user_options[i].description, ( (user_options[i].argq == -1) ? 'n' : (user_options[i].argq + 48) ));
                        else
                            printf("\t--%s : %s. Takes \"%c\" arguments.\n", user_options[i].long_name, user_options[i].description, ( (user_options[i].argq == -1) ? 'n' : (user_options[i].argq + 48) ));
                    }

                    i++;
                }
            }



            break;

        case __VERSION_KEY:  // --version
            if(cmdf_default_info_version!=NULL)
                printf("%s\n",cmdf_default_info_version);
            break;

        case __INFO_KEY:     // --info
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
 * @brief Runs trought an array of options looking for the correponding option for a given long name.
 * @param long_name: The full name of the options.
 * @param options_array: Options array to check against.
 * @return Returns a pointer to the option struct.
 */
cmdf_options *get_option_by_long_name(char *long_name, cmdf_options *options_array)
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
 * @param key: The char key of the option.
 * @param options_array: Options array to check against.
 * @return Returns a pointer to the option struct.
 */
cmdf_options *get_option_by_key(char key, cmdf_options *options_array)
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
 * @param options_required_ptr: Pointer to array of strings, to be allocated and receive program required options to work correctly and its aliases.
 * @param options_registered_ptr: Pointer to string, to be allocated and receive all the registered options.
 * @param options_default_ptr: Pointer to string, to be allocated and receive all the default options.
 */
void parse_registered_options(cmdf_options **options_array_ptr, PARSER_FLAGS_Typedef flags, char options_required_ptr[0xFF][0xFF], char options_registered_ptr[0xFF], char options_default_ptr[0xFF])
{
    int i;
    int j;

    // to check for not optional options    
    int flag_option_was_non_alias_and_required = 0;

    // to check for duplicate keys
    char *all_keys = calloc(sizeof(*all_keys)*0xFF,1);

    // to check for aliases
    cmdf_options last_option = {0};


    /* Create new array with default and user defined options to be parsed */

    int options_len = 0;

    while((*options_array_ptr)[options_len].long_name != NULL)
        options_len++;

    int total_options_length = options_len + DEFAULT_OPTIONS_LENGTH;

    cmdf_options *options_array = calloc(sizeof(cmdf_options)*(total_options_length+1),1); // new array

    memcpy(options_array, default_options, sizeof(cmdf_options)*DEFAULT_OPTIONS_LENGTH); // copy default options
    memcpy((options_array + DEFAULT_OPTIONS_LENGTH), (*options_array_ptr), sizeof(cmdf_options)*options_len); // copy user options

    options_array[total_options_length].long_name = NULL; // last element shall be empty

    *options_array_ptr = options_array;   // make the passed pointer point to new array

    // there's no need to free the original pointer to user defined options struct, because it is stack allocated.



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
        if(options_array[options_len].key == 0)
            error_handler_parse_options_internal(flags,"The option -%c / --%s have the char key set to 0, the 0 key is reserved, please change to another int or char.\n",options_array[options_len].key,options_array[options_len].long_name);

        if( (options_array[options_len].parameters & OPTION_NO_CHAR_KEY) && is_letter(options_array[options_len].key)) // if no char key, then it must check to see if the key is a non assci letter
            error_handler_parse_options_internal(flags, "An option with (OPTION_NO_CHAR_KEY) specified must be a non ascii alphabetical character. Option: -%c / --%s.\n",options_array[options_len].key,options_array[options_len].long_name);

        if( !(options_array[options_len].parameters & OPTION_NO_CHAR_KEY) && !is_letter(options_array[options_len].key)) // if char key, then it must check to see if the key is a asci letter
            error_handler_parse_options_internal(flags, "An option with a specified char key must be a ascii alphabetical character. Option: -%c / --%s.\n",options_array[options_len].key,options_array[options_len].long_name);

        

        // Check for 0 key, reserved by the program
        if(options_array[options_len].key == 0)
            error_handler_parse_options_internal(flags, "No defined option should use the '0' key, it is reserved. The option --%s has key '0'\n",options_array[options_len].long_name);



        // Optional options
        if(!(options_array[options_len].parameters & OPTION_OPTIONAL) && !(options_array[options_len].parameters & OPTION_ALIAS))    // if not optional and non alias, !(OPTION_OPTIONAL)
        {
            flag_option_was_non_alias_and_required = 1;
            i++;
            strcat_char(options_required_ptr[i], options_array[options_len].key);
        }
        else if((options_array[options_len].parameters & OPTION_ALIAS) && flag_option_was_non_alias_and_required == 1)   // if this is a alias and last option was non optional
        {
            strcat_char(options_required_ptr[i], options_array[options_len].key);
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
            options_array[options_len].description = "Alias for the above option ^^";
        }
        else
        {
            last_option = options_array[options_len];
        }
        

        // Save default and registered options in strings
        if(options_len < DEFAULT_OPTIONS_LENGTH)
            strcat_char(options_default_ptr, options_array[options_len].key);
        else
            strcat_char(options_registered_ptr, options_array[options_len].key);


        options_len++;
    }

}



/**
 * @brief Receives an option an call user define option parser function upon the arguments that succed the option
 * @param argv: Array of strings of arguments from command line. 
 * @param count: Pointer to int counter of arguments from argv. 
 * @param current_option: Struct with the current option information. 
 * @param user_parser_function: User defined parser function to parser the options. 
 * @param extern_user_variables_struct: Pointer to user defined struct. 
 * @param flags: Flags used for the error handler function. 
 */
void option_parser(int argc, char **argv, int *count, cmdf_options *current_option, option_parse_function user_parse_function, void *extern_user_variables_struct, cmdf_options *registered_options, char *default_options, PARSER_FLAGS_Typedef flags)
{
    int arguments_to_take = current_option->argq;
    char *current_argument = NULL;
    int arg_counter = 0;
    int i = 0;

    if(arguments_to_take == 0)          // -------------- take no arguments
    {
        if(strchr(default_options,current_option->key) == NULL)    // user option
            user_parse_function(current_option->key, NULL, arg_counter, extern_user_variables_struct); // call with NULL on argument;
        else                                                        // default option
            default_options_parser(current_option->key, registered_options, flags);


        return;
    }
    else if(arguments_to_take == -1)    // -------------- take as many arguments as possible                            
    {

        if((*count) < (argc - 1)) // to prevent for acessing undesirable memory beyond the argv array
            (*count)++;
        
        current_argument = argv[*count];

        if(current_argument[0] == '-') // and option was given after the current one
        {
            error_handler_parse_options_internal(flags, "The option -%c / --%s needs at least one valid argument.\n", current_option->key, current_option->long_name);
            return;
        }

        while(current_argument[0] != '-') // until another option comes
        {
            if(strchr(default_options, current_option->key) == NULL) // if the current option is not a default option
                user_parse_function(current_option->key, current_argument, arg_counter, extern_user_variables_struct);
            else
                default_options_parser(current_option->key, registered_options, flags);


            arg_counter++; // each new argument to the option has a index given by arg_counter
            
            if((*count) < (argc - 1)) // to prevent for acessing undesirable memory beyond the argv array
                (*count)++; // read next on argv
            else 
                return; // exit function after all cmd arguments have been read 

            current_argument = argv[*count];
        }

        (*count)--; // goes back by one, because we need to re read the new option

        return;

    }
    else if(arguments_to_take > 0)      // -------------- take "n" arguments  
    {

        if((*count) < (argc - 1)) // to prevent for acessing undesirable memory beyond the argv array
            (*count)++;
        
        while( (argv[*count] != NULL) && ((current_argument = argv[*count])[0] != '-') ) // until another option comes or the end of argv
        {
            if(strchr(default_options,current_option->key) == NULL) // if the current option is not a default option
                user_parse_function(current_option->key, current_argument, arg_counter, extern_user_variables_struct);
            else
                default_options_parser(current_option->key, registered_options, flags);
          
            (*count)++;

            arg_counter++; // each new argument to the option has a index given by arg_counter            
        }

        if(arg_counter > arguments_to_take)
        {
            error_handler_parse_options_internal(flags, "The option -%c / --%s has too many arguments, it only receives \"%i\" many.\n",current_option->key,current_option->long_name,current_option->argq);
        }
        else if(arg_counter < arguments_to_take)
        {
            error_handler_parse_options_internal(flags, "The option -%c / --%s has too few arguments, it expects at least \"%i\".\n",current_option->key,current_option->long_name,current_option->argq);
        }

        (*count)--; // goes back by one, because we need to re read the new option

        return;
    }
    else
    {
        error_handler_parse_options_internal(flags, "The option --%s was registered with invalid number of argument: (%i). It should be, -1, 0 or bigger than 0.\n", arguments_to_take);
        return;
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



/**
 * @brief Main library function, used to parse options in main program.
 * The "cdmf_parse_options" function will call the user defined function to handle actions as desired.
 * Each call will have a key and argument, where each argument must have be assigned to only one key.
 * @param registered_options: Receives pointer to struct "cmdf_options" array that shall contain user define options.
 * @param parse_function: User defined parse function pointer.
 * @param argc: Main function parameter containing number of passed parameters in command line.
 * @param argv: Main function parameter containing the array of string containing the actual parameters.
 * @param flags: Flags used to customize the function "cdmf_parse_options" behavior, flags shall be located on "PARSER_FLAGS_Typedef" enumerator.
 * @param extern_user_variables_struct: Opaque pointer to user define struct in main program, used to be accessed in the also user define parser function.
 */
int cdmf_parse_options(cmdf_options *registered_options, option_parse_function user_parse_function, int argc, char **argv, PARSER_FLAGS_Typedef flags, void *extern_user_variables_struct)
{
    if(argc >= MAX_CMD_ARGUMENTS)
        error_handler_parse_options_internal(flags, "The maximum number of (%d) arguments was passed.\n", MAX_CMD_ARGUMENTS);

    int i = 0, j = 0, k= 0;
    int x = -1, y = 0;

    char current_key = 0;
    char* current_argument;
    int current_key_arg_counter = -1; // in case of first coming arguments with no set key, they will be processed until a key arrives
    
    char options_required_matrix[0xFF][0xFF] = {0};          // required options with respective aliases
    char options_default_array[0xFF] = {0};             // default options given by the library
    char options_registered_array[0xFF] = {0};          // all the non default options
    char options_passed_array[0xFF] = {0};   // the options given on the cmd

    parse_registered_options(&registered_options, flags, options_required_matrix, options_registered_array, options_default_array);

    cmdf_options *current_option = NULL;

    // run for every argument passed on cmd
    i = 1; // to the first cmd argument after the programs name
    while(i < argc) 
    {
        current_argument = argv[i];

        if(current_argument[0]=='-' && current_argument[1]=='-')            // ------------- long name option
        {

            current_argument += 2; // remove the "--" in the beginning
            current_option = get_option_by_long_name(current_argument,registered_options);

            if(current_option == NULL) // option is not registered
            {
                if (flags & PARSER_FLAG_DONT_IGNORE_NON_REGISTERED_OPTIONS)
                    error_handler_parse_options_internal(flags, "The option --%s is invalid!\n", current_argument);
                

                if(i < argc) // to prevent for acessing undesirable memory beyond the argv array
                {
                    i++;
                    continue; // ignore if the error handler above doesn't exit the program
                }
                else
                    break; // exit from while
            }

            strcat_char(options_passed_array, current_option->key); // remember option

            if(current_option->parameters & OPTION_NO_LONG_KEY) // long name of the option is not to be used
            {
                if(i < argc) // to prevent for acessing undesirable memory beyond the argv array
                {
                    i++;
                    continue;
                }
                else
                    break; // exit from while

            }

            option_parser(argc, argv, &i, current_option, user_parse_function, extern_user_variables_struct, registered_options, options_default_array, flags);

        }
        else if(current_argument[0]=='-')                                   // ------------- char key option
        {
            
            current_argument += 1; // jump over the "-" 

            j = 0;
            while(current_argument[j] != '\0') // for every letter in option
            {
                current_option = get_option_by_key(current_argument[j], registered_options);

                if(current_option == NULL) // option is not registered
                {
                    if (flags & PARSER_FLAG_DONT_IGNORE_NON_REGISTERED_OPTIONS)
                        error_handler_parse_options_internal(flags, "The option -%s is invalid!\n", current_argument);
                    
                    j++;
                    continue; // ignore if the error handler above doesn't exit the program
                }

                if( (strlen(current_argument) > 1) && (current_option->argq != 0) ) // check for when nested options come, trown an error if one of then requires an argument. only no argument options can be nested.
                    error_handler_parse_options_internal(flags, "Only nested options can be nested in a single \"-\". Nested options passed: -%s , Option that requires arguments: -%c.\n",current_argument,current_option->key);

                strcat_char(options_passed_array, current_option->key); // remember option

                option_parser(argc, argv, &i, current_option, user_parse_function, extern_user_variables_struct, registered_options, options_default_array, flags);

                j++;
            }

        }
        else                                                                // ------------- floating argument
        {
            user_parse_function(0, current_argument, i, extern_user_variables_struct); // pass alone argument with the 0 key
        }

        i++;
    }

    // check for required options if no default option was called
    if(strpbrk(options_passed_array, options_default_array) == NULL)
    {
        i = 0;
        while(options_required_matrix[i][0] != '\0')
        {
            if(strpbrk(options_passed_array,options_required_matrix[i]) == NULL)
            {
                cmdf_options *required_option = get_option_by_key(options_required_matrix[i][0], registered_options);
                error_handler_parse_options_internal(flags, "The option -%c / --%s needs to be specified.\n",required_option->key,required_option->long_name);
            }

            i++;
        }
    }


    return 0;
}

#ifdef __cplusplus 
} 
#endif 