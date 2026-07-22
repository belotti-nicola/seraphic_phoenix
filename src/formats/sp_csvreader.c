#include <internal/formats/sp_csvreader.h>

bool sp_csvreader_open(sp_csvreader *reader, const char *path)
{
    if(reader == NULL || reader->records == NULL) return false;

    FILE *ptr = fopen(path,"r");
    if(ptr == NULL) return false;

    reader->file         = ptr;
    reader->records_size = 0;

    return true;
}

bool sp_csvreader_next(sp_csvreader *reader)
{
    if(reader == NULL || reader->file == NULL) return false;

    reader->records_size = 0;

    char c;
    size_t counter = 0;
    bool first_slash_found = false;
    bool comment = false;
    while(true)
    {
        if(counter == BUFFER_SIZE) break;

        c = fgetc(reader->file);
        
        if(c == EOF) break;

        if(c == '\r' || c == '\n' || c == '\t') 
        {
            comment = false;
            continue;
        }

        if(comment == true) continue;

        if(c == '/' && first_slash_found == false)
        {
            first_slash_found = true;
            continue;
        }

        if(c == '/' && first_slash_found == true)
        {
            first_slash_found = false;
            comment = true;
            continue;
        }

        if(c != ';')
        {
            reader->buffer[counter] = c;
            reader->buffer[counter+1] = '\0';
            counter++;
            continue;
        }

        break;
    }

    if(counter == 0) return false;

    int state = 0;
    
    for(size_t tmp=0 ; tmp < counter ; tmp++ )
    {
        char c = reader->buffer[tmp];
        if(c == EOF) break;
        switch(state)
        {
            case 0:
            {
                if(c == '\n' || c == '\t' || c == '\r' || c == ' ') break;

                if(c == '/')
                {

                }

                size_t offset = reader->records_size;
                if(c == '"')
                {
                    reader->records[offset] = reader->buffer + tmp +1;
                    reader->records_size   += 1;
                    state = 1;
                    break;
                }
                reader->records[offset] = reader->buffer + tmp;
                reader->records_size   += 1;
                state = 3;
                break;
            }
            case 1:
            {
                if(c == '"')
                {
                    reader->buffer[tmp] = '\0';
                    state = 2;
                    break;
                }
                break;  
            }
            case 2:
            {
                if(c == '\n' || c == '\t' || c == '\r' || c == ' ') break;

                if(c == ',')
                {
                    reader->buffer[tmp] = '\0';
                    state = 0;
                    break;
                }
                if(c == ';')
                {
                    reader->buffer[tmp] = '\0';
                    state = 4;
                }
                break;
            }
            case 3:
            {
                if(c == '\n' || c == '\t' || c == '\r' || c == ' ') break;

                if(c == ',')
                {
                    reader->buffer[tmp] = '\0';
                    state = 0;
                    break;
                }
                if(c == ';')
                {
                    reader->buffer[tmp] = '\0';
                    state = 4;
                }
                break;
            }
            case 4:
            {
                break;
            }
            default:
            {
                return false;
            }
        }
    }

    return true;
}

bool sp_csvreader_close(sp_csvreader *reader)
{
    if(reader == NULL || reader->file == NULL) return false;

    return true;
}