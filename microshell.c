#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <pwd.h>
#include <fcntl.h>

#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_RESET "\x1b[0m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_YELLOW "\x1b[33m"

char komenda[100];
char ostatnia[100];
char ostatnia2[100];
char ostatni_foldery[100];
char historia[100][100];
int n_historia = 1;
struct stat st = {0};
char pat[100];
char pw[100];
int zwrot = 0;

void help()
{
	printf(ANSI_COLOR_CYAN "Projekt Microshell \n\nTworca: Adam Hacia \n\nObecne komendy to: \n\nhelp - wyswietla informacje o autorze i funkcjach powloki \n\nexit - konczy dzialanie powloki \n\ncd [katalog] - umozliwia zmiane katalogu \n\n\tcd .. - przenosi do nadrzednego katalogu\n\tcd . - pokazuje biezacy katalog\n\tcd - - przenosi do poprzedniego katalogu\n\tcd ~ lub cd - przenosi do katalogu domowego\n\nhistory - wyswietla historie uzytych komend w danej sesji\n\nmkdir [flagi] - tworzy katalogi\n\tmkdir -p A/B - tworzy katalog A z podkatalogiem B\n\tmkdir A\\ B - tworzy katalog o zanwie \'A B\' \n\tmkdir A B - tworzy dwa katalogi A i B\n\tmkdir -p A/{B,C} - tworzy katalog A, a w nim dwa podkatalogi B i C\n\nwc [flagi] [nazwa pliku] - zlicza rozne dane w pliku tekstowym\n\twc - zwroci ilosc znakow, lini i slow\n\twc -l - zwroci ilosc lini w pliku\n\twc -w - zwroci ilosc slow w pliku\n\twc -L - zwroci dlugosc najdluzszej linii w pliku\n\twc -m - zwroci ilosc znakow w pliku\n\twc -c - zrowci ilosc bitow w pliku\n\n" );

};

void history()
{
	int q;
	for(q=1; q<n_historia; q++)
		printf(ANSI_COLOR_GREEN "%d %s\n" ANSI_COLOR_RESET, q, historia[q]);
};

void make(char *s, int x)
{
	if(stat(s, &st) == -1)
		mkdir(s, 0777);
	else if(x==0)
		printf("Istnieje folder o danej nazwie %s\n", s);	
};

void makedir_p(char split[])
{
	char *schowek2;
	char *split2[100];
	int k=0,j;
	int q=-1,w=-1,e;
	for(e=0;e<strlen(split);e++)
	{
		if(split[e]=='{'&&q==-1)
			q=e;
		if(split[e]=='}'&&w==-1)
			w=e;
	}
	for(e=q;e<=w;e++)
		if(split[e]=='/')
			split[e]='+';
	split2[1]=0;
	schowek2 = strtok(split,"/");
	while( schowek2 != NULL)
	{
		split2[k]=schowek2;
		k=k+1;
		schowek2 = strtok( NULL, "/");			
	}
	for(e=q;e<=w;e++)
		if(split[e]=='+')
			split[e]='/';
	strcpy(ostatni_foldery,getcwd(pat,sizeof(pat)));		
	for(j=0;j<k;j++)
	{	
		if(split2[j][0]=='{')
		{	
			zwrot=1;
			char *schowek3;
			char *split3[100];
			int l=0,m;
			split3[1]=0;
			split2[j][0]='!';
			split2[j][strlen(split2[j])-1]='!';

			int t=-1,y=-1,u;
			for(u=0;u<strlen(split2[j]);u++)
			{
				if(split2[j][u]=='{'&&t==-1)
					t=u;
				if(split2[j][u]=='}'&&y==-1)
					y=u;
			}
			for(u=t;u<=y;u++)
				if(split2[j][u]==',')
					split2[j][u]='+';

			schowek3 = strtok(split2[j],",!");
			while( schowek3 != NULL)
			{
				split3[l]=schowek3;
				l=l+1;
				schowek3 = strtok( NULL, ",!");			
			}
			for(u=t;u<=y;u++)
				if(split2[j][u]=='+')
					split2[j][u]=',';	

			chdir(split2[j-1]);
			for(m=0;m<l;m++)
			{
				makedir_p(split3[m]);
				chdir(split2[j-1]);	
			}
			chdir("~");
		}	
		else
		{
			if(j==k-1)
				make(split2[j],0);
			else
				make(split2[j],1);
			chdir(split2[j]);
		}
	}
	chdir(ostatni_foldery);
	if(zwrot==1)
		chdir(pw);
};

void makedir(char **split, int i,int p)
{
	strcpy(pw,getcwd(pat,sizeof(pat)));
	int j=1,s;
	for(j=1;j<i;j++)
	{
		s=0;
		if(split[j][strlen(split[j])-1]=='\\')	
		{	
			if(j==i-1)
				break;
			split[j][strlen(split[j])-1]=' ';
			strcat(split[j],split[j+1]);
			s=1;
		}
		if(p==0)
		{
			make(split[j],0);
		}
		else if(p==1&&j!=1)
		{		
			makedir_p(split[j]);	
		}
		j=j+s;
	}
};


void wc(char **split)
{
	struct stat sb;
	int bytes=0, chars=0, lines=0, max_line=0, words=0, j=1, i, max=0, flaga=0;
	int n_bytes=0, n_chars=0, n_lines=0, n_max_line=0, n_words=0;
	while(1)
	{
		if(split[j][0]!='-')
			break;
		for(i=1; i<strlen(split[j]); i++)
		{
			if(split[j][i]=='c')
			{
				bytes=1;
				flaga = flaga + 1;
			}
			else if(split[j][i]=='m')
			{
				chars=1;
				flaga = flaga + 1;
			}
			else if(split[j][i]=='l')
			{
				lines=1;
				flaga = flaga + 1;
			}
			else if(split[j][i]=='L')
			{
				max_line=1;
				flaga = flaga + 1;
			}
			else if(split[j][i]=='w')
			{
				words=1;
				flaga = flaga + 1;
			}
		}	
		j = j + 1;
	}
	if(stat(split[j], &sb) == -1)
	{
		perror("Nie znaleziono danego pliku");
	}
	else
	{
		n_bytes = sb.st_size;
		printf("Informacje o pliku %s\n", split[j]); 
		if(bytes==1||flaga==0)
			printf("Ilosc bitow w pliku: %d\n", n_bytes);
		FILE *fp;
		char ch;
		int powtorka = 0;
		fp = fopen(split[j],"r");
		while((ch = getc(fp)) != EOF)
		{
			n_chars = n_chars + 1;
			if(ch == '\t' || ch == ' ' || ch == '\n')
			{
				if(powtorka != 1)
					n_words = n_words + 1;
				powtorka = 1;		
			}
			else
				powtorka = 0;
			if(ch == '\n')
			{
				if(n_max_line > max)
					max = n_max_line;
				n_max_line = 0;
				n_lines = n_lines + 1;

			}
			else
				n_max_line = n_max_line + 1;		
		}
		if(chars==1)
			printf("Ilosc znakow w pliku: %d\n", n_chars);
		if(lines==1||flaga==0)
			printf("Ilosc lini w pliku: %d\n", n_lines);
		if(max_line==1)
			printf("Dlugosc najdluzszej lini w pliku: %d\n", max);
		if(words==1||flaga==0)
			printf("Ilosc slow w pliku: %d\n", n_words);
		
	}	
	

	
	
};

void cd(char **split, int i)
{	
	char* home = getenv("HOME");
	if(i!=1 && strcmp("-",split[1]) == 0)
	{
		strcpy(ostatnia2,getcwd(pat,sizeof(pat)));
		chdir(ostatnia);	
		strcpy(ostatnia,ostatnia2);
	}
	else
	{
		strcpy(ostatnia,getcwd(pat,sizeof(pat)));
		if(i==1 || strcmp("~",split[1]) == 0 || strcmp("",split[1]) == 0)
			chdir(home);	
		else if (chdir(split[1])==-1)
			printf("Nie ma takiego folderu jak: %s\n", split[1]);	
		else if (strcmp("..",split[1])!=0)
			chdir(split[1]);
	}
};

void check_command(char *komenda)
{
	char korektor[] = " ";
	char *schowek;
	char *split[10];
	char kop[100];
	strcpy(kop,komenda);
	int i=0;
	split[1]=0;
	schowek = strtok( komenda, korektor );
	while( schowek != NULL)
	{
		split[i]=schowek;
		i=i+1;
		schowek = strtok( NULL, korektor );
				
	}
	if(i!=0)
	{	
		strcpy(historia[n_historia],kop);
		n_historia=n_historia+1;
	}
	if(strcmp("cd",split[0])==0)
	{	
		cd(split,i);
	}
	else if(strcmp("mkdir",split[0])==0)
	{
		if(strcmp("-p",split[1])==0)
			makedir(split,i,1);
		else
			makedir(split,i,0);
	}
	else if(strcmp("history",split[0])==0)
	{
		history();
	}
	else if(strcmp("help",split[0])==0)
	{
		help();
	}
	else if(strcmp("wc",split[0])==0)
	{
		if(i!=1)
			wc(split);
		else
		{
		printf("Zle parametry\n");
		}
	}
	else
	{
		if(fork() == 0)
		{
			split[i]=NULL;
			if(execvp(split[0],split) == -1)
			{
				perror("Blad wykonania polecenia");
				exit(1);
			}
		}
		else
			wait(0);
	}
};



void prompt()
{
	char path[100];
	printf(ANSI_COLOR_RED "%s:", getlogin());
	printf(ANSI_COLOR_YELLOW "%s $ "ANSI_COLOR_RESET, getcwd(path,sizeof(path)));
	
};

int main()
{
	while(1)
	{	
		prompt();
		fgets(komenda, 100, stdin);
		if(komenda[strlen(komenda)-1]=='\n')
			komenda[strlen(komenda)-1]=0;	
		if(strcmp(komenda,"exit")==0)
		{
			printf(ANSI_COLOR_MAGENTA"Do zobaczenia\n"ANSI_COLOR_RESET);
			exit(0);
		}
		else
			check_command(komenda);

	}
	return 0;
}
