/*
The Network Wizards Interface cable
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    With the interface cable, one can program the NAM and ESN on the phone 
with this mod.  Clone a phone fast, easy and fun!


ESN Location #1  $be8e-$be91
ESN Location #2  $be93-$be96
ESN Location #3  $be98-$be9b
ESN Location #4  $be9d-$bea0
ESN location #5  $bea2-$bea5
*/

#include <stdio.h>
#include <dos.h>
#include <conio.h>
#include <bios.h>
#include <ctype.h>
#include <math.h>
#include <string.h>

#define FALSE           0
#define TRUE            1
#define SWAP(a,b) (a^=b,b^=a,a^=b)   
typedef unsigned char bool;
typedef unsigned char byte;
typedef unsigned int  word;

#include "ctlib.h"

/*    Defines that CTLIB does not have   */
#define CT_KEY_1AND3      0x30
#define CT_KEY_4AND6      0x31
#define CT_KEY_7AND9      0x32
#define CT_KEY_STAR_POUND 0x33
#define CT_KEY_RCL_MENU   0x28          
#define CT_KEY_SND_END    0x35
/*   You may want to use these too!#@ */

#define BUFLEN          128
#define ESC             0x1B

char buf[BUFLEN];
char ps_system;
byte nambyte = 0x00;
byte namonebyte =0x01;
char ournum[32];                        /* our telephone number */
char ournum1[32];
char ournum2[32];
char ournum3[32];
char ournum4[32];
char ournum5[32];
unsigned long ouresn;                   /* our esn */
byte namindex;
word pointer;
byte cmd;
word key;


main(argc,argv)
int argc;
char *argv[];
{
   int i;
    
   /* initialize ct library using the specified COM port */
   if (argc > 1)
      {
      if (*argv[1] == '1')
     ct_lib_init(900,0x3f8,4);
      else if (*argv[1] == '2')
     ct_lib_init(900,0x2f8,3);
      else if (*argv[1] == '3')
     ct_lib_init(900,0x3e8,5);
      else
     {
        puts("Type 'TMPRO 2' to use COM2");
        exit(0);
     }
      }
   else
      ct_lib_init(900,0x3f8,4);           /* com1 by default */
   for (;;)
   {
     
   /* power up oki and tell it what mode to use */
   if (!ct_on(MODE_TEST))
      {
     fprintf(stderr,"?No response from OKI\n");
     cleanup();
     exit(1);
      }
   if (!ct_get_nams())
      {
     cprintf("?Can't read NAM info\r\n");
     cleanup();
     exit(1);
      }
   pointer = 0xBF2C;
   ct_read_mem(&namindex,pointer,1);
   strcpy(ournum,nam_info[namindex - 1].number);    /* use current nam */
   printf("\n\nTel# is %s, ",ournum);
   if (!ct_get_esn(&ouresn))
      {
     fprintf(stderr,"?Can't get ESN\n");
     cleanup();
     exit(1);
      }
   printf("ESN is %08lx, ",ouresn);
   printf("Current NAM index is #%d\n\n", namindex);
   strcpy(ournum1,nam_info[0].number);
   strcpy(ournum2,nam_info[1].number);
   strcpy(ournum3,nam_info[2].number);
   strcpy(ournum4,nam_info[3].number);
   strcpy(ournum5,nam_info[4].number);

   cmd_elite_stuff();
   }
}



cleanup(void)
{
   ct_off();                             /* turn off phone */
   ct_lib_done();                        /* cleanup library stuff */
   return;
}


cmd_elite_stuff(void)
{
   char ch;
   unsigned long esn;
   byte counter;
   char sysid[6];

   fetch_esn(&esn,1);
   printf("NAM#1 ESN : %08lx\n",esn);
   printf("NAM#1 MIN : %s\n\n",ournum1);
   fetch_esn(&esn,2);
   printf("NAM#2 ESN : %08lx\n",esn);
   printf("NAM#2 MIN : %s\n\n",ournum2);
   fetch_esn(&esn,3);
   printf("NAM#3 ESN : %08lx\n",esn);
   printf("NAM#3 MIN : %s\n\n",ournum3);
   fetch_esn(&esn,4);
   printf("NAM#4 ESN : %08lx\n",esn);
   printf("NAM#4 MIN : %s\n\n",ournum4);
   fetch_esn(&esn,5);
   printf("NAM#5 ESN : %08lx\n",esn);
   printf("NAM#5 MIN : %s\n\n",ournum5);
   printf("Enter number of NAM to configure (1-5, Enter 6 to exit) : ");
   ch = getche();
   
   if (ch == 54)
      out_of_here();
   
   printf("\nEnter new ESN : ");
   scanf("%8lx", &esn);
   printf("Enter new MIN : ");
   scanf("%10s", &ournum);
   printf("Enter system ID : ");
   scanf("%5s", &sysid);
   
   store_esn(esn, ch-48);

   nambyte = ch-48;
   ct_set_block(&nambyte,0xBF2C,1);
   ct_set_block(&nambyte,0xC0FF,1);

   store_min((ch-48),ournum,sysid);
   return;
 
}


int fetch_esn(esn,nam)
unsigned long *esn;
int nam;
{
   word addr;
   union esn_un {
      unsigned long l;
      byte b[4];
   } myesn;

   switch (nam) {
      case 1:  addr = 0xBE8E; break;
      case 2:  addr = 0xBE93; break;
      case 3:  addr = 0xBE98; break;
      case 4:  addr = 0xBE9D; break;
      case 5:  addr = 0xBEA2; break;
      default: exit(1); break;
   }

   ct_read_block(esn,addr,4);
   myesn.l = *esn;
   SWAP(myesn.b[0],myesn.b[3]);
   SWAP(myesn.b[1],myesn.b[2]);
   *esn = myesn.l;
   return(0);
}


int store_esn(unsigned long stored_esn, int nam)
{
   word addr;
   union esn_un {
      unsigned long l;
      byte b[4];
   } myesn;

   switch (nam) {
      case 1:  addr = 0xBE8E; break;
      case 2:  addr = 0xBE93; break;
      case 3:  addr = 0xBE98; break;
      case 4:  addr = 0xBE9D; break;
      case 5:  addr = 0xBEA2; break;
      default: exit(1);     break;
   }

   myesn.l = stored_esn;
   SWAP(myesn.b[0],myesn.b[3]);
   SWAP(myesn.b[1],myesn.b[2]);
   stored_esn = myesn.l;
   ct_set_block(&stored_esn,addr,4);
   return(0);
}


store_min(int nam, char *num, char *sysid)
{
   int x;
   ct_off();
   delay(1000);
   if (!ct_on(MODE_NORMAL))
   {
      printf("There has been an err communicating with the OKI\n");
      printf("Unplug CTEK cable, init phone and try again.\n\n");
      cleanup();
      exit(1);
   }
   delay(700);
/*   send(CT_KEY_RCL_MENU); */
   ct_keypress(CT_KEY_RCL_MENU);
   delay(250);
   ct_keypress(CT_KEY_RELEASE);
   send(CT_KEY_STAR);
   delay(200);
   send(CT_KEY_6);
   send(CT_KEY_2);
   send(CT_KEY_7);
   send(CT_KEY_2);
   send(CT_KEY_9);
   send(CT_KEY_8);
   send(CT_KEY_5);
   send(CT_KEY_4);
   send(CT_KEY_POUND);
   printf("Waiting for messages to settle\n");
   delay(4000);
   for (x=0;x<4+nam;x++)
      send(CT_KEY_DOWN);
   printf("Waiting for NAM %d to fall through\n",nam);
   delay(3000);
   sendnum(num);
   send(CT_KEY_STO);
   send(CT_KEY_DOWN);
   sendnum(sysid);
   send(CT_KEY_STO);
   send(CT_KEY_CLR);
   send(CT_KEY_CLR);
   ct_off();
   return(0);
}


sendnum(char *number)
{
   int x;

   for (x=0;x<strlen(number);x++)
   {
      switch (number[x])
      {
          case '0' :     send(CT_KEY_0); break;
          case '1' :     send(CT_KEY_1); break;
          case '2' :     send(CT_KEY_2); break;
          case '3' :     send(CT_KEY_3); break;
          case '4' :     send(CT_KEY_4); break;
          case '5' :     send(CT_KEY_5); break;
          case '6' :     send(CT_KEY_6); break;
          case '7' :     send(CT_KEY_7); break;
          case '8' :     send(CT_KEY_8); break;
          case '9' :     send(CT_KEY_9); break;
          default  :     printf("Fuckup in sendnum string!"); exit(1); break;
      }       // switch
   }       // for
   return(0);
}


/* "push" key for 100 milliseconds */
send(key)
int key;
{
   ct_keypress(key);
   delay(120);
   ct_keypress(CT_KEY_RELEASE);
   delay(10);
   return(0);
}


set_nam_index(void)
{
   int namnum;
   int y;
   int z;
   ct_on(MODE_TEST);
   ct_get_nams();
   pointer = 0xBF2C;
   ct_read_mem(&namindex,pointer,1);
   printf("\nPresent NAM index in use is #%d\n",namindex);
   ct_off();
   printf("Which NAM would you like to use? (1-5) : ");
   scanf("%d",&namnum);
   ct_on(MODE_NORMAL);
   delay(700);
   send(CT_KEY_MENU);
   send(CT_KEY_MENU);
   send(CT_KEY_MENU);
   send(CT_KEY_MENU);
   send(CT_KEY_MENU);
   send(CT_KEY_MENU);
   send(CT_KEY_MENU);
   send(CT_KEY_MENU);
   send(CT_KEY_RCL);
   send(CT_KEY_1);
   send(CT_KEY_2);
   send(CT_KEY_3);
   send(CT_KEY_4);
   send(CT_KEY_5);
   send(CT_KEY_6);

   switch(namindex) {
      case 1: y=5; break;
      case 2: y=4; break;
      case 3: y=3; break;
      case 4: y=2; break;
      case 5: y=1; break;
      default: y=5; break;
   }
   for (z=0;z<(namnum+y);z++)
   {
      send(CT_KEY_RCL);
   }
   send(CT_KEY_STO);
   ct_off();
   return;
}


out_of_here(void)
{
   printf("\nPlease wait...\n\n");
   set_nam_index();
   cleanup();
   exit();
   return(0);
}