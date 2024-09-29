#include "swfknife.h"

void show_intro();
FILE *open_input_file(const char *name);
FILE *create_output_file(const char *name);
void go_offset(FILE *file,const unsigned long int offset);
void fast_data_dump(FILE *input,FILE *output,const size_t length);
void data_dump(FILE *input,FILE *output,const size_t length);
unsigned long int get_file_size(FILE *file);
char *get_string_memory(const size_t length);
size_t get_extension_position(const char *source);
char *get_short_name(const char *name);
char* get_name(const char *name,const char *ext);
void check_executable(FILE *input);
void check_signature(FILE *input);
unsigned long int get_movie_length(FILE *input);
void decompile(const char *target,const char *flash);
void work(const char *target);

int main(int argc, char *argv[])
{
 show_intro();
 if(argc==2)
 {
  puts("Working... Please wait");
  work(argv[1]);
  puts("The work has been finished");
 }
 else
 {
  puts("You must give a target file name as the command-line argument!");
 }
 return 0;
}

void show_intro()
{
 putchar('\n');
 puts("Swf knife. Version 0.1.6");
 puts("A simple tool for extracting an Adobe flash movie from a self-played movie");
 puts("This sofware was made by Popov Evgeniy Alekseyevich,2022 year-2024 years");
 puts("This software is distributed under GNU GENERAL PUBLIC LICENSE");
 putchar('\n');
}

FILE *open_input_file(const char *name)
{
 FILE *target;
 target=fopen(name,"rb");
 if (target==NULL)
 {
  puts("Can't open the input file");
  exit(1);
 }
 return target;
}

FILE *create_output_file(const char *name)
{
 FILE *target;
 target=fopen(name,"wb");
 if (target==NULL)
 {
  puts("Can't create the ouput file");
  exit(2);
 }
 return target;
}

void go_offset(FILE *file,const unsigned long int offset)
{
 if (fseek(file,offset,SEEK_SET)!=0)
 {
  puts("Can't jump to the target offset");
  exit(3);
 }

}

void data_dump(FILE *input,FILE *output,const size_t length)
{
 unsigned char data;
 size_t index;
 data=0;
 for (index=0;index<length;++index)
 {
  fread(&data,sizeof(unsigned char),1,input);
  fwrite(&data,sizeof(unsigned char),1,output);
 }

}

void fast_data_dump(FILE *input,FILE *output,const size_t length)
{
 unsigned char *buffer=NULL;
 buffer=(unsigned char*)calloc(length,sizeof(unsigned char));
 if (buffer==NULL)
 {
  data_dump(input,output,length);
 }
 else
 {
  fread(buffer,sizeof(unsigned char),length,input);
  fwrite(buffer,sizeof(unsigned char),length,output);
  free(buffer);
 }

}

unsigned long int get_file_size(FILE *file)
{
 unsigned long int length;
 fseek(file,0,SEEK_END);
 length=ftell(file);
 rewind(file);
 return length;
}

char *get_string_memory(const size_t length)
{
 char *memory=NULL;
 memory=(char*)calloc(length+1,sizeof(char));
 if(memory==NULL)
 {
  puts("Can't allocate memory");
  exit(4);
 }
 return memory;
}

size_t get_extension_position(const char *source)
{
 size_t index;
 for(index=strlen(source);index>0;--index)
 {
  if(source[index]=='.')
  {
   break;
  }

 }
 if (index==0) index=strlen(source);
 return index;
}

char *get_short_name(const char *name)
{
 size_t length;
 char *result=NULL;
 length=get_extension_position(name);
 result=get_string_memory(length);
 strncpy(result,name,length);
 return result;
}

char* get_name(const char *name,const char *ext)
{
  char *result=NULL;
  char *output=NULL;
  size_t length;
  output=get_short_name(name);
  length=strlen(output)+strlen(ext);
  result=get_string_memory(length);
  strcpy(result,output);
  free(output);
  return strcat(result,ext);
}

void check_executable(FILE *input)
{
 char signature[2];
 fread(signature,sizeof(char),2,input);
 if (strncmp(signature,"MZ",2)!=0)
 {
  puts("The executable file of the Flash Player Projector was corrupted");
  exit(5);
 }

}

void check_signature(FILE *input)
{
 unsigned long int signature;
 signature=0;
 fread(&signature,sizeof(unsigned long int),1,input);
 if (signature!=0xFA123456)
 {
  puts("The Flash movie was corrupted");
  exit(6);
 }

}

unsigned long int get_movie_length(FILE *input)
{
 unsigned long int length;
 length=0;
 fread(&length,sizeof(unsigned long int),1,input);
 return length;
}

void decompile(const char *target,const char *flash)
{
 FILE *input;
 FILE *output;
 unsigned long int total,movie;
 input=open_input_file(target);
 check_executable(input);
 total=get_file_size(input);
 go_offset(input,total-8);
 check_signature(input);
 movie=get_movie_length(input);
 go_offset(input,total-movie-8);
 output=create_output_file(flash);
 fast_data_dump(input,output,(size_t)movie);
 fclose(input);
 fclose(output);
}

void work(const char *target)
{
 char *output=NULL;
 output=get_name(target,".swf");
 decompile(target,output);
 free(output);
}