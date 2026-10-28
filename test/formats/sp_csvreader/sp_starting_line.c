#include <seraphic_phoenix.h>
#include <string.h>

#include <internal/sp_utils.h>


int main()
{
    sp_csvreader csvReader = {0};

    char buff[128] = {0};
    
    get_executable_directory(buff,128);
    strcat(buff,"/test_data/sp_starting_line.csv");

    if ( sp_csvreader_open(&csvReader,buff) == false ) 
    {
        TRACE_ERROR_POSITION();
        TEST_INFO("Error opening test file");
        return 1;
    }
    
    int entry_counter = 0;
    while ( sp_csvreader_next(&csvReader) )
    {
        switch ( entry_counter )
        {
            case 0:
            {
                if ( csvReader.starting_line != 1)
                {
                    TRACE_ERROR_POSITION();
                    TEST_INFO("Error: computed starting_line %d differs from expected one %ld ",csvReader.starting_line, 1);
                    return 1;
                }

                break;
            }
            case 1:
            {
                if ( csvReader.starting_line != 6)
                {
                    TRACE_ERROR_POSITION();
                    TEST_INFO("Error: computed starting_line %d differs from expected one %ld ",csvReader.starting_line, 6);
                    return 1;
                }
                
                break;
            }
            case 2:
            {
                if ( csvReader.starting_line != 11)
                {
                    TRACE_ERROR_POSITION();
                    TEST_INFO("Error: computed starting_line %d differs from expected one %ld ",csvReader.starting_line, 11);
                    return 1;
                }
                
                break;
            }
            case 3:
            {
                if ( csvReader.starting_line != 13)
                {
                    TRACE_ERROR_POSITION();
                    TEST_INFO("Error: computed starting_line %d differs from expected one %ld ",csvReader.starting_line, 13);
                    return 1;
                }
                
                break;
            }
            case 4:
            {
                if ( csvReader.starting_line != 14)
                {
                    TRACE_ERROR_POSITION();
                    TEST_INFO("Error: computed starting_line %d differs from expected one %ld ",csvReader.starting_line, 14);
                    return 1;
                }
                
                break;
            }
            case 5:
            {
                if ( csvReader.starting_line != 15)
                {
                    TRACE_ERROR_POSITION();
                    TEST_INFO("Error: computed starting_line %d differs from expected one %ld ",csvReader.starting_line, 15);
                    return 1;
                }
                
            }
            default:
            {
                TRACE_ERROR_POSITION();
                TEST_INFO("Error: entry_counter is not expected (%d)",entry_counter);
                return 1;
            }

            entry_counter += 1;
        }
    }
    

    return 0;
}