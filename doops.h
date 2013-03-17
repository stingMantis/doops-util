/*doops.h*/
#ifndef DOOPS_HEADER
#define DOOPS_HEADER

#include <stdio.h>

#define SIZE 200/*a basic buffer size used by filename[char]*/
#define DIGITS 30/*for phone number digits*/
#define nums "0123456789"/*allowable digits in phone number*/
#define DOOPS_VERSION "1.0.1"

#define INTERNAL_DUPES "internal-doops.txt"
#define SUP_DUPES "suppression-doops.txt"

/*basic record struct and we can add complexity to it later ex: addy*/
typedef struct{
        char flag;
        char phone_digit[DIGITS];
}doops_t;
/*ex: doops_t[NUM_RECORDS].phone_digit[ndx]='3'*/

/*opens file for reading and work and returns its addy*/
FILE* doops_open_file_name(void);

/*goes through a given file and counts num of records*/
long doops_num_of_records(FILE *);

/*allocate contents of file to struct*/
doops_t *doops_load_lead_file(FILE *,long );

/*removes doops from lead/suppression based on parameters and prints out*/
void doops_find_duplicates_in_files(doops_t *lead_file,long num_of_leads,doops_t *sup_file,long num_of_sup);

/*main interface to function from the cmd line*/
void doops_non_gui_menu();

/*specify the job we want to do could amalgamate but code would be bulky*/
void doops_call_to_print_supression_dupes(void);
void doops_call_to_print_internal_dupes(void);

#endif
