//doops.c 
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>
#include "doops.h"

//generate sample files to play with 
void doops_create_samples(){
	puts("dummy item right now...");
}

/*first thing to do is get the name of the file to parse and work on*/
FILE *doops_open_file_name(void){
     char filename[SIZE];
     char *find;
     FILE *fp;
   
     while( (fgets(filename,SIZE,stdin)) !=NULL && filename[0]!='\n'){
        if( (find=strchr(filename,'\n'))!=NULL)
            *find='\0';
   
        if ( (fp=fopen(filename,"r")) == NULL){
        perror("\aerror");
        return NULL;
        }
        
        else{
         printf("File opened successfully\n");
         return fp;
        } 
    }
 return NULL;       
}

/*given the file name, return the number of records in it*/
long doops_num_of_records(FILE *thefile){
  long records=0;
  int character;
  while( (character = fgetc(thefile)) !=EOF){
         if ( (character == '\n'))
              records+=1; 
              }
 fprintf(stdout,"Num of records in  this file: %ld\n",records);
  return records; 
}

/*put contents of the file into the doops_t struct so can work on it*/
doops_t *doops_load_lead_file(FILE *filename,long num_of_records){
    long record_count=0;/*current record*/
    int seq_ndx=0;/*current ndx in phone number*/
    int character;/*current character in the stream*/
    int num_ndx;
    doops_t *record;
    /*create an array of doops_t so we can have each number record|1|2|3|4|...*/
    record = malloc(num_of_records * sizeof(doops_t) );
     //record = new doops_t [num_of_records];
    rewind(filename);/*just in case*/

    /*copy every character from the file to a record type struct*/ 
    while( (character = fgetc(filename) )!=EOF){
       /*iterate through every num for each character...*/
       for(num_ndx=0; num_ndx<10; num_ndx++){
          /*only add the characters that are numbers..*/
          if ( (character == nums[num_ndx]) ){
            record[record_count].phone_digit[seq_ndx] = character;
           // printf("record[%d].phone_digit[%d] = %c which is %c\n",
            //record_count,seq_ndx,character,nums[num_ndx]);
            seq_ndx+=1;/*go to the next place in the phone number*/
            //getchar();
            break;//stop loop and next seq and next caracter
           }//end of if block
        }//for loop finished next seq position
       
      /*create a new record in the file*/     
        if( ( character == '\n')){
             record[record_count].phone_digit[seq_ndx] = '\0';
             seq_ndx=0; /*reset info buffer*/
             record_count+=1; /*next record*/
        }       
 }/*end of while*/ 
return record;
}
 

/*flags duplicates in the files and print*/           
void doops_find_duplicates_in_files(doops_t *lead_file,long recs,doops_t *sup_file,long sups){
     /*record is the lead file, dupe is the suppression rename accordingly*/
     long record_ndx;
     long dup_count;
     
    /*used for keeping track of internal deduppes*/
     long prev;
     long next;

     /*both types used these*/
     long times=0;
     long dupes_in_file=0;
     FILE *outfile;
     time_t start,end;/*check how long it took us to parse n records*/
     time(&start);/*start timing right away*/
     printf("dedooping start: %s",ctime(&start) );

     /*if there is no suppression file called just dedoops internal lead file*/
     if ( (sup_file == NULL) ){
        /*open the outfile for internal dupes to print to*/
        if ( (outfile=fopen(INTERNAL_DUPES,"w")) == NULL){
        perror("unable to open file");
        exit(EXIT_FAILURE);
        }

     /*else start counting from first entry and print that entry to output file*/
     for(prev=0;prev<recs;prev++){
       printf("Checking record: %ld\r",prev);                          
		 //this is where we should print the header:
		 //if(prev == 0 ) fputs("phone number, duplicated", outfile); 

       fprintf(outfile,"%s,",lead_file[prev].phone_digit);/*print that first record
       then compare it against all other records in the file*/
         for(next=prev+1;next<recs;next++){/*start counting from next number*/
           if( (strcmp(lead_file[prev].phone_digit,lead_file[next].phone_digit))==0)
           times++;/*how many times does this entry appear*/
         } 
               
     /*once at end of the file for that entry */
     fprintf(outfile,"Duplicated %ld times",times);//on the  same line
     dupes_in_file+=times;/*amalgamate times to dupes for total dupes*/
     times=0;/*reset for next number*/
     fputc('\n',outfile);/*go to next line on output file*/ 
     } /*first for loop end of all records*/
  }/*end of first if loops*/


       /*else if both a lead file and suppression file present*/
  else{
    /*print results compared against suppression file*/
      if ( (outfile = fopen(SUP_DUPES,"w")) == NULL){
          perror("error: ");
          exit(EXIT_FAILURE);
         }

      /*start from the first record in the lead file ....*/
      for(record_ndx=0;record_ndx<recs;record_ndx++){
        /*for each record print it...*/
        fprintf(outfile,"%s,",lead_file[record_ndx].phone_digit);
        fprintf(stdout,"Checking record: %ld\r",record_ndx+1);
        /*...and compare against each num in suppression list...*/
          for(dup_count=0;dup_count<sups;dup_count++){
            /*...for each number if it compares and is equal......*/
               if( (strcmp(lead_file[record_ndx].phone_digit,sup_file[dup_count].phone_digit))==0) 
             times++;
           }//end of for loop
           
      /*next record...but before that print out how many times it appeared in file*/
        fprintf(outfile,"Duplicated %ld times\n",times);
        dupes_in_file+=times;
        times=0;/*reset the occurance of that particular duplicate*/
     }/*end of all records being checked in for loop*/
  }/*end of else and thus end of block */
      
      /*close up the file that was opened....*/   
      if(fclose(outfile)!=0){
      perror("error");
      exit(EXIT_FAILURE);
      }/*end of if and exit*/
      
      /*give a nice little message about what we did*/
      time(&end);/*finish timing out dedooping*/
      /*seconds / 60 = minutes if minutes > 60 divide / 60 == hours if
      hours > 24 / 24 == days etc...*/
      printf("dedooping ended: %s",ctime(&end) );
      printf("%ld doops found in %.1f seconds %s file\n",dupes_in_file,
      difftime(end,start),sup_file == NULL ? "in lead" : "from the supression");
  
}/*end of doops_find_duplicates_in_files*/


/*loads lead and suppression file and dedupes them only really need this funtion
as a caller since can supply it with args and if only 1 file it does only lead file*/
void doops_call_to_print_supression_dupes(){
    FILE *leadFile,*SupFile;/*for getfile return value*/
    long total_records;/*for num of records return value*/
    long total_sups;/*for total suppresion entries*/
    doops_t *Leads;/*return value of LoadLeadfile. should have called it LEADS*/  
    doops_t *Supre;/*return value of doops_load_supression_file.*/
    
    /*get lead file and suppresion file name*/    
    printf("Enter Lead file name: ");   
    if( ( leadFile=doops_open_file_name() ) == NULL )
        return;/*get out of the function so another prompt*/
     
    //else
    total_records=doops_num_of_records(leadFile);/*returns and assigns lines in the file*/
    Leads=doops_load_lead_file(leadFile,total_records);/*put all text content to the struct*/
    if(Leads==NULL){
      perror("\aunable to allocate leads to the struct");
      getchar();
      exit(EXIT_FAILURE);
    }
    
    //else
    if ( Leads) puts("lead file allocated.");
    
    /*Get,count and allocate contents of Suppresion file*/
    printf("Enter suppresion File name: ");
    /*should make this a while sup=openfile == null so prompted to repeat name
    or a do while?*/
    if( (SupFile=doops_open_file_name() ) ==NULL)
        return;/*get out of the function back to the menu then*/
       
   //else put some exception handling here so exits gracefully
   total_sups=doops_num_of_records(SupFile);/*how many lines in supression file*/
   Supre=doops_load_lead_file(SupFile,total_sups);/*put all nums in sup struct*/ 
    if (Supre==NULL){ 
       perror("\aError");
       getchar();
       exit(EXIT_FAILURE);
    }
    //esle
    printf("Supression file allocated. enter to scrub leads");
    getchar();
    doops_find_duplicates_in_files(Leads,total_records,Supre,total_sups);
}

/*loads lead file and dedupes it*/
void doops_call_to_print_internal_dupes(){
/*functions do all the work for the suppresion and lead file*/
    FILE *leadFile;/*for getfile return value*/
    long total_records;/*for num of records return value*/
    doops_t *Leads;/*return value of LoadLeadfile. should have called it LEADS*/ 
    
    /*Get,count and allocate contents of lead file*/
    printf("Lead file name: ");   
    if( ( leadFile=doops_open_file_name() ) != NULL ){/*returns addy of the file in a mode*/
       total_records=doops_num_of_records(leadFile);/*returns and assigns lines in the file*/
       Leads=doops_load_lead_file(leadFile,total_records);/*put all text content to the struct*/
       if (Leads) puts("lead file allocated.");
       /*call the function but let it know it won't be doing suppressions*/
       doops_find_duplicates_in_files(Leads,total_records,NULL,0);   
    } 


}//end of function

/*interface to non gui doops*/
void doops_non_gui_menu(){
     
     int max = 20;
     char temp[max];/*holds user input*/
     
     printf("Please Make your selection or press [ENTER] to quit Doops:\n\n");
	  puts("a) Find Doops in lead File");
     puts("b) Find Doops from suppresion File");
	  puts("c) Create sample files");
     
     /*this menu sucks if you don't enter in a lead file name for choice a
     then it goes to each succesive choice until to exit....so we put break
     at the end of each call to the menu since it will do this 1nce then exit*/
     while( (fgets(temp,max,stdin) ) !=NULL && temp[0] !='\n'){
       switch(toupper(temp[0]) ){
         case 'A':doops_call_to_print_internal_dupes();doops_non_gui_menu();break;
         case 'B':doops_call_to_print_supression_dupes();doops_non_gui_menu();break;
			case 'C':doops_create_samples(); doops_non_gui_menu();break;
         default :puts("That Choice is Not Available.");doops_non_gui_menu();
       }/*end of switch*/
       return;/*in case its a '\n' thats entered*/
     }/*end of while*/
     return;
}/*end of doops_menu() */   

