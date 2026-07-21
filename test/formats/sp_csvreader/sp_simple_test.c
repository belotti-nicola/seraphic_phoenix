#include <seraphic_phoenix.h>
#include <string.h>

#include <internal/sp_utils.h>


int main()
{
    sp_csvreader csvReader = {0};

    char buff[128] = {0};
    
    get_executable_directory(buff,128);
    strcat(buff,"/test_data/sp_simple_test.csv");

    sp_csvreader_open(&csvReader,buff);
    
    if(sp_csvreader_next(&csvReader) == false)
    {
        TRACE_ERROR_POSITION();
        TEST_INFO("Error opening: %s",buff);
        return 1;
    }

    if(csvReader.records_size != 3)
    {
        return 1;
    }

    if(strcmp(csvReader.records[0],"usr") != 0)
    {
        return 1;
    }

    if(strcmp(csvReader.records[1],"aa") != 0)
    {
        return 1;
    }

    if(strcmp(csvReader.records[2],"mm") != 0)
    {
        return 1;
    }
    
    if(sp_csvreader_next(&csvReader) == true)
    {
        return 1;
    }

    return 0;
}