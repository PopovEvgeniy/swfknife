#include "swfknife.h"
#include "format.h"

void show_intro();
FILE *open_input_file(const char *name);
FILE *create_output_file(const char *name);
void read_data(void *data,const size_t length,FILE *input);
void write_data(const void *data,const size_t length,FILE *output);
void go_offset(FILE *target,const unsigned long int offset);
char *get_memory(const size_t length);
void check_executable(FILE *input);
unsigned long int check_signature(FILE *input);
void data_dump(FILE *input,FILE *output,const size_t length);
void fast_data_dump(FILE *input,FILE *output,const size_t length);
unsigned long int get_file_size(FILE *target);
size_t get_extension_position(const char *source);
char *get_short_name(const char *name);
char *get_name(const char *name,const char *ext);
void decompile(const char *target,const char *flash);
void work(const char *target);

int main(int argc, char *argv[])
{
 show_intro();
 if(argc<2)
 {
  puts("You must give a target file name as the command-line argument!");
 }
 else
 {
  puts("Working... Please wait");
  work(argv[1]);
  puts("The work has been finished");
 }
 return 0;
}

void show_intro()
{
 putchar('\n');
 puts("Swf knife. Version 0.2.9");
 puts("A simple tool for extracting an Adobe flash movie from a standalone movie");
 puts("This sofware was made by Popov Evgeniy Alekseyevich,2022-2026 years");
 puts("This software is distributed under the GNU GENERAL PUBLIC LICENSE");
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

void read_data(void *data,const size_t length,FILE *input)
{
 fread(data,length,sizeof(char),input);
 if (ferror(input)!=0)
 {
  puts("Can't read data!");
  exit(3);
 }

}

void write_data(const void *data,const size_t length,FILE *output)
{
 fwrite(data,length,sizeof(char),output);
 if (ferror(output)!=0)
 {
  puts("Can't write data!");
  exit(4);
 }

}

void go_offset(FILE *target,const unsigned long int offset)
{
 if (fseek(target,offset,SEEK_SET)!=0)
 {
  puts("Can't jump to the target offset");
  exit(5);
 }

}

char *get_memory(const size_t length)
{
 char *memory=NULL;
 memory=(char*)calloc(length,sizeof(char));
 if(memory==NULL)
 {
  puts("Can't allocate memory");
  exit(6);
 }
 return memory;
}

void check_executable(FILE *input)
{
 char signature[2];
 read_data(signature,2,input);
 if (strncmp(signature,"MZ",2)!=0)
 {
  puts("The executable file of the Flash Player projector was corrupted");
  exit(7);
 }

}

unsigned long int check_signature(FILE *input)
{
 service_information information;
 read_data(&information,sizeof(service_information),input);
 if (strncmp(information.signature,"V4",2)!=0)
 {
  puts("The standalone movie was corrupted");
  exit(8);
 }
 return information.length;
}

void data_dump(FILE *input,FILE *output,const size_t length)
{
 char *buffer;
 size_t current,elapsed,block;
 elapsed=0;
 block=4096;
 buffer=get_memory(block);
 for (current=0;current<length;current+=block)
 {
  elapsed=length-current;
  if (elapsed<block)
  {
   block=elapsed;
  }
  read_data(buffer,block,input);
  write_data(buffer,block,output);
 }
 free(buffer);
}

void fast_data_dump(FILE *input,FILE *output,const size_t length)
{
 char *buffer;
 buffer=(char*)malloc(length);
 if (buffer==NULL)
 {
  data_dump(input,output,length);
 }
 else
 {
  read_data(buffer,length,input);
  write_data(buffer,length,output);
  free(buffer);
 }

}

unsigned long int get_file_size(FILE *target)
{
 unsigned long int length;
 fseek(target,0,SEEK_END);
 length=ftell(target);
 rewind(target);
 return length;
}

size_t get_extension_position(const char *source)
{
 size_t index,position;
 position=strlen(source);
 for(index=position;index>0;--index)
 {
  if(source[index]=='.')
  {
   position=index;
   break;
  }

 }
 return position;
}

char *get_short_name(const char *name)
{
 size_t length;
 char *result=NULL;
 length=get_extension_position(name);
 result=get_memory(length+1);
 return strncpy(result,name,length);
}

char *get_name(const char *name,const char *ext)
{
  char *result=NULL;
  char *output=NULL;
  size_t length;
  output=get_short_name(name);
  length=strlen(output)+strlen(ext);
  result=get_memory(length+1);
  sprintf(result,"%s%s",output,ext);
  free(output);
  return result;
}

void decompile(const char *target,const char *flash)
{
 FILE *input;
 FILE *output;
 unsigned long int total,movie;
 input=open_input_file(target);
 check_executable(input);
 total=get_file_size(input);
 go_offset(input,total-SERVICE_LENGTH);
 movie=check_signature(input);
 go_offset(input,total-movie-SERVICE_LENGTH);
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