#define SERVICE_LENGTH 8

typedef struct
{
 char signature[4];
 unsigned int length:32;
} service_information;