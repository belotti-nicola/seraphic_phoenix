#include <seraphic_phoenix.h>
#include <string.h>

#include <internal/sp_utils.h>


int main()
{
    char buff[128] = {0};

    sp_csvreader csvReader = {0};
    
    get_executable_directory(buff,128);
    strcat(buff,"/test_data/sp_simple_test.csv");

    if(sp_csvreader_open(&csvReader,buff) == false)
    {
        TRACE_ERROR_POSITION();
        TEST_INFO("Error opening: %s",buff);
        return 1;
    }

    sp_csvreader_close(&csvReader);  
    

    return 0;
}