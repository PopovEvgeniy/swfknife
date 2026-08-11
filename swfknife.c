#include "swfknife.h"
#include "format.h"
#include "exitcode.h"

void show_intro();
void show_error(const char *message);
FILE *open_input_file(const char *name);
FILE *create_output_file(const char *name);
void read_data(void *data,const size_t length,FILE *input);
void write_data(const void *data,const size_t length,FILE *output);
void go_offset(FILE *target,const unsigned long int offset);
unsigned long int get_file_size(FILE *target);
char *get_memory(const size_t length);
void check_executable(FILE *input);
unsigned long int check_signature(FILE *input);
void data_dump(FILE *input,FILE *output,const size_t length);
void fast_data_dump(FILE *input,FILE *output,const size_t length);
size_t get_name_without_extension_length(const char *source);
char *get_name_without_extension(const char *name);
char *get_name(const char *name,const char *extension);
void decompile(const char *target,const char *flash);
void work(const char *target);

int main(int argc, char *argv[])
{
 show_intro();
 if(argc<2)
 {
  puts("You must give a target file name as the command-line argument!");
  exit(COMMAND_LINE_ARGUMENTS_ERROR);
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
 puts("Swf knife. Version 0.3.3");
 puts("A simple tool for extracting an Adobe flash movie from a standalone movie");
 puts("This sofware was made by Popov Evgeniy Alekseyevich,2022-2026 years");
 puts("This software is distributed under the GNU GENERAL PUBLIC LICENSE");
 putchar('\n');
}

void show_error(const char *message)
{
 fputc('\n',stderr);
 fputs(message,stderr);
 fputc('\n',stderr);
}

FILE *open_input_file(const char *name)
{
 FILE *target=NULL;
 if (name==NULL)
 {
  show_error("Can't open the input file");
  exit(OPEN_FILE_ERROR);
 }
 target=fopen(name,"rb");
 if (target==NULL)
 {
  show_error("Can't open the input file");
  exit(OPEN_FILE_ERROR);
 }
 return target;
}

FILE *create_output_file(const char *name)
{
 FILE *target=NULL;
 if (name==NULL)
 {
  show_error("Can't create the ouput file");
  exit(CREATE_FILE_ERROR);
 }
 target=fopen(name,"wb");
 if (target==NULL)
 {
  show_error("Can't create the ouput file");
  exit(CREATE_FILE_ERROR);
 }
 return target;
}

void read_data(void *data,const size_t length,FILE *input)
{
 if (fread(data,sizeof(char),length,input)<length)
 {
  show_error("Can't read data!");
  exit(READ_DATA_ERROR);
 }

}

void write_data(const void *data,const size_t length,FILE *output)
{
 if (fwrite(data,sizeof(char),length,output)<length)
 {
  show_error("Can't write data!");
  exit(WRITE_DATA_ERROR);
 }

}

void go_offset(FILE *target,const unsigned long int offset)
{
 if (fseek(target,offset,SEEK_SET)!=0)
 {
  show_error("Can't jump to the target offset");
  exit(SET_FILE_POSITION_ERROR);
 }

}

unsigned long int get_file_size(FILE *target)
{
 unsigned long int length=0;
 if (fseek(target,0,SEEK_END)!=0)
 {
  show_error("Can't get the file size!");
  exit(GET_FILE_SIZE_ERROR);
 }
 length=ftell(target);
 rewind(target);
 return length;
}

char *get_memory(const size_t length)
{
 char *memory=NULL;
 memory=(char*)calloc(length,sizeof(char));
 if(memory==NULL)
 {
  show_error("Can't allocate memory");
  exit(MEMORY_ALLOCATION_ERROR);
 }
 return memory;
}

void check_executable(FILE *input)
{
 char signature[2];
 read_data(signature,2,input);
 if (strncmp(signature,"MZ",2)!=0)
 {
  show_error("The executable file of the Flash Player projector was corrupted");
  exit(INVALID_FLASH_PROJECTOR_ERROR);
 }

}

unsigned long int check_signature(FILE *input)
{
 service_information information;
 read_data(&information,sizeof(service_information),input);
 if (strncmp(information.signature,"V4",2)!=0)
 {
  show_error("The standalone movie was corrupted");
  exit(INVALID_FLASH_MOVIE_ERROR);
 }
 return information.length;
}

void data_dump(FILE *input,FILE *output,const size_t length)
{
 char *buffer=NULL;
 size_t current=0;
 size_t elapsed=0;
 size_t block=4096;
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
 char *buffer=NULL;
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

size_t get_name_without_extension_length(const char *source)
{
 size_t index=0;
 size_t position=0;
 size_t length=0;
 if (source!=NULL)
 {
  length=strlen(source);
 }
 for (index=length;index>0;--index)
 {
  position=index-1;
  if (source[position]==DIRECTORY_SEPARATOR)
  {
   break;
  }
  if (source[position]=='.')
  {
   if (position>0)
   {
    if ((source[position-1]!=DIRECTORY_SEPARATOR) && (source[position-1]!='.'))
    {
     length=position;
     break;
    }

   }

  }

 }
 return length;
}

char *get_name_without_extension(const char *name)
{
 char *result=NULL;
 size_t length=0;
 length=get_name_without_extension_length(name);
 if (length>0)
 {
  result=get_memory(length+1);
  strncpy(result,name,length);
 }
 return result;
}

char *get_name(const char *name,const char *extension)
{
  char *result=NULL;
  char *name_without_extension=NULL;
  size_t name_length=0;
  size_t extension_length=0;
  name_without_extension=get_name_without_extension(name);
  if (name_without_extension!=NULL)
  {
   name_length=strlen(name_without_extension);
  }
  if (extension!=NULL)
  {
   extension_length=strlen(extension);
  }
  if ((name_length>0) && (extension_length>0))
  {
   result=get_memory(name_length+extension_length+1);
   strncpy(result,name_without_extension,name_length);
   strncat(result,extension,extension_length);
  }
  free(name_without_extension);
  return result;
}

void decompile(const char *target,const char *flash)
{
 FILE *input=NULL;
 FILE *output=NULL;
 unsigned long int total=0;
 unsigned long int movie=0;
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