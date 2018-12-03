
/* =====[ interpreter sample ]==============================================

   File Name:       interp.c 
   
   Description:     A fully functional command line for the ESP32.
                    Sample functions show easy extendibility. 
                    ? and help shows an example of command aliasing.

   Revisions:

      REV   DATE            BY              DESCRIPTION
      ----  -----------     ----------      ------------------------------
      0.00  nov.29.2018     Peter Glen      Initial version.
      0.00  dec.02.2018     Peter Glen      Release as ESP sample

   ======================================================================= */

#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <assert.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/timers.h"
#include "nvs_flash.h"
#include "esp_event_loop.h"
#include "tcpip_adapter.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_now.h"
#include "rom/ets_sys.h"
#include "rom/crc.h"

#include "interp.h"

//////////////////////////////////////////////////////////////////////////
//  ESP32 monitor interpreter.
//
// How to add command:
//
// Add command string to commands, help string to help. 
// Add function to handle request.
// Fill in arrays: cmdarr funcarr helparr with the new values. 
// The interpreter groups functions and command by ordinal, edit all
// three at the same time.
//

// The overall interpreter is primitive. Assuming single spaces 
// between arguments ... etc ....
// Remember the intent is to set params or diagnose stuff on an 
// embedded system. See example commands for extending it.

//------------------------------------------------------------------------                        
// Commands:

static char echocmd[]  = "echo";
static char noopcmd[]  = "noop";
static char memcmd[]   = "mem";
static char bootcmd[]  = "reboot";
static char statcmd[]  = "stat";
static char dumpcmd[]  = "dump";
static char helpcmd[]  = "help";
static char qmcmd[]    = "?";

static char echohelp[]  = "echo: echo string back to terminal";
static char noophelp[]  = "noop: NOOP does nothing";
static char memhelp[]   = "mem: Show  memory consumption (leak detection)";
static char boothelp[]  = "reboot: reboot ";
static char stathelp[]  = "stat: show  internal tables and statistics";
static char dumphelp[]  = "dump: dump current stuff ...";
static char helphelp[]  = "help: Show help on command";

static int echofunc(char *buff, int len); 
static int noopfunc(char *buff, int len); 
static int memfunc(char *buff, int len);
static int bootfunc(char *buff, int len);
static int statfunc(char *buff, int len); 
static int dumpfunc(char *buff, int len); 
static int helpfunc(char *buff, int len); 

// -----------------------------------------------------------------------
// Command Functions

void  *cmdarr[] = {
        echocmd, noopcmd, memcmd, bootcmd, 
        statcmd, dumpcmd, qmcmd, helpcmd, NULL
        };

void *funcarr[] = {
        echofunc, noopfunc, memfunc, bootfunc, 
        statfunc, dumpfunc, helpfunc, helpfunc, NULL
        };

void *helparr[] = {
        echohelp, noophelp, memhelp, boothelp, 
        stathelp, dumphelp, helphelp, helphelp, NULL
        };                                                

// -----------------------------------------------------------------------
// This drives the parser

struct parse
{
    char    *cmd;
    char    *help;
    int     len;
    void    (*func)(char *str, int len);
};

static struct parse parsearr[ sizeof(cmdarr) / sizeof(char *) + 2];

//////////////////////////////////////////////////////////////////////////
//
// Parse command buffer. Call command function if command recognized.
// Return -1 if no command recognized

static int parse_cmd(char *buff, int len)

{
    int ret = -1, idx = 0;

    while(1)
        {
        struct parse currp = parsearr[idx];
        if(currp.cmd == NULL)
            break;

        if(strncmp(buff, currp.cmd, currp.len) == 0)
            {
            //printf("Found cmd: '%s'\n", currp.cmd);
            ret = 0;
            currp.func(buff, len);
            }
        idx++;
        }
    if(ret != 0)
        {
        printf("Command '%s' not found.\n", buff);
        }
    return ret;
}

//////////////////////////////////////////////////////////////////////////
//  command interpreter

int     interpret(char *comm)

{
    int ret = 0;
    //printf("got command: '%s'\n", comm);
    ret = parse_cmd(comm, strlen(comm));
    return ret;
}

//////////////////////////////////////////////////////////////////////////
// Functions:

int     get_mem_usage()

{
    multi_heap_info_t info;
    heap_caps_get_info(&info, MALLOC_CAP_DEFAULT);
    return info.total_allocated_bytes;
}


static int statfunc(char *buff, int len)
{
    return 0;
}

static int dumpfunc(char *buff, int len)
{
    return 0;
}
 
static int helpfunc(char *buff, int len)
{
    char *cmd2 = strstr(buff, " ");
    
    if(cmd2 != NULL)
        {
        //printf("Help for '%s' command:\n", cmd2);
        for(int idx = 0; parsearr[idx].cmd != NULL; idx++)
            {
            if(strcmp(parsearr[idx].cmd, cmd2 + 1) == 0)
                {
                printf("%s\n", parsearr[idx].help);
                break;
                }
            }
        }
    else
        {
        printf("Available functions:\n");
        for(int idx = 0; parsearr[idx].cmd != NULL; idx++)
            {
            printf("%s ", parsearr[idx].cmd);
            }
        printf("\n");
        }
    return 0;
}
 
static int echofunc(char *buff, int len)
{
    char *cmd2 = strstr(buff, " ");
    
    if(cmd2 != NULL)
        {
        printf("%s\n", cmd2 + 1);
        }
    else
        {
        printf("Echo with blank argument\n");
        }
    return 0;
}
 
static int noopfunc(char *buff, int len)
{
    return 0;
}

static int bootfunc(char *buff, int len)
{
    printf(" reboot in 2 seconds.\n");
    vTaskDelay(2000 / portTICK_RATE_MS);
    esp_restart();
    return 0;
}

static int memfunc(char *buff, int len)
{
    printf(" used memory: %d\n", get_mem_usage()); 
    return 0;
}

//////////////////////////////////////////////////////////////////////////
// Get string from terminal.

void    get_term_str(char *cline, int maxlen)
{
    int prog = 0;     
    
    while(1)
        {
        int cc = fgetc(stdin);
        if(cc == -1)
            {
            int ccc = fgetc(stdin);     // throw it away
            (void)ccc;
            //printf("-%c", ccc); fflush(stdout);
            vTaskDelay(50 / portTICK_RATE_MS);
            continue;
            }
        if(cc == '\r')
            continue;
            
        printf("%c", cc); fflush(stdout);
        if(cc == '\n' || prog >= maxlen - 2)
            {
            cline[prog] = '\0';
            printf("\n");
            break;
            }
        else
            {
            cline[prog++] = cc;
            }
        }
}

// -----------------------------------------------------------------------
// Fill in configured strings to parser array.
// Reason why it is done at run time, is we can obtain strlen 
// for the commands. Also, the compiler flags lots of stuff if we 
// misconfigure items.

void    initparser()
{
    int idx = 0;
    while(1)
        {
        char *curr = cmdarr[idx];
        parsearr[idx].cmd = curr;
        if(curr == NULL)
            {
            break;
            }
        parsearr[idx].len = strlen(curr);
        parsearr[idx].func = funcarr[idx];
        parsearr[idx].help = helparr[idx];
        idx++;
        }
}

// EOF


