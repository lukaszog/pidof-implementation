#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>
#include<dirent.h>
#include<libgen.h>
#include<sys/types.h>
#include<signal.h>

int check(char *dir)//test kropeczek
{
	int i;
	for(i=0; dir[i]!= '\0'; i++)//koniec lini
	{
		if(!isdigit(dir[i]))
		{
			return 0;
		}
	}
	return 1;
}


int *pidofImpl(char *processname)//wszystko wskazniki
{
	DIR *dir;//katalog
	FILE *f;//plik
	struct dirent *record;//struktura

	int *pidlist,pidlist_x=0, pidlist_realloc=1;//lista pidow
	char path[1024], buffer[1024];//sciezka i bufor

	dir = opendir("/proc/");//otwieramy katalog proc
	if(dir == NULL)//test czu sie da
	{
		perror("Nie moge otworzyc katalogu /proc/");
		return NULL;
	}

	//udalo sie, dzialam dalej
	pidlist = malloc(sizeof(int) * 32);//dynamiczna allokacja pamieci dla listy pidow
	if(pidlist == NULL)//test czy wszystko ok
	{
		perror("Blad przy alokacji pamieci");
		return NULL;
	}

	while((record = readdir(dir)) != NULL)//petla bedzie trwac az katalog nie pedzie pusty
	{
		if(check(record->d_name))//test kropeczek
		{
			strcpy(path, "/proc/");//laczenie tekstu
			strcat(path, record->d_name);
			strcat(path, "/comm");

			f = fopen(path, "r");//otwieramy plik
			//printf("%s\n", path);

			if(f != NULL)//jezeli istnieje otwarty plik
			{
				fscanf(f, "%s", buffer);//zczytujemy zawartosc pliku do bufora
				if(strcmp(buffer, processname) == 0)//porownuje bufor i nazwe procesu
				{
					pidlist[pidlist_x++] = atoi(record->d_name);//pidlist jest rowny czesci struktury record
				}

				//relokacja pamieci do zrobienia potem
				if(pidlist_x == 32 * pidlist_realloc)//32 rozmiar typu tzn pid list x ma rozmiar warty pidlistreall razy rozmiar typu
				{
					pidlist_realloc++;//inkrementacja
					pidlist = realloc(pidlist, sizeof(int) * 32 * pidlist_realloc);//realokacja pamieci
					if(pidlist == NULL)//test bledu istnienie pidlist
					{
						perror("Blad przy relokacji pamieci");
						return NULL;
					}
				}
			}else{
				perror("Nie moge otworzyc pliku");
				return NULL;
			}
			fclose(f);//zamukamy plik
		}		 
	}

	closedir(dir);//zamykamy katalog
	pidlist[pidlist_x] = -1;//konczymy liste

	return pidlist;//zwraca pidlist
}

int send_signal(int pid)//zabijanie procesu danego pida
{
	kill(pid, SIGINT);//ktory program i jaki sygnal
}


int main(int argc, char **argv)//main
{
	int *processInfo, i, killflag=0;//wskaznik na proces i flaga mordu

	if(argc != 2)//jezeli brak parametru
	{
		printf("Uzycie programu: %s nazwa_procesu\n", argv[0]);
		return 0;
	}

	if(strcmp(argv[0], "./killall") == 0)//jezli wywolamu program przez kill
	{
		//printf("Odpalamy killall");
		killflag=1;//ustawiamy flage
	}

	processInfo = pidofImpl(argv[1]);//zczytujemy parametr nazwe

	for(i=0; processInfo[i] != -1; i++)//iterujemy po tym z wrocila funcja (liste adresow)
	{
		if(killflag == 1)//test flagi
		{
			send_signal(processInfo[i]);//i pokolei zabijamy
		}
		printf("%d ", processInfo[i]);//wystwietlamy
	}

	free(processInfo);//zwalniamy pamiec

	printf("\n");//nowa linia

	return 0;//koniec
}