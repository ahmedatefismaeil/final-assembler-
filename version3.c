// file description: reads file named "first assembly.txt" and removes full comment lines and any beginning blank spaces at the beginning
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct tree_node
{
	char label[30];// name of label
	int line;// number of instruction
	struct tree_node* next_ptr;// pointer to next node
};
typedef struct tree_node treeNode;
typedef struct tree_node* treeNodePtr;

int read_file(char***);//reads assembly file
void locate_labels(treeNodePtr*,int,char**); // makes a tree of labels
void insert_node(treeNodePtr*,int,char [30]);
void print_labels(treeNodePtr);
//was used to check that labels are collected

void dectobin(char*,int,int);// returns a string (fist input) of size given by third input which is the binary of decimal (second input)
void hextobin(char*,char[],int);// returns a string (first input) of size given by thitd input which is the binary of hexadecimal (second input)
void opcode(char**,char[][32],int,treeNodePtr);// stores the opcode of instructions input1 is the assembly code input2 is the binary file, third input is number of lines
int fetch_op(char[][32],char[],int);//transfer assembly into machine code

int fetch_label(char [30],treeNodePtr);

int main( void )
{
	char ***code_ptr;// pointer to 2d array
	char **code = (char **)calloc(1,sizeof(char*));// allocate 2d array of size 1
	code_ptr = &code;// store array address in code_ptr 
	int line = read_file(code_ptr);// read assembly file, return the number of lines
	int i,j;//used as counters
	char bin_file[line][32];
	treeNodePtr root_node = NULL;// forms a root node initialized to null
	locate_labels(&root_node,line,code);// forms tree of labels
	
	opcode(code,bin_file,line,root_node);
		
	/*for(i = 0;i < line;i++)
	{
		for(j = 0 ; j < 32; j++)
		{
			putchar(bin_file[i][j]);
		}
		printf("\n");
	}*/
	
	//print_labels(root_node);// has a problem when coming after the opcode function 
	
	/*for (i = 0; i < line; i++)
	{
		printf("%d\t",i);
		for (j = 0; (code[i][j] != '\n') ; j++)
		{
			putchar(code[i][j]);
		}
		printf("\n");
	}*/
	return 0;
}

int read_file(char*** code)
{
	// open file
	int c;
	FILE *file;
	file = fopen("first assembly.txt","r");
	
	// make a 2d array
	**code = (char*)calloc(150,sizeof(char));
	char** code_new;
	
	//counters for line and characters and 
	int line = 0 , character = 0;
	
	//flag to show if line has code or not
	char flag = 1;
	
	if(file)//checks if file exists
	{
		while( (c = getc(file)) != EOF )//fetches characters from file  
		{
			if( ((c != ' ') && (c != '\t' )) || (flag != 1))//ignores spaces at the beginning
			{
				if( (c == ';') && (flag == 1) )//ignores comments
				{
					while ( (c != '\n') && (c != EOF) )//stops at end of line or file
						c = getc(file);//removes comment characters
					character = 0;//reset character counter
					if ( c == EOF )
					{	
						break;//exit reading loop
					}
				}
				else 
				{
					if(flag != 1 || c != '\n')//remove blank lines
					{
						flag = 0;
						if(c == ';')//if there is comment remove it
						{
							while((c != '\n') && (c != EOF))
								c = getc(file);
								if( c == EOF)
									break;
						}					
						*(*(*code+line)+character) = c; // store char in array
						character++;//goto next char
						//putchar(c);
						if(c == '\n')
						{
							line++;//go to next line
							flag = 1;//resets flag of start of line
							character = 0;//rese	ts char counter
							*code = (char **)realloc( *code , ( sizeof(char*) * (line+1)) );//makes a new line
							if (*code)
								*(*code+line) = (char *)calloc(150,sizeof(char));//makes an array in new line
							else
								printf("no memory for extending array");
						}
					}
				}	
			}			
		}
		*(*(*code+line)+character)='\n';
		fclose(file);
		int flag2 = 1;
		for(character = 0; *(*(*code+line)+character) != '\n';character++ )
		{
			if (*(*(*code+line)+character) != ' ' && *(*(*code+line)+character) != '\t')
			{
				flag2 = 0;
			}
		}
		if (flag2)
		{
			line--;
		}
	}
	else
		printf("problem openning file\n");
return line;	
}

void locate_labels(treeNodePtr* tree_ptr,int line,char** code)//forms a tree of labels
{
	int i,j;//counters
	char* label;//pointer to : if existed
	char name[30];//label name
	for(i = 0; i<=line; i++)// loops on all lines
	{
		label = strchr(*(code+i),':');//sees if there is label or not
		if (label)
		{
			for(j = 0; ((code[i][j] != ' ') && (code[i][j] != ':'));j++)
			{	
				name[j] = code[i][j];//copys label name
			}
			name[j] = 0;// add null terminator
			insert_node(tree_ptr,i,name);//makes a new node to enter label
		}
	}
}

void insert_node(treeNodePtr* treePtr,int line,char label[30])//makes a new node
{
	int i;
	if(*treePtr)//if root exists
	{
		insert_node(&( (*treePtr)-> next_ptr),line,label);//goes to next node
	}
	else
	{
		*treePtr = (treeNodePtr)malloc( sizeof( treeNode ) );//makes a root node
		if ( *treePtr ) //checks if there is memory
		{
			for( i = 0; i < 30 ; i++)
				( *treePtr )->label[i] = label[i];//copy label name to node
			( *treePtr )->line = line;// copy label line to node
			( *treePtr )->next_ptr = NULL;// set the node to leaf
		} 
		else 
		{
			printf( "%d not inserted. No memory available.\n", line );
		}
	}
}

void print_labels(treeNodePtr treePtr)
{
	if(treePtr)
	{
		printf("%s   %d",treePtr->label,treePtr->line);
		print_labels(treePtr->next_ptr);
	}
}

void dectobin(char* number,int value,int len)// 26 bits for j instructions unsigned only
{
	unsigned c;
	unsigned displayMask = 1 << len-1;
	for ( c = 1; c <= len; c++ )
	{
		//putchar( value & displayMask ? '1' : '0' );
		*(number+c-1) = value & displayMask ? '1' : '0';
		value <<= 1;
	}
}

void hextobin(char* newn,char number[],int len)
{
	
	int ans = 0;//store the decimal number
	int i,j;// used as counters
	int power = 1;//power of hexadecimal
	for(i=0;number[i] != 0;i++)
	{
		j = i;//find the end of the hexa number
	}
	for(; j >= 0; j--)
	{
		if(((number[j] >= 48) && (number[j] <=57)))
		{
			ans += (number[j]-48)*power;//covert ascii of number to number 
		}
		else if((number[j] >= 65) && (number[j] <= 70))
		{
			ans += (number[j]-55)*power;//convert ascii letter uppercase to number

		}
		else if(((number[j] >= 97) && (number[j] <= 102)))
		{
			ans += (number[j]-87)*power;//convert ascii letter lowercase to number
		}
		power *=16;// power is multiped by 16
	}
	dectobin(newn,ans,len);// dec to bin
}

void opcode(char** code,char bin_file[][32],int line,treeNodePtr tree)
{
	char* label;
	char label_name[30];
	int counter;
	int reg;
	char number[50];
	int x;
	int i;
	int j;
	int character;
	char instruction[100];
	for(i = 0; i <= line; i++)
	{
		//printf("reached line# %d",i);
		label = strchr(code[i],':');
		character = 0;
		if(label)
		{
			for(;code[i][character] != ':';character++);
			character++;
			for(; (code[i][character] == ' ') || (code[i][character] == '\t'); character++);//finds instruction after label 
		}
		j = 0;
		for (;(code[i][character] != ' ') && (code[i][character] != '\t') && (code[i][character] != '\n');character++ )
		{
			instruction[j] = code[i][character];
			j++;
		}
		instruction[j] = 0;
		x=fetch_op(bin_file,instruction,i);
		
		if (x ==  0)//3 registers finished and revised
		{
			for(;code[i][character] == ' ' || code[i][character] == '\t';character++);
			if(code[i][character] != 'r' && code[i][character] != 'R')
			{
				printf("name of RD in line %d is wrong\n",i);
			}
			reg = (code[i][character+1]*10) + code[i][character+2] - 480 - 48;
			switch(reg)
			{
				case 0:
				{
					bin_file[i][14] = '0';
					bin_file[i][15] = '0';
					bin_file[i][16] = '0';
					bin_file[i][17] = '0';
					break;
				}
				case 1:
				{
					bin_file[i][14] = '0';
					bin_file[i][15] = '0';
					bin_file[i][16] = '0';
					bin_file[i][17] = '1';
					break;
				}
				case 2:
				{
					bin_file[i][14] = '0';
					bin_file[i][15] = '0';
					bin_file[i][16] = '1';
					bin_file[i][17] = '0';
					break;
				}
				case 3:
				{
					bin_file[i][14] = '0';
					bin_file[i][15] = '0';
					bin_file[i][16] = '1';
					bin_file[i][17] = '1';
					break;
				}
				case 4:
				{
					bin_file[i][14] = '0';
					bin_file[i][15] = '1';
					bin_file[i][16] = '0';
					bin_file[i][17] = '0';
					break;
				}
				case 5:
				{
					bin_file[i][14] = '0';
					bin_file[i][15] = '1';
					bin_file[i][16] = '0';
					bin_file[i][17] = '1';
					break;
				}
				case 6:
				{
					bin_file[i][14] = '0';
					bin_file[i][15] = '1';
					bin_file[i][16] = '1';
					bin_file[i][17] = '0';
					break;
				}
				case 7:
				{
					bin_file[i][14] = '0';
					bin_file[i][15] = '1';
					bin_file[i][16] = '1';
					bin_file[i][17] = '1';
					break;
				}
				case 8:
				{
					bin_file[i][14] = '1';
					bin_file[i][15] = '0';
					bin_file[i][16] = '0';
					bin_file[i][17] = '0';
					break;
				}
				case 9:
				{
					bin_file[i][14] = '1';
					bin_file[i][15] = '0';
					bin_file[i][16] = '0';
					bin_file[i][17] = '1';
					break;
				}
				case 10:
				{
					bin_file[i][14] = '1';
					bin_file[i][15] = '0';
					bin_file[i][16] = '1';
					bin_file[i][17] = '0';
					break;
				}
				case 11:
				{
					bin_file[i][14] = '1';
					bin_file[i][15] = '0';
					bin_file[i][16] = '1';
					bin_file[i][17] = '1';
					break;
				}
				case 12:
				{
					bin_file[i][14] = '1';
					bin_file[i][15] = '1';
					bin_file[i][16] = '0';
					bin_file[i][17] = '0';
					break;
				}
				case 13:
				{
					bin_file[i][14] = '1';
					bin_file[i][15] = '1';
					bin_file[i][16] = '0';
					bin_file[i][17] = '1';
					break;
				}
				case 14:
				{
					bin_file[i][14] = '1';
					bin_file[i][15] = '1';
					bin_file[i][16] = '1';
					bin_file[i][17] = '0';
					break;
				}
				case 15:
				{
					bin_file[i][14] = '1';
					bin_file[i][15] = '1';
					bin_file[i][16] = '1';
					bin_file[i][17] = '1';
					break;
				}
				default:
				{
					printf("number of register %d RD is wrong in line %d\n",reg,i);
				}
			}
			character +=3;
			for(;code[i][character] == ' ' || code[i][character] == '\t';character++);
			if(code[i][character] != ',')
			{
				printf("syntax of registers is wrong in line %d",i);
			}
			character++;
			for(;code[i][character] == ' ' || code[i][character] == '\t';character++);
			if(code[i][character] != 'r' && code[i][character] != 'R')
			{
				printf("name of RT in line %d is wrong\n",i);
			}
			reg = (code[i][character+1]*10) + code[i][character+2] - 480 - 48;
			switch(reg)
			{
				case 0:
				{
					bin_file[i][10] = '0';
					bin_file[i][11] = '0';
					bin_file[i][12] = '0';
					bin_file[i][13] = '0';
					break;
				}
				case 1:
				{
					bin_file[i][10] = '0';
					bin_file[i][11] = '0';
					bin_file[i][12] = '0';
					bin_file[i][13] = '1';
					break;
				}
				case 2:
				{
					bin_file[i][10] = '0';
					bin_file[i][11] = '0';
					bin_file[i][12] = '1';
					bin_file[i][13] = '0';
					break;
				}
				case 3:
				{
					bin_file[i][10] = '0';
					bin_file[i][11] = '0';
					bin_file[i][12] = '1';
					bin_file[i][13] = '1';
					break;
				}
				case 4:
				{
					bin_file[i][10] = '0';
					bin_file[i][11] = '1';
					bin_file[i][12] = '0';
					bin_file[i][13] = '0';
					break;
				}
				case 5:
				{
					bin_file[i][10] = '0';
					bin_file[i][11] = '1';
					bin_file[i][12] = '0';
					bin_file[i][13] = '1';
					break;
				}
				case 6:
				{
					bin_file[i][10] = '0';
					bin_file[i][11] = '1';
					bin_file[i][12] = '1';
					bin_file[i][13] = '0';
					break;
				}
				case 7:
				{
					bin_file[i][10] = '0';
					bin_file[i][11] = '1';
					bin_file[i][12] = '1';
					bin_file[i][13] = '1';
					break;
				}
				case 8:
				{
					bin_file[i][10] = '1';
					bin_file[i][11] = '0';
					bin_file[i][12] = '0';
					bin_file[i][13] = '0';
					break;
				}
				case 9:
				{
					bin_file[i][10] = '1';
					bin_file[i][11] = '0';
					bin_file[i][12] = '0';
					bin_file[i][13] = '1';
					break;
				}
				case 10:
				{
					bin_file[i][10] = '1';
					bin_file[i][11] = '0';
					bin_file[i][12] = '1';
					bin_file[i][13] = '0';
					break;
				}
				case 11:
				{
					bin_file[i][10] = '1';
					bin_file[i][11] = '0';
					bin_file[i][12] = '1';
					bin_file[i][13] = '1';
					break;
				}
				case 12:
				{
					bin_file[i][10] = '1';
					bin_file[i][11] = '1';
					bin_file[i][12] = '0';
					bin_file[i][13] = '0';
					break;
				}
				case 13:
				{
					bin_file[i][10] = '1';
					bin_file[i][11] = '1';
					bin_file[i][12] = '0';
					bin_file[i][13] = '1';
					break;
				}
				case 14:
				{
					bin_file[i][10] = '1';
					bin_file[i][11] = '1';
					bin_file[i][12] = '1';
					bin_file[i][13] = '0';
					break;
				}
				case 15:
				{
					bin_file[i][10] = '1';
					bin_file[i][11] = '1';
					bin_file[i][12] = '1';
					bin_file[i][13] = '1';
					break;
				}
				default:
				{
					printf("number of register RT is wrong in line %d\n",i);
				}
			}
			character +=3;
			for(;code[i][character] == ' ' || code[i][character] == '\t';character++);
			if(code[i][character] != ',')
			{
				printf("syntax of registers is wrong in line %d\n",i);
			}
			character++;
			for(;code[i][character] == ' ' || code[i][character] == '\t';character++);
			if(code[i][character] != 'r' && code[i][character] != 'R')
			{
				printf("name of RS in line %d is wrong\n",i);
			}
			reg = (code[i][character+1]*10) + code[i][character+2] - 480 - 48; 
			switch(reg)
			{
				case 0:
				{
					bin_file[i][6] = '0';
					bin_file[i][7] = '0';
					bin_file[i][8] = '0';
					bin_file[i][9] = '0';
					break;
				}
				case 1:
				{
					bin_file[i][6] = '0';
					bin_file[i][7] = '0';
					bin_file[i][8] = '0';
					bin_file[i][9] = '1';
					break;
				}
				case 2:
				{
					bin_file[i][6] = '0';
					bin_file[i][7] = '0';
					bin_file[i][8] = '1';
					bin_file[i][9] = '0';
					break;
				}
				case 3:
				{
					bin_file[i][6] = '0';
					bin_file[i][7] = '0';
					bin_file[i][8] = '1';
					bin_file[i][9] = '1';
					break;
				}
				case 4:
				{
					bin_file[i][6] = '0';
					bin_file[i][7] = '1';
					bin_file[i][8] = '0';
					bin_file[i][9] = '0';
					break;
				}
				case 5:
				{
					bin_file[i][6] = '0';
					bin_file[i][7] = '1';
					bin_file[i][8] = '0';
					bin_file[i][9] = '1';
					break;
				}
				case 6:
				{
					bin_file[i][6] = '0';
					bin_file[i][7] = '1';
					bin_file[i][8] = '1';
					bin_file[i][9] = '0';
					break;
				}
				case 7:
				{
					bin_file[i][6] = '0';
					bin_file[i][7] = '1';
					bin_file[i][8] = '1';
					bin_file[i][9] = '1';
					break;
				}
				case 8:
				{
					bin_file[i][6] = '1';
					bin_file[i][7] = '0';
					bin_file[i][8] = '0';
					bin_file[i][9] = '0';
					break;
				}
				case 9:
				{
					bin_file[i][6] = '1';
					bin_file[i][7] = '0';
					bin_file[i][8] = '0';
					bin_file[i][9] = '1';
					break;
				}
				case 10:
				{
					bin_file[i][6] = '1';
					bin_file[i][7] = '0';
					bin_file[i][8] = '1';
					bin_file[i][9] = '0';
					break;
				}
				case 11:
				{
					bin_file[i][6] = '1';
					bin_file[i][7] = '0';
					bin_file[i][8] = '1';
					bin_file[i][9] = '1';
					break;
				}
				case 12:
				{
					bin_file[i][6] = '1';
					bin_file[i][7] = '1';
					bin_file[i][8] = '0';
					bin_file[i][9] = '0';
					break;
				}
				case 13:
				{
					bin_file[i][6] = '1';
					bin_file[i][7] = '1';
					bin_file[i][8] = '0';
					bin_file[i][9] = '1';
					break;
				}
				case 14:
				{
					bin_file[i][6] = '1';
					bin_file[i][7] = '1';
					bin_file[i][8] = '1';
					bin_file[i][9] = '0';
					break;
				}
				case 15:
				{
					bin_file[i][6] = '1';
					bin_file[i][7] = '1';
					bin_file[i][8] = '1';
					bin_file[i][9] = '1';
					break;
				}
				default:
				{
					printf("number of register RS is wrong in line %d\n",i);
				}
			}
		}
		else if(x == 1)//2 register , 1 imm
		{
			for(;code[i][character] == ' ' || code[i][character] == '\t';character++);
			if(code[i][character] != 'r' && code[i][character] != 'R')
			{
				printf("name of RT in line %d is wrong\n",i);
			}
			reg = (code[i][character+1]*10) + code[i][character+2] - 480 - 48;
			switch(reg)
			{
				case 0:
				{
					bin_file[i][6] = '0';
					bin_file[i][7] = '0';
					bin_file[i][8] = '0';
					bin_file[i][9] = '0';
					break;
				}
				case 1:
				{
					bin_file[i][6] = '0';
					bin_file[i][7] = '0';
					bin_file[i][8] = '0';
					bin_file[i][9] = '1';
					break;
				}
				case 2:
				{
					bin_file[i][6] = '0';
					bin_file[i][7] = '0';
					bin_file[i][7] = '1';
					bin_file[i][8] = '0';
					break;
				}
				case 3:
				{
					bin_file[i][6] = '0';
					bin_file[i][7] = '0';
					bin_file[i][8] = '1';
					bin_file[i][9] = '1';
					break;
				}
				case 4:
				{
					bin_file[i][6] = '0';
					bin_file[i][7] = '1';
					bin_file[i][8] = '0';
					bin_file[i][9] = '0';
					break;
				}
				case 5:
				{
					bin_file[i][6] = '0';
					bin_file[i][7] = '1';
					bin_file[i][8] = '0';
					bin_file[i][9] = '1';
					break;
				}
				case 6:
				{
					bin_file[i][6] = '0';
					bin_file[i][7] = '1';
					bin_file[i][8] = '1';
					bin_file[i][9] = '0';
					break;
				}
				case 7:
				{
					bin_file[i][6] = '0';
					bin_file[i][7] = '1';
					bin_file[i][8] = '1';
					bin_file[i][9] = '1';
					break;
				}
				case 8:
				{
					bin_file[i][6] = '1';
					bin_file[i][7] = '0';
					bin_file[i][8] = '0';
					bin_file[i][9] = '0';
					break;
				}
				case 9:
				{
					bin_file[i][6] = '1';
					bin_file[i][7] = '0';
					bin_file[i][8] = '0';
					bin_file[i][9] = '1';
					break;
				}
				case 10:
				{
					bin_file[i][6] = '1';
					bin_file[i][7] = '0';
					bin_file[i][8] = '1';
					bin_file[i][9] = '0';
					break;
				}
				case 11:
				{
					bin_file[i][6] = '1';
					bin_file[i][7] = '0';
					bin_file[i][8] = '1';
					bin_file[i][9] = '1';
					break;
				}
				case 12:
				{
					bin_file[i][6] = '1';
					bin_file[i][7] = '1';
					bin_file[i][8] = '0';
					bin_file[i][9] = '0';
					break;
				}
				case 13:
				{
					bin_file[i][6] = '1';
					bin_file[i][7] = '1';
					bin_file[i][8] = '0';
					bin_file[i][9] = '1';
					break;
				}
				case 14:
				{
					bin_file[i][6] = '1';
					bin_file[i][7] = '1';
					bin_file[i][8] = '1';
					bin_file[i][9] = '0';
					break;
				}
				case 15:
				{
					bin_file[i][6] = '1';
					bin_file[i][7] = '1';
					bin_file[i][8] = '1';
					bin_file[i][9] = '1';
					break;
				}
				default:
				{
					printf("number of RD register is wrong in line %d\n",i);
				}
			}
			character +=3;
			for(;code[i][character] == ' ' || code[i][character] == '\t';character++);
			if(code[i][character] != ',')
			{
				printf("syntax of registers is wrong in line %d\n",i);
			}
			character++;
			for(;code[i][character] == ' ' || code[i][character] == '\t';character++);
			if(code[i][character] != 'r' && code[i][character] != 'R')
			{
				printf("name of RS in line %d is wrong\n",i);
			}
			reg = (code[i][character+1]*10) + code[i][character+2] - 480 - 48;
			switch(reg)
			{
				case 0:
				{
					bin_file[i][10] = '0';
					bin_file[i][11] = '0';
					bin_file[i][12] = '0';
					bin_file[i][13] = '0';
					break;
				}
				case 1:
				{
					bin_file[i][10] = '0';
					bin_file[i][11] = '0';
					bin_file[i][12] = '0';
					bin_file[i][13] = '1';
					break;
				}
				case 2:
				{
					bin_file[i][10] = '0';
					bin_file[i][11] = '0';
					bin_file[i][12] = '1';
					bin_file[i][13] = '0';
					break;
				}
				case 3:
				{
					bin_file[i][10] = '0';
					bin_file[i][11] = '0';
					bin_file[i][12] = '1';
					bin_file[i][13] = '1';
					break;
				}
				case 4:
				{
					bin_file[i][10] = '0';
					bin_file[i][11] = '1';
					bin_file[i][12] = '0';
					bin_file[i][13] = '0';
					break;
				}
				case 5:
				{
					bin_file[i][10] = '0';
					bin_file[i][11] = '1';
					bin_file[i][12] = '0';
					bin_file[i][13] = '1';
					break;
				}
				case 6:
				{
					bin_file[i][10] = '0';
					bin_file[i][11] = '1';
					bin_file[i][12] = '1';
					bin_file[i][13] = '0';
					break;
				}
				case 7:
				{
					bin_file[i][10] = '0';
					bin_file[i][11] = '1';
					bin_file[i][12] = '1';
					bin_file[i][13] = '1';
					break;
				}
				case 8:
				{
					bin_file[i][10] = '1';
					bin_file[i][11] = '0';
					bin_file[i][12] = '0';
					bin_file[i][13] = '0';
					break;
				}
				case 9:
				{
					bin_file[i][10] = '1';
					bin_file[i][11] = '0';
					bin_file[i][12] = '0';
					bin_file[i][13] = '1';
					break;
				}
				case 10:
				{
					bin_file[i][10] = '1';
					bin_file[i][11] = '0';
					bin_file[i][12] = '1';
					bin_file[i][13] = '0';
					break;
				}
				case 11:
				{
					bin_file[i][10] = '1';
					bin_file[i][11] = '0';
					bin_file[i][12] = '1';
					bin_file[i][13] = '1';
					break;
				}
				case 12:
				{
					bin_file[i][10] = '1';
					bin_file[i][11] = '1';
					bin_file[i][12] = '0';
					bin_file[i][13] = '0';
					break;
				}
				case 13:
				{
					bin_file[i][10] = '1';
					bin_file[i][11] = '1';
					bin_file[i][12] = '0';
					bin_file[i][13] = '1';
					break;
				}
				case 14:
				{
					bin_file[i][10] = '1';
					bin_file[i][11] = '1';
					bin_file[i][12] = '1';
					bin_file[i][13] = '0';
					break;
				}
				case 15:
				{
					bin_file[i][10] = '1';
					bin_file[i][11] = '1';
					bin_file[i][12] = '1';
					bin_file[i][13] = '1';
					break;
				}
				default:
				{
					printf("number of RS register is wrong in line %d\n",i);
				}
			}
			character +=3;
			for(;code[i][character] == ' ' || code[i][character] == '\t';character++);
			if(code[i][character] != ',')
			{
				printf("syntax of registers and immediate value is wrong in line %d",i);
			}
			character++;
			for(;code[i][character] == ' ' || code[i][character] == '\t';character++);
			if(code[i][character] != 'h' && code[i][character] != 'H')
			{
				printf("only accept hexadeciaml value");
			}
			character++;
			for(;character == '0';character++);
			j=0;
			for(;code[i][character] != ' '&& code[i][character] != '\t' && code[i][character] != '\n';character++)
			{
				number[j] = code[i][character];
				j++;
			}
			number[j] = 0;
			hextobin(&bin_file[i][14],number,18);
		}
		else if( x == 2) //2 registers only
		{
			for(;code[i][character] == ' ' || code[i][character] == '\t';character++);
			if(code[i][character] != 'r' && code[i][character] != 'R')
			{
				printf("name of RT in line %d is wrong\n",i);
			}
			reg = (code[i][character+1]*10) + code[i][character+2] - 480 - 48;
			switch(reg)
			{
				case 0:
				{
					bin_file[i][10] = '0';
					bin_file[i][11] = '0';
					bin_file[i][12] = '0';
					bin_file[i][13] = '0';
					break;
				}
				case 1:
				{
					bin_file[i][10] = '0';
					bin_file[i][11] = '0';
					bin_file[i][12] = '0';
					bin_file[i][13] = '1';
					break;
				}
				case 2:
				{
					bin_file[i][10] = '0';
					bin_file[i][11] = '0';
					bin_file[i][12] = '1';
					bin_file[i][13] = '0';
					break;
				}
				case 3:
				{
					bin_file[i][10] = '0';
					bin_file[i][11] = '0';
					bin_file[i][12] = '1';
					bin_file[i][13] = '1';
					break;
				}
				case 4:
				{
					bin_file[i][10] = '0';
					bin_file[i][11] = '1';
					bin_file[i][12] = '0';
					bin_file[i][13] = '0';
					break;
				}
				case 5:
				{
					bin_file[i][10] = '0';
					bin_file[i][11] = '1';
					bin_file[i][12] = '0';
					bin_file[i][13] = '1';
					break;
				}
				case 6:
				{
					bin_file[i][10] = '0';
					bin_file[i][11] = '1';
					bin_file[i][12] = '1';
					bin_file[i][13] = '0';
					break;
				}
				case 7:
				{
					bin_file[i][10] = '0';
					bin_file[i][11] = '1';
					bin_file[i][12] = '1';
					bin_file[i][13] = '1';
					break;
				}
				case 8:
				{
					bin_file[i][10] = '1';
					bin_file[i][11] = '0';
					bin_file[i][12] = '0';
					bin_file[i][13] = '0';
					break;
				}
				case 9:
				{
					bin_file[i][10] = '1';
					bin_file[i][11] = '0';
					bin_file[i][12] = '0';
					bin_file[i][13] = '1';
					break;
				}
				case 10:
				{
					bin_file[i][10] = '1';
					bin_file[i][11] = '0';
					bin_file[i][12] = '1';
					bin_file[i][13] = '0';
					break;
				}
				case 11:
				{
					bin_file[i][10] = '1';
					bin_file[i][11] = '0';
					bin_file[i][12] = '1';
					bin_file[i][13] = '1';
					break;
				}
				case 12:
				{
					bin_file[i][10] = '1';
					bin_file[i][11] = '1';
					bin_file[i][12] = '0';
					bin_file[i][13] = '0';
					break;
				}
				case 13:
				{
					bin_file[i][10] = '1';
					bin_file[i][11] = '1';
					bin_file[i][12] = '0';
					bin_file[i][13] = '1';
					break;
				}
				case 14:
				{
					bin_file[i][10] = '1';
					bin_file[i][11] = '1';
					bin_file[i][12] = '1';
					bin_file[i][13] = '0';
					break;
				}
				case 15:
				{
					bin_file[i][10] = '1';
					bin_file[i][11] = '1';
					bin_file[i][12] = '1';
					bin_file[i][13] = '1';
					break;
				}
				default:
				{
					printf("number of register is wrong in line %d\n",i);
				}
			}
			character +=3;
			for(;code[i][character] == ' ' || code[i][character] == '\t';character++);
			if(code[i][character] != ',')
			{
				printf("syntax of registers is wrong in line %d",i);
			}
			character++;
			for(;code[i][character] == ' ' || code[i][character] == '\t';character++);
			if(code[i][character] != 'r' && code[i][character] != 'R')
			{
				printf("name of RS in line %d is wrong\n",i);
			}
			reg = (code[i][character+1]*10) + code[i][character+2] - 480 - 48;
			switch(reg)
			{
				case 0:
				{
					bin_file[i][6] = '0';
					bin_file[i][7] = '0';
					bin_file[i][8] = '0';
					bin_file[i][9] = '0';
					break;
				}
				case 1:
				{
					bin_file[i][6] = '0';
					bin_file[i][7] = '0';
					bin_file[i][8] = '0';
					bin_file[i][9] = '1';
					break;
				}
				case 2:
				{
					bin_file[i][6] = '0';
					bin_file[i][7] = '0';
					bin_file[i][8] = '1';
					bin_file[i][9] = '0';
					break;
				}
				case 3:
				{
					bin_file[i][6] = '0';
					bin_file[i][7] = '0';
					bin_file[i][8] = '1';
					bin_file[i][9] = '1';
					break;
				}
				case 4:
				{
					bin_file[i][6] = '0';
					bin_file[i][7] = '1';
					bin_file[i][8] = '0';
					bin_file[i][9] = '0';
					break;
				}
				case 5:
				{
					bin_file[i][6] = '0';
					bin_file[i][7] = '1';
					bin_file[i][8] = '0';
					bin_file[i][9] = '1';
					break;
				}
				case 6:
				{
					bin_file[i][6] = '0';
					bin_file[i][7] = '1';
					bin_file[i][8] = '1';
					bin_file[i][9] = '0';
					break;
				}
				case 7:
				{
					bin_file[i][6] = '0';
					bin_file[i][7] = '1';
					bin_file[i][8] = '1';
					bin_file[i][9] = '1';
					break;
				}
				case 8:
				{
					bin_file[i][6] = '1';
					bin_file[i][7] = '0';
					bin_file[i][8] = '0';
					bin_file[i][9] = '0';
					break;
				}
				case 9:
				{
					bin_file[i][6] = '1';
					bin_file[i][7] = '0';
					bin_file[i][8] = '0';
					bin_file[i][9] = '1';
					break;
				}
				case 10:
				{
					bin_file[i][6] = '1';
					bin_file[i][7] = '0';
					bin_file[i][8] = '1';
					bin_file[i][9] = '0';
					break;
				}
				case 11:
				{
					bin_file[i][6] = '1';
					bin_file[i][7] = '0';
					bin_file[i][8] = '1';
					bin_file[i][9] = '1';
					break;
				}
				case 12:
				{
					bin_file[i][6] = '1';
					bin_file[i][7] = '1';
					bin_file[i][8] = '0';
					bin_file[i][9] = '0';
					break;
				}
				case 13:
				{
					bin_file[i][6] = '1';
					bin_file[i][7] = '1';
					bin_file[i][8] = '0';
					bin_file[i][9] = '1';
					break;
				}
				case 14:
				{
					bin_file[i][6] = '1';
					bin_file[i][7] = '1';
					bin_file[i][8] = '1';
					bin_file[i][9] = '0';
					break;
				}
				case 15:
				{
					bin_file[i][6] = '1';
					bin_file[i][7] = '1';
					bin_file[i][8] = '1';
					bin_file[i][9] = '1';
					break;
				}
				default:
				{
					printf("number of register is wrong in line %d\n",i);
				}
			}
		}
		else if(x == 3)// out
		{
			for(;code[i][character] == ' ' || code[i][character] == '\t';character++);
			reg = (code[i][character+1]*10) + code[i][character+2] - 480 - 48;
			switch(reg)
			{
				case 0:
				{
					bin_file[i][6] = '0';
					bin_file[i][7] = '0';
					bin_file[i][8] = '0';
					bin_file[i][9] = '0';
					break;
				}
				case 1:
				{
					bin_file[i][6] = '0';
					bin_file[i][7] = '0';
					bin_file[i][8] = '0';
					bin_file[i][9] = '1';
					break;
				}
				case 2:
				{
					bin_file[i][6] = '0';
					bin_file[i][7] = '0';
					bin_file[i][8] = '1';
					bin_file[i][9] = '0';
					break;
				}
				case 3:
				{
					bin_file[i][6] = '0';
					bin_file[i][7] = '0';
					bin_file[i][8] = '1';
					bin_file[i][9] = '1';
					break;
				}
				case 4:
				{
					bin_file[i][6] = '0';
					bin_file[i][7] = '1';
					bin_file[i][8] = '0';
					bin_file[i][9] = '0';
					break;
				}
				case 5:
				{
					bin_file[i][6] = '0';
					bin_file[i][7] = '1';
					bin_file[i][8] = '0';
					bin_file[i][9] = '1';
					break;
				}
				case 6:
				{
					bin_file[i][6] = '0';
					bin_file[i][7] = '1';
					bin_file[i][8] = '1';
					bin_file[i][9] = '0';
					break;
				}
				case 7:
				{
					bin_file[i][6] = '0';
					bin_file[i][7] = '1';
					bin_file[i][8] = '1';
					bin_file[i][9] = '1';
					break;
				}
				case 8:
				{
					bin_file[i][6] = '1';
					bin_file[i][7] = '0';
					bin_file[i][8] = '0';
					bin_file[i][9] = '0';
					break;
				}
				case 9:
				{
					bin_file[i][6] = '1';
					bin_file[i][7] = '0';
					bin_file[i][8] = '0';
					bin_file[i][9] = '1';
					break;
				}
				case 10:
				{
					bin_file[i][6] = '1';
					bin_file[i][7] = '0';
					bin_file[i][8] = '1';
					bin_file[i][9] = '0';
					break;
				}
				case 11:
				{
					bin_file[i][6] = '1';
					bin_file[i][7] = '0';
					bin_file[i][8] = '1';
					bin_file[i][9] = '1';
					break;
				}
				case 12:
				{
					bin_file[i][6] = '1';
					bin_file[i][7] = '1';
					bin_file[i][8] = '0';
					bin_file[i][9] = '0';
					break;
				}
				case 13:
				{
					bin_file[i][6] = '1';
					bin_file[i][7] = '1';
					bin_file[i][8] = '0';
					bin_file[i][9] = '1';
					break;
				}
				case 14:
				{
					bin_file[i][6] = '1';
					bin_file[i][7] = '1';
					bin_file[i][8] = '1';
					bin_file[i][9] = '0';
					break;
				}
				case 15:
				{
					bin_file[i][6] = '1';
					bin_file[i][7] = '1';
					bin_file[i][8] = '1';
					bin_file[i][9] = '1';
					break;
				}
				default:
				{
					printf("number of register %d RD is wrong in line %d\n",reg,i);
				}
			}
		}
		else if(x == 4)// in
		{
			for(;code[i][character] == ' ' || code[i][character] == '\t';character++);
			reg = (code[i][character+1]*10) + code[i][character+2] - 480 - 48;
			switch(reg)
			{
				case 0:
				{
					bin_file[i][14] = '0';
					bin_file[i][15] = '0';
					bin_file[i][16] = '0';
					bin_file[i][17] = '0';
					break;
				}
				case 1:
				{
					bin_file[i][14] = '0';
					bin_file[i][15] = '0';
					bin_file[i][16] = '0';
					bin_file[i][17] = '1';
					break;
				}
				case 2:
				{
					bin_file[i][14] = '0';
					bin_file[i][15] = '0';
					bin_file[i][16] = '1';
					bin_file[i][17] = '0';
					break;
				}
				case 3:
				{
					bin_file[i][14] = '0';
					bin_file[i][15] = '0';
					bin_file[i][16] = '1';
					bin_file[i][17] = '1';
					break;
				}
				case 4:
				{
					bin_file[i][14] = '0';
					bin_file[i][15] = '1';
					bin_file[i][16] = '0';
					bin_file[i][17] = '0';
					break;
				}
				case 5:
				{
					bin_file[i][14] = '0';
					bin_file[i][15] = '1';
					bin_file[i][16] = '0';
					bin_file[i][17] = '1';
					break;
				}
				case 6:
				{
					bin_file[i][14] = '0';
					bin_file[i][15] = '1';
					bin_file[i][16] = '1';
					bin_file[i][17] = '0';
					break;
				}
				case 7:
				{
					bin_file[i][14] = '0';
					bin_file[i][15] = '1';
					bin_file[i][16] = '1';
					bin_file[i][17] = '1';
					break;
				}
				case 8:
				{
					bin_file[i][14] = '1';
					bin_file[i][15] = '0';
					bin_file[i][16] = '0';
					bin_file[i][17] = '0';
					break;
				}
				case 9:
				{
					bin_file[i][14] = '1';
					bin_file[i][15] = '0';
					bin_file[i][16] = '0';
					bin_file[i][17] = '1';
					break;
				}
				case 10:
				{
					bin_file[i][14] = '1';
					bin_file[i][15] = '0';
					bin_file[i][16] = '1';
					bin_file[i][17] = '0';
					break;
				}
				case 11:
				{
					bin_file[i][14] = '1';
					bin_file[i][15] = '0';
					bin_file[i][16] = '1';
					bin_file[i][17] = '1';
					break;
				}
				case 12:
				{
					bin_file[i][14] = '1';
					bin_file[i][15] = '1';
					bin_file[i][16] = '0';
					bin_file[i][17] = '0';
					break;
				}
				case 13:
				{
					bin_file[i][14] = '1';
					bin_file[i][15] = '1';
					bin_file[i][16] = '0';
					bin_file[i][17] = '1';
					break;
				}
				case 14:
				{
					bin_file[i][14] = '1';
					bin_file[i][15] = '1';
					bin_file[i][16] = '1';
					bin_file[i][17] = '0';
					break;
				}
				case 15:
				{
					bin_file[i][14] = '1';
					bin_file[i][15] = '1';
					bin_file[i][16] = '1';
					bin_file[i][17] = '1';
					break;
				}
				default:
				{
					printf("number of register %d RD is wrong in line %d\n",reg,i);
				}
			}
		}
		else if(x == 5)//pop and push
		{
			for(;code[i][character] == ' ' || code[i][character] == '\t';character++);
			if(code[i][character] != 'r' && code[i][character] != 'R')
			{
				printf("name of register in line %d is wrong\n",i);
			}
			reg = (code[i][character+1]*10) + code[i][character+2] - 480 - 48;
			switch(reg)
			{
				case 0:
				{
					bin_file[i][10] = '0';
					bin_file[i][11] = '0';
					bin_file[i][12] = '0';
					bin_file[i][13] = '0';
					break;
				}
				case 1:
				{
					bin_file[i][10] = '0';
					bin_file[i][11] = '0';
					bin_file[i][12] = '0';
					bin_file[i][13] = '1';
					break;
				}
				case 2:
				{
					bin_file[i][10] = '0';
					bin_file[i][11] = '0';
					bin_file[i][12] = '1';
					bin_file[i][13] = '0';
					break;
				}
				case 3:
				{
					bin_file[i][10] = '0';
					bin_file[i][11] = '0';
					bin_file[i][12] = '1';
					bin_file[i][13] = '1';
					break;
				}
				case 4:
				{
					bin_file[i][10] = '0';
					bin_file[i][11] = '1';
					bin_file[i][12] = '0';
					bin_file[i][13] = '0';
					break;
				}
				case 5:
				{
					bin_file[i][10] = '0';
					bin_file[i][11] = '1';
					bin_file[i][12] = '0';
					bin_file[i][13] = '1';
					break;
				}
				case 6:
				{
					bin_file[i][10] = '0';
					bin_file[i][11] = '1';
					bin_file[i][12] = '1';
					bin_file[i][13] = '0';
					break;
				}
				case 7:
				{
					bin_file[i][10] = '0';
					bin_file[i][11] = '1';
					bin_file[i][12] = '1';
					bin_file[i][13] = '1';
					break;
				}
				case 8:
				{
					bin_file[i][10] = '1';
					bin_file[i][11] = '0';
					bin_file[i][12] = '0';
					bin_file[i][13] = '0';
					break;
				}
				case 9:
				{
					bin_file[i][10] = '1';
					bin_file[i][11] = '0';
					bin_file[i][12] = '0';
					bin_file[i][13] = '1';
					break;
				}
				case 10:
				{
					bin_file[i][10] = '1';
					bin_file[i][11] = '0';
					bin_file[i][12] = '1';
					bin_file[i][13] = '0';
					break;
				}
				case 11:
				{
					bin_file[i][10] = '1';
					bin_file[i][11] = '0';
					bin_file[i][12] = '1';
					bin_file[i][13] = '1';
					break;
				}
				case 12:
				{
					bin_file[i][10] = '1';
					bin_file[i][11] = '1';
					bin_file[i][12] = '0';
					bin_file[i][13] = '0';
					break;
				}
				case 13:
				{
					bin_file[i][10] = '1';
					bin_file[i][11] = '1';
					bin_file[i][12] = '0';
					bin_file[i][13] = '1';
					break;
				}
				case 14:
				{
					bin_file[i][10] = '1';
					bin_file[i][11] = '1';
					bin_file[i][12] = '1';
					bin_file[i][13] = '0';
					break;
				}
				case 15:
				{
					bin_file[i][10] = '1';
					bin_file[i][11] = '1';
					bin_file[i][12] = '1';
					bin_file[i][13] = '1';
					break;
				}
				default:
				{
					printf("number of register RT is wrong in line %d\n",i);
				}
			}
		}
		else if(x == 6)// jump and call
		{
			for(;code[i][character]== ' ' || code[i][character] == '\t';character++);
			counter = 0;
			for (;code[i][character] != ' ' && code[i][character] != '\n' && code[i][character] != '\t';character++)
			{
				label_name[counter] = code[i][character];
				counter++; 
			}
			label_name[counter] = 0;
			dectobin(&bin_file[i][6],fetch_label(label_name,tree),26);
		}
		else if(x == 8)//branches
		{
			for(;code[i][character] == ' ' || code[i][character] == '\t';character++);
			if(code[i][character] != 'r' && code[i][character] != 'R')
			{
				printf("name of register in line %d is wrong\n",i);
			}
			reg = (code[i][character+1]*10) + code[i][character+2] - 480 - 48;
			switch(reg)
			{
				case 0:
				{
					bin_file[i][6] = '0';
					bin_file[i][7] = '0';
					bin_file[i][8] = '0';
					bin_file[i][9] = '0';
					break;
				}
				case 1:
				{
					bin_file[i][6] = '0';
					bin_file[i][7] = '0';
					bin_file[i][8] = '0';
					bin_file[i][9] = '1';
					break;
				}
				case 2:
				{
					bin_file[i][6] = '0';
					bin_file[i][7] = '0';
					bin_file[i][8] = '1';
					bin_file[i][9] = '0';
					break;
				}
				case 3:
				{
					bin_file[i][6] = '0';
					bin_file[i][7] = '0';
					bin_file[i][8] = '1';
					bin_file[i][9] = '1';
					break;
				}
				case 4:
				{
					bin_file[i][6] = '0';
					bin_file[i][7] = '1';
					bin_file[i][8] = '0';
					bin_file[i][9] = '0';
					break;
				}
				case 5:
				{
					bin_file[i][6] = '0';
					bin_file[i][7] = '1';
					bin_file[i][8] = '0';
					bin_file[i][9] = '1';
					break;
				}
				case 6:
				{
					bin_file[i][6] = '0';
					bin_file[i][7] = '1';
					bin_file[i][8] = '1';
					bin_file[i][9] = '0';
					break;
				}
				case 7:
				{
					bin_file[i][6] = '0';
					bin_file[i][7] = '1';
					bin_file[i][8] = '1';
					bin_file[i][9] = '1';
					break;
				}
				case 8:
				{
					bin_file[i][6] = '1';
					bin_file[i][7] = '0';
					bin_file[i][8] = '0';
					bin_file[i][9] = '0';
					break;
				}
				case 9:
				{
					bin_file[i][6] = '1';
					bin_file[i][7] = '0';
					bin_file[i][8] = '0';
					bin_file[i][9] = '1';
					break;
				}
				case 10:
				{
					bin_file[i][6] = '1';
					bin_file[i][7] = '0';
					bin_file[i][8] = '1';
					bin_file[i][9] = '0';
					break;
				}
				case 11:
				{
					bin_file[i][6] = '1';
					bin_file[i][7] = '0';
					bin_file[i][8] = '1';
					bin_file[i][9] = '1';
					break;
				}
				case 12:
				{
					bin_file[i][6] = '1';
					bin_file[i][7] = '1';
					bin_file[i][8] = '0';
					bin_file[i][9] = '0';
					break;
				}
				case 13:
				{
					bin_file[i][6] = '1';
					bin_file[i][7] = '1';
					bin_file[i][8] = '0';
					bin_file[i][9] = '1';
					break;
				}
				case 14:
				{
					bin_file[i][6] = '1';
					bin_file[i][7] = '1';
					bin_file[i][8] = '1';
					bin_file[i][9] = '0';
					break;
				}
				case 15:
				{
					bin_file[i][6] = '1';
					bin_file[i][7] = '1';
					bin_file[i][8] = '1';
					bin_file[i][9] = '1';
					break;
				}
				default:
				{
					printf("number of register RS is wrong in line %d\n",i);
				}
			}
			character +=3;
			for(;code[i][character] == ' ' || code[i][character] == '\t';character++);
			if(code[i][character] != ',')
			{
				printf("syntax of registers is wrong in line %d",i);
			}
			character++;
			for(;code[i][character] == ' ' || code[i][character] == '\t';character++);
			if(code[i][character] != 'r' && code[i][character] != 'R')
			{
				printf("name of RT in line %d is wrong\n",i);
			}
			reg = (code[i][character+1]*10) + code[i][character+2] - 480 - 48;
			switch(reg)
			{
				case 0:
				{
					bin_file[i][10] = '0';
					bin_file[i][11] = '0';
					bin_file[i][12] = '0';
					bin_file[i][13] = '0';
					break;
				}
				case 1:
				{
					bin_file[i][10] = '0';
					bin_file[i][11] = '0';
					bin_file[i][12] = '0';
					bin_file[i][13] = '1';
					break;
				}
				case 2:
				{
					bin_file[i][10] = '0';
					bin_file[i][11] = '0';
					bin_file[i][12] = '1';
					bin_file[i][13] = '0';
					break;
				}
				case 3:
				{
					bin_file[i][10] = '0';
					bin_file[i][11] = '0';
					bin_file[i][12] = '1';
					bin_file[i][13] = '1';
					break;
				}
				case 4:
				{
					bin_file[i][10] = '0';
					bin_file[i][11] = '1';
					bin_file[i][12] = '0';
					bin_file[i][13] = '0';
					break;
				}
				case 5:
				{
					bin_file[i][10] = '0';
					bin_file[i][11] = '1';
					bin_file[i][12] = '0';
					bin_file[i][13] = '1';
					break;
				}
				case 6:
				{
					bin_file[i][10] = '0';
					bin_file[i][11] = '1';
					bin_file[i][12] = '1';
					bin_file[i][13] = '0';
					break;
				}
				case 7:
				{
					bin_file[i][10] = '0';
					bin_file[i][11] = '1';
					bin_file[i][12] = '1';
					bin_file[i][13] = '1';
					break;
				}
				case 8:
				{
					bin_file[i][10] = '1';
					bin_file[i][11] = '0';
					bin_file[i][12] = '0';
					bin_file[i][13] = '0';
					break;
				}
				case 9:
				{
					bin_file[i][10] = '1';
					bin_file[i][11] = '0';
					bin_file[i][12] = '0';
					bin_file[i][13] = '1';
					break;
				}
				case 10:
				{
					bin_file[i][10] = '1';
					bin_file[i][11] = '0';
					bin_file[i][12] = '1';
					bin_file[i][13] = '0';
					break;
				}
				case 11:
				{
					bin_file[i][10] = '1';
					bin_file[i][11] = '0';
					bin_file[i][12] = '1';
					bin_file[i][13] = '1';
					break;
				}
				case 12:
				{
					bin_file[i][10] = '1';
					bin_file[i][11] = '1';
					bin_file[i][12] = '0';
					bin_file[i][13] = '0';
					break;
				}
				case 13:
				{
					bin_file[i][10] = '1';
					bin_file[i][11] = '1';
					bin_file[i][12] = '0';
					bin_file[i][13] = '1';
					break;
				}
				case 14:
				{
					bin_file[i][10] = '1';
					bin_file[i][11] = '1';
					bin_file[i][12] = '1';
					bin_file[i][13] = '0';
					break;
				}
				case 15:
				{
					bin_file[i][10] = '1';
					bin_file[i][11] = '1';
					bin_file[i][12] = '1';
					bin_file[i][13] = '1';
					break;
				}
				default:
				{
					printf("number of register RT is wrong in line %d\n",i);
				}
			}
			character +=3;
			for(;code[i][character] == ' ' || code[i][character] == '\t';character++);
			if(code[i][character] != ',')
			{
				printf("syntax of registers is wrong in line %d",i);
			}
			character++;
			for(;code[i][character]== ' ' || code[i][character] == '\t';character++);
			counter = 0;
			for (;code[i][character] != ' ' && code[i][character] != '\n' && code[i][character] != '\t';character++)
			{
				label_name[counter] = code[i][character];
				counter++; 
			}
			label_name[counter] = 0;
			dectobin(&bin_file[i][6],fetch_label(label_name,tree)-i,26);
		}
		for(j=0 ; j<32;j++)
		{
			putchar(bin_file[i][j]);
		}
		putchar('\n');
	}
}

int fetch_label(char name[30],treeNodePtr tree)
{
	int i=0;
	int flag = 0;
	if(tree)
	{
		for(;i < 30;i++)
		{
			if(name[i] != tree->label[i])
			{
				flag = 1;
			}
			if(name[i] == 0)
			{
				break;
			}
		}
		if(flag)
		{
			fetch_label(name,tree->next_ptr);
		}
		else
		{
			return tree->line;
		}
	}
	else
	{
		printf("label: %s not found",name);
		return 0;
	}
}

int fetch_op(char bin_file[][32],char instruction[],int line)
{
	int i;
	switch(instruction[0])
	{
		case 'a':
		case 'A':
		{
			//printf("found a ");
			switch(instruction[1])
			{
				case 'd':
				case 'D':
				{
					//printf("found d ");
					switch(instruction[2])
					{
						case 'd':
						case 'D':
						{
							//printf("found d ");
							switch(instruction[3])
							{
								case 0:
								{
									bin_file[line][0] = '0';
									bin_file[line][1] = '0';
									bin_file[line][2] = '0';
									bin_file[line][3] = '0';
									bin_file[line][4] = '0';
									bin_file[line][5] = '0';
									bin_file[line][18] = '0';
									bin_file[line][19] = '0';
									bin_file[line][20] = '0';
									bin_file[line][21] = '0';
									for(i=22; i< 32;i++)
									{
										bin_file[line][i] = '0';
									}
									//printf("found add\n");
									return 0; // zero for 3 registers
									//found add
									break;
								}
								case 'i':
								case 'I':
								{
									switch(instruction[4])
									{
										case 0:
										{
											bin_file[line][0] = '1';
											bin_file[line][1] = '0';
											bin_file[line][2] = '0';
											bin_file[line][3] = '0';
											bin_file[line][4] = '0';
											bin_file[line][5] = '0';
											return 1;// one for 2 registers and imm
											//printf("found addi\n");
											//found addi
											break;
										}
										default:
										{
											printf("did you u mean addi in line %d\n",line);
										}
									}
									break;
								}
								default:
								{
									printf("you were either trying to write and or addi in line %d\n",line);
								}
							}
							break;
						}
						default:
						{
							printf("you were either trying to write and or addi in line %d\n",line);
						}
					}
					break;
				}
				case 'n':
				case 'N':
				{
					switch(instruction[2])
					{
						case 'd':
						case 'D':
						{
							switch(instruction[3])
							{
								case 0:
								{
									bin_file[line][0] = '0';
									bin_file[line][1] = '0';
									bin_file[line][2] = '0';
									bin_file[line][3] = '0';
									bin_file[line][4] = '0';
									bin_file[line][5] = '0';
									bin_file[line][18] = '0';
									bin_file[line][19] = '1';
									bin_file[line][20] = '0';
									bin_file[line][21] = '0';
									for(i=22; i< 32;i++)
									{
										bin_file[line][i] = '0';
									}
									return 0;//zero for 3 registers
								    //printf("found and\n");
									break;
									//found and
								}
								case 'i':
								case 'I':
								{
									switch(instruction[4])
									{
										case 0:
										{
											bin_file[line][0] = '1';
											bin_file[line][1] = '0';
											bin_file[line][2] = '0';
											bin_file[line][3] = '1';
											bin_file[line][4] = '0';
											bin_file[line][5] = '0';
											return 1;
											//printf("found andi\n");
											break;
											//found andi
										}
										default:
										{
											printf("did you mean andi in line %d",line);
										}
									}
									break;
								}
								default:
								{
									printf("you were either trying to write and or andi %d\n",line);
								}
							}
							break;
						}
					}
					break;
				}
				default:
				{
					printf("you were either trying to write and or andi or add or addi %d\n",line);
				}
			}
			break;
		}
		case 's':
		case 'S':
		{
			switch(instruction[1])
			{
				case 'u':
				case 'U':
				{
					switch(instruction[2])
					{
						case 'b':
						case 'B':
						{
							switch(instruction[3])
							{
								case 0:
								{
									bin_file[line][0] = '0';
									bin_file[line][1] = '0';
									bin_file[line][2] = '0';
									bin_file[line][3] = '0';
									bin_file[line][4] = '0';
									bin_file[line][5] = '0';
									bin_file[line][18] = '0';
									bin_file[line][19] = '0';
									bin_file[line][20] = '0';
									bin_file[line][21] = '1';
									for(i=22; i< 32;i++)
									{
										bin_file[line][i] = '0';
									}
									//printf("found sub\n");
									return 0;
									break;//found sub
								}
								case 'i':
								case 'I':
								{
									switch(instruction[4])
									{
										case 0:
										{
											bin_file[line][0] = '1';
											bin_file[line][1] = '0';
											bin_file[line][2] = '0';
											bin_file[line][3] = '0';
											bin_file[line][4] = '0';
											bin_file[line][5] = '1';
											return 1;
											//printf("found subi\n");
											break;//found subi
										}
										default:
										{
											printf("did you mean subi in line %d",line);
										}
									}
									break;
								}
								default:
								{
									printf("did you mean sub or subi in line %d",line);	
								}
							}
							break;
						}
						printf("did you mean sub or subi in line %d",line);
					}
				}
				case 'h':
				case 'H':
				{
					switch( instruction[2] )
					{
						case 'l':
						case 'L':
						{
							switch( instruction[3] )
							{
								case 0:
								{
									bin_file[line][0] = '1';
									bin_file[line][1] = '0';
									bin_file[line][2] = '1';
									bin_file[line][3] = '0';
									bin_file[line][4] = '0';
									bin_file[line][5] = '0';
									for(i = 14;i < 32;i++)
									{
										bin_file[line][i] = '0';
									}
									return 2;//10 for 2 regs only
									break;
									//found shift left
								}
								default:
								{
									printf("did you mean sll in line %d",line);	
								}
							}
							break;
						}
						case 'r':
						case 'R':
						{
							switch( instruction[3] )
							{
								case 0:
								{
									bin_file[line][0] = '1';
									bin_file[line][1] = '0';
									bin_file[line][2] = '1';
									bin_file[line][3] = '0';
									bin_file[line][4] = '0';
									bin_file[line][5] = '1';
									for(i = 14;i < 32;i++)
									{
										bin_file[line][i] = '0';
									}
									return 2;
									break;
									// found shift right
								}
								default:
								{
									printf("did you mean slr in line %d",line);
								}
							}
							break;
						}
						default:
						{
							printf("did you mean sll or slr in line %d",line);
						}
					}
					break;
				}
				case 'a':
				case 'A':
				{
					switch( instruction[2] )
					{
						case 'r':
						case 'R':
						{
							switch( instruction[3] )
							{
								case 0:
								{
									bin_file[line][0] = '1';
									bin_file[line][1] = '0';
									bin_file[line][2] = '1';
									bin_file[line][3] = '0';
									bin_file[line][4] = '1';
									bin_file[line][5] = '0';
									for(i = 14;i < 32;i++)
									{
										bin_file[line][i] = '0';
									}
									return 2;
									break;
									//found shift right arithmetic
								}
								default:
								{
									printf("did you mean sar in line %d",line);
								}
							}
							break;
						}
						default:
						{
							printf("did you mean sar in line %d",line);
						}
					}
					break;
				}
				case 't':
				case 'T':
				{
					switch( instruction[2] )
					{
						case 'o':
						case 'O':
						{
							switch( instruction[3] )
							{
								case 'r':
								case 'R':
								{
									switch( instruction[4] )
									{
										case 'e':
										case 'E':
										{
											switch( instruction[5] )
											{
												case 0:
												{
													bin_file[line][0] = '1';
													bin_file[line][1] = '0';
													bin_file[line][2] = '1';
													bin_file[line][3] = '0';
													bin_file[line][4] = '1';
													bin_file[line][5] = '1';
													return 1;
													break;
													//found store
												}
												default:
												{
													printf("did you mean store in line %d",line);
												}
											}
											break;
										}
										default:
										{
											printf("did you mean store in line %d",line);
										}
									}
									break;
								}
								default:
								{
									printf("did you mean store in line %d",line);
								}
							}
							break;
						}
						default:
						{
							printf("did you mean store in line %d",line);
						}
					}
					break;
				}
				default:
				{
					printf("did you mean store or sar or sll or slr or sub or subi in line %d",line);
				}
			}
		}
		case 'm':
		case 'M':
		{
			switch( instruction[1])
			{
				case 'u':
				case 'U':
				{
					switch(instruction[2])
					{
						case 'l':
						case 'L':
						{
							switch( instruction[3])
							{
								case 'l':
								case 'L':
								{
									switch( instruction[4] )
									{
										case 0:
										{
											bin_file[line][0] = '0';
											bin_file[line][1] = '0';
											bin_file[line][2] = '0';
											bin_file[line][3] = '0';
											bin_file[line][4] = '0';
											bin_file[line][5] = '0';
											bin_file[line][18] = '0';
											bin_file[line][19] = '0';
											bin_file[line][20] = '1';
											bin_file[line][21] = '0';
											for(i=22; i< 32;i++)
											{
												bin_file[line][i] = '0';
											}
											return 0;
											//printf("found multiply low\n");
											break;
											//found multiply low
										}
										case 'i':
										case 'I':
										{
											switch(instruction[5])
											{
												case 0:
												{
													bin_file[line][0] = '1';
													bin_file[line][1] = '0';
													bin_file[line][2] = '0';
													bin_file[line][3] = '0';
													bin_file[line][4] = '1';
													bin_file[line][5] = '0';
													return 1;
													break;
													//found multiply low immediate
												}
												default:
												{
													printf("did you mean mulli in line %d",line);
												}
											}
											break;
										}
										default:
										{
											printf("did you mean mull or mulli in line %d",line);
										}
									}
									break;
								}
								case 'h':
								case 'H':
								{
									switch( instruction[4] )
									{
										case 0:
										{
											bin_file[line][0] = '0';
											bin_file[line][1] = '0';
											bin_file[line][2] = '0';
											bin_file[line][3] = '0';
											bin_file[line][4] = '0';
											bin_file[line][5] = '0';
											bin_file[line][18] = '0';
									        bin_file[line][19] = '0';
											bin_file[line][20] = '1';
											bin_file[line][21] = '1';
											for(i=22; i< 32;i++)
											{
												bin_file[line][i] = '0';
											}
											//printf("found multiply high\n");
											//found multiply high
											return 0;
											break;
										}
										case 'i':
										case 'I':
										{
											switch(instruction[5])
											{
												case 0:
												{
													bin_file[line][0] = '1';
													bin_file[line][1] = '0';
													bin_file[line][2] = '0';
													bin_file[line][3] = '0';
													bin_file[line][4] = '1';
													bin_file[line][5] = '1';
													return 1;
													break;
													//found multiply high immediate
												}
												default:
												{
													printf("did you mean mulhi in line %d",line);
												}
											}
										}
										default:
										{
											printf("did you mean mulh or mulhi in line %d",line);
										}
									} 
									break;
								}
								default:
								{
									printf("did you mean mull or mulli or mulh or mulhi in line %d",line);
								}
							}
							break;
						}
						default:
						{
							printf("did you mean mull or mulli or mulh or mulhi in line %d",line);
						}
					}
					break;
				}
				default:
				{
					printf("did you mean mull or mulli or mulh or mulhi in line %d",line);
				}
			}
			break;
		}
		case 'o':
		case 'O':
		{
			switch(instruction[1])
			{
				case 'r':
				case 'R':
				{
					switch(instruction[2])
					{
						case 0:
						{
							bin_file[line][0] = '0';
							bin_file[line][1] = '0';
							bin_file[line][2] = '0';
							bin_file[line][3] = '0';
							bin_file[line][4] = '0';
							bin_file[line][5] = '0';
							bin_file[line][18] = '0';
					        bin_file[line][19] = '1';
							bin_file[line][20] = '0';
							bin_file[line][21] = '1';
							for(i=22; i< 32;i++)
							{
								bin_file[line][i] = '0';
							}
							return 0;
							break;
							// found or
						}
						case 'i':
						case 'I':
						{
							switch(instruction[3])
							{
								case 0:
								{
									bin_file[line][0] = '1';
									bin_file[line][1] = '0';
									bin_file[line][2] = '0';
									bin_file[line][3] = '1';
									bin_file[line][4] = '0';
									bin_file[line][5] = '1';
									return 1;
									break;
									// found ori
								}
								default:
								{
									printf("did you mean ori in line %d",line);
								}
							}
						}
						default:
						{
							printf("did you mean OR or ORI in line %d",line);
						}
					}
					break;
				}
				case 'u':
				case 'U':
				{
					switch( instruction[2] )
					{
						case 't':
						case 'T':
						{
							switch( instruction[3] )
							{
								case 0:
								{
									bin_file[line][0] = '1';
									bin_file[line][1] = '1';
									bin_file[line][2] = '0';
									bin_file[line][3] = '1';
									bin_file[line][4] = '0';
									bin_file[line][5] = '0';
									for(i = 10; i < 32; i++)
									{
										bin_file[line][i] = '0';
									}
									return 3;// one register only
									break;
									//found out
								}
								default:
								{
									printf("did you mean out in line %d",line);
								}
							}
							break;
						}
						default:
						{
							printf("did you mean out in line %d",line);
						}
					}
					break;
				}
				default:
				{
					printf("did you mean OR or ORI or OUT in line %d",line);
				}
			}
			break;
		}
		case 'x':
		case 'X':
		{
			switch( instruction[1] )
			{
				case 'o':
				case 'O':
				{
					switch( instruction[2] )
					{
						case 'r':
						case 'R':
						{
							switch( instruction[3] )
							{
								case 0:
								{
									bin_file[line][0] = '0';
									bin_file[line][1] = '0';
									bin_file[line][2] = '0';
									bin_file[line][3] = '0';
									bin_file[line][4] = '0';
									bin_file[line][5] = '0';
									bin_file[line][18] = '0';
							        bin_file[line][19] = '1';
									bin_file[line][20] = '1';
									bin_file[line][21] = '0';
									for(i=22; i< 32;i++)
									{
										bin_file[line][i] = '0';
									}
									return 0;
									break;
									//found xor 
								}
								case 'i':
								case 'I':
								{
									switch( instruction[4] )
									{
										case 0:
										{
											bin_file[line][0] = '1';
											bin_file[line][1] = '0';
											bin_file[line][2] = '0';
											bin_file[line][3] = '1';
											bin_file[line][4] = '1';
											bin_file[line][5] = '0';
											return 1;
											break;
											// found xori
										}
										default:
										{
											printf("did you mean xori or xor in line %d",line);
										}
									}
									break;
								}
								default:
								{
									printf("did you mean xor or xori in line %d",line);
								}
							}
							break;
						}
						default:
						{
							printf("did you mean xor or xori in line %d",line);
						}
					}
					break;
				}
				case 'n':
				case 'N':
				{
					switch( instruction[2] )
					{
						case 'o':
						case 'O':
						{
							switch( instruction[3] )
							{
								case 'r':
								case 'R':
								{
									switch( instruction[4] )
									{
										case 0:
										{
											bin_file[line][0] = '0';
											bin_file[line][1] = '0';
											bin_file[line][2] = '0';
											bin_file[line][3] = '0';
											bin_file[line][4] = '0';
											bin_file[line][5] = '0';
											bin_file[line][18] = '0';
									        bin_file[line][19] = '1';
											bin_file[line][20] = '1';
											bin_file[line][21] = '1';
											for(i=22; i< 32;i++)
											{
												bin_file[line][i] = '0';
											}
											return 0;
											break;
											//found xnor
										}
										case 'i':
										case 'I':
										{
											switch( instruction[5] )
											{
												case 0:
												{
													bin_file[line][0] = '1';
													bin_file[line][1] = '0';
													bin_file[line][2] = '0';
													bin_file[line][3] = '1';
													bin_file[line][4] = '1';
													bin_file[line][5] = '1';
													return 1;
													break;
													//found xnori
												}
												default:
												{
													printf("did you mean xnori in line %d",line);
												}
											}
											break;
										}
										default:
										{
											printf("did you mean xnor or xnori in line %d",line);
										}
									}
									break;
								}
								default:
								{
									printf("did you mean xnor or xnori in line %d",line);
								}
							}
							break;
						}
						default:
						{
							printf("did you mean xnor or xnori in line %d",line);
						}
					}
						break;
				}
				default:
				{
					printf("did you mean xor or xori or xnor or xnori in line %d",line);
				}
			}
			break;
		}
		case 'p':
		case 'P':
		{
			switch( instruction[1] )
			{
				case 'u':
				case 'U':
				{
					switch( instruction[2] )
					{
						case 's':
						case 'S':
						{
							switch( instruction[3] )
							{
								case 'h':
								case 'H':
								{
									switch( instruction[4] )
									{
										case 0:
										{
											bin_file[line][0] = '1';
											bin_file[line][1] = '0';
											bin_file[line][2] = '1';
											bin_file[line][3] = '1';
											bin_file[line][4] = '0';
											bin_file[line][5] = '1';
											for(i=6;i<=9;i++)
											{
												bin_file[line][i] = '0';
											}
											for(i=14;i<32;i++)
											{
												bin_file[line][i] = '0';
											}
											return 5;
											break;
											//found push
										}
										default:
										{
											printf("did you mean push in line %d",line);
										}
									}
									break;
								}
								default:
								{
									printf("did you mean push in line %d",line);
								}
							}
							break;
						}
						default:
						{
							printf("did you mean push in line %d",line);
						}
					}
					break;
				}
				case 'O':
				case 'o':
				{
					switch( instruction[2] )
					{
						case 'p':
						case 'P':
						{
							switch( instruction[3] )
							{
								case 0:
								{
									bin_file[line][0] = '1';
									bin_file[line][1] = '0';
									bin_file[line][2] = '1';
									bin_file[line][3] = '1';
									bin_file[line][4] = '1';
									bin_file[line][5] = '0';
									for(i=6;i<=9;i++)
									{
										bin_file[line][i] = '0';
									}
									for(i=14;i<32;i++)
									{
										bin_file[line][i] = '0';
									}
									return 5;
									break;
									//found pop
								}
								default:
								{
									printf("did you mean pop in line %d",line);
								}
							}
							break;
						}
						default:
						{
							printf("did you mean pop in line %d",line);
						}
					}
					break;
				}
				default:
				{
					printf("did you mean push or pop in line %d",line);
				}	
			}
			break;
		}
		case 'l':
		case 'L':
		{
			switch( instruction[1] )
			{
				case 'o':
				case 'O':
				{
					switch( instruction[2] )
					{
						case 'a':
						case 'A':
						{
							switch( instruction[3] )
							{
								case 'd':
								case 'D':
								{
									switch( instruction[4] )
									{
										case 0:
										{
											bin_file[line][0] = '1';
											bin_file[line][1] = '0';
											bin_file[line][2] = '1';
											bin_file[line][3] = '1';
											bin_file[line][4] = '0';
											bin_file[line][5] = '0';
											return 1;
											break;
											//found load
										}
										default:
										{
											printf("did you mean load in line %d",line);
										}
									}
									break;
								}
								default:
								{
									printf("did you mean load in line %d",line);
								}
							}
							break;
						}
						default:
						{
							printf("did you mean load in line %d",line);
						}
					}
					break;
				}
				printf("did you mean load in line %d",line);
			}
			break;
		}	
		case 'b':
		case 'B':
		{
			switch( instruction[1] )
			{
				case 'e':
				case 'E':
				{
					switch( instruction[2] )
					{
						case 0:
						{
							bin_file[line][0] = '1';
							bin_file[line][1] = '1';
							bin_file[line][2] = '0';
							bin_file[line][3] = '0';
							bin_file[line][4] = '0';
							bin_file[line][5] = '0';
							return 8;
							break;
							//found branch if equal
						}
						default:
						{
							printf("did you mean be in line %d",line);
						}
					}
					break;
				}
				case 'n':
				case 'N':
				{
					switch( instruction[2] )
					{
						case 'e':
						case 'E':
						{
							switch(instruction[3])
							{
								case 0:
								{
									bin_file[line][0] = '1';
									bin_file[line][1] = '1';
									bin_file[line][2] = '0';
									bin_file[line][3] = '0';
									bin_file[line][4] = '0';
									bin_file[line][5] = '1';
									return 8;
									break;
									//found branch if not equal
								}
								default:
								{
									printf("did you mean bne in line %d",line);
								}
							}
							break;
						}
						default:
						{
							printf("did you mean be or be in line %d",line);
						}
					}
					break;
				}
				case 'g':
				case 'G':
				{
					switch( instruction[2] )
					{
						case 0:
						{
							bin_file[line][0] = '1';
							bin_file[line][1] = '1';
							bin_file[line][2] = '0';
							bin_file[line][3] = '0';
							bin_file[line][4] = '1';
							bin_file[line][5] = '0';
							return 8;
							break;
							// found branch if greater than
						}
						case 'e':
						{
							switch( instruction[3] )
							{
								case 0:
								{
									bin_file[line][0] = '1';
									bin_file[line][1] = '0';
									bin_file[line][2] = '0';
									bin_file[line][3] = '0';
									bin_file[line][4] = '1';
									bin_file[line][5] = '1';
									return 8;
									break;
									//found branch if greater than or equal
								}
								default:
								{
									printf("did you mean bge in line %d",line);
								}
							}
						}
						default:
						{
							printf("did you mean bge or bg in line %d",line);
						}
					}
					break;
				}
				default:
				{
					printf("did you mean be or bne or bg or bge in line %d",line);
				}
			}
			break;
		}
		case 'j':
		case 'J':
		{
			switch( instruction[1] )
			{
				case 'u':
				case 'U':
				{
					switch( instruction[2] )
					{
						case 'm':
						case 'M':
						{
							switch( instruction[3] )
							{
								case 'p':
								case 'P':
								{
									switch(instruction[4])
									{
										case 0:
										{
											bin_file[line][0] = '0';
											bin_file[line][1] = '1';
											bin_file[line][2] = '0';
											bin_file[line][3] = '0';
											bin_file[line][4] = '0';
											bin_file[line][5] = '0';
											return 6;//no registers at all
											break;
											// found jump
										}
										default:
										{
											printf("did you mean jump in line %d",line);
										}
									}
									break;
								}
								default:
								{
									printf("did you mean jump in line %d",line);
								}
							}
							break;
						}
						default:
						{
							printf("did you mean jump in line %d",line);
						}
					}
					break;
				}
				default:
				{
					printf("did you mean jump in line %d",line);
				}
			}
			break;
		}
		case 'c':
		case 'C':
		{
			switch( instruction[1] )
			{
				case 'a':
				case 'A':
				{
					switch( instruction[2] )
					{
						case 'l':
						case 'L':
						{
							switch( instruction[3] )
							{
								case 'l':
								case 'L':
								{
									switch( instruction[4] )
									{
										case 0:
										{
											bin_file[line][0] = '0';
											bin_file[line][1] = '1';
											bin_file[line][2] = '0';
											bin_file[line][3] = '0';
											bin_file[line][4] = '0';
											bin_file[line][5] = '1';
											return 6;
											break;
											// found call
										}
										default:
										{
											printf("did you mean call in line %d",line);
										}
									}
									break;
								}
								default:
								{
									printf("did you mean call in line %d",line);
								}
							}
							break;	
						}
						default:
						{
							printf("did you mean call in line %d",line);
						}
					}
					break;
				}
				printf("did you mean call in line %d",line);
			}
			break;
		}
		case 'r':
		case 'R':
		{
			switch( instruction[1] )
			{
				case 'e':
				case 'E':
				{
					switch( instruction[2] )
					{
						case 't':
						case 'T':
						{
							switch( instruction[3] )
							{
								case 0:
								{
									bin_file[line][0] = '0';
									bin_file[line][1] = '1';
									bin_file[line][2] = '0';
									bin_file[line][3] = '0';
									bin_file[line][4] = '1';
									bin_file[line][5] = '0';
									for(i=6;i<32;i++)
									{
										bin_file[line][i] = '0';
									}
									return 7;
									break;
									//found return
								}
								default:
								{
									printf("did you mean ret in line %d",line);
								}
							}
							break;
						}
						default:
						{
							printf("did you mean ret in line %d",line);
						}
					}
					break;
				}
				printf("did you mean ret in line %d",line);
			}
			break;
		}
		case 'I':
		case 'i':
		{
			switch( instruction[1] )
			{
				case 'n':
				case 'N':
				{
					switch( instruction[2] )
					{
						case 0:
						{
							bin_file[line][0] = '0';
							bin_file[line][1] = '0';
							bin_file[line][2] = '1';
							bin_file[line][3] = '0';
							bin_file[line][4] = '1';
							bin_file[line][5] = '1';
							for(i=6;i<=13;i++)
							{
								bin_file[line][i] = '0';
							}
							for(i=18;i<32;i++)
							{
								bin_file[line][i] = '0';
							}
							return 4;
							break;
							//found in
						}
						default:
						{
							printf("did you mean IN in line %d",line);
						}
					}
					break;
				}
				default:
				{
					printf("did you mean nop in line %d",line);
				}
			}
			break;
		}
		case 'N':
		case 'n':
		{
			switch( instruction[1] )
			{
				case 'o':
				case 'O':
				{
					switch( instruction[2] )
					{
						case 'p':
						case 'P':
						{
							switch( instruction[3] )
							{
								case 0:
								{
									bin_file[line][0] = '1';
									bin_file[line][1] = '0';
									bin_file[line][2] = '0';
									bin_file[line][3] = '0';
									bin_file[line][4] = '0';
									bin_file[line][5] = '0';
									for (i = 6; i<= 31;i++)
									{
										bin_file[line][i] = '0';
									}
									return 7;//nothing
									break;
									// found nop
								}
								default:
								{
									printf("did you mean nop in line %d",line);
								}
							}
							break;
						}
						default:
						{
							printf("did you mean nop in line %d",line);
						}
					}
					break;	
				}
				default:
				{
					printf("did you mean nop in line %d",line);
				}
			}
			break;
		}
		case 'h':
		case 'H':
		{
			switch( instruction[1] )
			{
				case 'a':
				case 'A':
				{
					switch( instruction[2] )
					{
						case 'l':
						case 'L':
						{
							switch( instruction[3] )
							{
								case 't':
								case 'T':
								{
									switch( instruction[4] )
									{
										case 0:
										{
											bin_file[line][0] = '0';
											bin_file[line][1] = '0';
											bin_file[line][2] = '1';
											bin_file[line][3] = '1';
											bin_file[line][4] = '1';
											bin_file[line][5] = '1';
											for (i = 6; i<= 31;i++)
											{
												bin_file[line][i] = '0';
											}
											//printf("found halt");
											return 7;
											break;
											//found halt
										}
										default:
										{
											printf("did you mean halt in line %d",line);
										}
									}
									break;
								}
								default:
								{
									printf("did you mean halt in line %d",line);
								}
							}
							break;
						}
						default:
						{
							printf("did you mean halt in line %d",line);
						}
					}
					break;
				}
				default:
				{
					printf("did you mean halt in line %d",line);
				}
			}
			break;
		}
		default:
		{
			printf("wrong instruction written in line %d",line);
		}
	}
}
