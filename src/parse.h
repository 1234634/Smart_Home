#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#define MAX_BUF_LEN 5000

void string_cpy(char** arg_cpy, const char* arg_string, int arg_size)
{
    *arg_cpy= (char*) malloc(arg_size + 1);
    memcpy(*arg_cpy, arg_string, arg_size);
    (*arg_cpy)[arg_size] = '\0';

}


char** str_split(char* a_str, const char a_delim)
{
    char** result    = 0;
    size_t count     = 0;
    char* tmp        = a_str;
    char* last_comma = 0;
    char delim[2];
    delim[0] = a_delim;
    delim[1] = 0;

    /* Count how many elements will be extracted. */
    while (*tmp)
    {
        if (a_delim == *tmp)
        {
            count++;
            last_comma = tmp;
        }
        tmp++;
    }

    /* Add space for trailing token. */
    count += last_comma < (a_str + strlen(a_str) - 1);

    /* Add space for terminating null string so caller
       knows where the list of returned strings ends. */
    count++;


   result =  (char**)malloc(sizeof(char*) * count);

    if (result)
    {
        size_t idx  = 0;
        char* token = strtok(a_str, delim);

        while (token)
        {
            assert(idx < count);
            *(result + idx++) = strdup(token);
            token = strtok(0, delim);
        }
        assert(idx == count - 1);
        *(result + idx) = 0;
    }

    return result;
}



void help()
{
    unsigned char src[MAX_BUF_LEN + 1];                                              //Bafer u koji se ucitavaju podaci
	FILE *fp = fopen("src/commands", "rb");                                        //Otvaranje fajla koji se cita
	if (fp != NULL)                                                         //Ako je fajl uspesno otvoren
    {
		size_t new_len = fread(src, sizeof(char), MAX_BUF_LEN, fp);         //Funkcija koja iz fajla upisuje karaktere u bafer src duzine MAX_BUF_LEN i smesta duzinu upsanob u new_len
		//printf("new len = %d", new_len);
		if ( ferror( fp ) != 0 )                                            //Ako se dogodi greska pri citanju fajla
		{
			fputs("Greska pri citanju fajla", stderr);                      //Ispis stringa kao greske
		}
        printf("%s", src);
		fclose(fp);                                                         //Zatvaranje fajla
	}
	else
    {
        printf("Nemere\n");
    }



}
