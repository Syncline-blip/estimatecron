//  CITS2002 Project 1 2022
//  Student 1: 22957291 Smith Owen
//  Student 2: 23419439 Lumagbas John

// Essential Libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <time.h>

// Macros, predefined length
#define MAX_COMMANDNAME_LEN 50
#define MAX_COMMAND_LEN 100
int numOfcommands = 0; //Number of commands in the estimates file
int numOfCrontabCommands = 0; //Number of commands in the crontab file
int totalInvoked = 0; //Total number of commands invoked in the month
int maxRunning = 0; //Maximum number of programs that ran at once

/**
 * Struct   : CronCommand
 * Purpose  : Stores commands specified in estimatesfiles. Stores command name, duration and
 *         the number of times that command is invoked.
 */
typedef struct 
{
    char commandName[MAX_COMMANDNAME_LEN];
    char taskDuration[10];
    int invokeCount;

} CronCommand;

/**
 * Struct   : CronTab
 * Purpose  : Stores commands specified in crontab-file. Stores the command's name and the: time, day, week and month of when
 *         the command is to be invoked.
 */
typedef struct 
{
    char minute[4];
    char hour[4];
    char month[4];    
    char dayOfMonth[4];
    char dayOfWeek[5]; // Supports both integer AND char values
    char commandName[MAX_COMMANDNAME_LEN];

} CronTab;

/**
 * Struct   : Running
 * Purpose  : Stores currently running commands and their specific finish times
 */
typedef struct
{
    char commandName[MAX_COMMANDNAME_LEN];
    int finishHour;
    int finishMin;
    int finishDay;
    
} Running;

// Array storage of each struct, iterated through when need to be accessed
static CronTab cronCmd[MAX_COMMAND_LEN];
static CronCommand currCmd[MAX_COMMAND_LEN]; 
static Running runningCommands[MAX_COMMANDNAME_LEN];


/**
 * Function     : crontabErrorMessage()
 * Parameters   : int lineNum, char line[MAX_COMMAND_LEN]
 * Returns      : (none)   
 * Purpose      : Shows incorrectly formatted line in crontab-file, invokes exit() when called.
 */
void crontabErrorMessage(int lineNum, char line[MAX_COMMAND_LEN])
{
    printf("ERROR: line %d in crontab file not recognised: '%s'\n", lineNum,line);
    exit(EXIT_FAILURE);
}

/**
 * Function     : estimatesErrorMessage()
 * Parameters   : int lineNum, char line[MAX_COMMAND_LEN]
 * Returns      : (none)   
 * Purpose      : Shows incorrectly formatted line in estimates-file, invokes exit() when called.
 */
void estimatesErrorMessage(int lineNum, char line[MAX_COMMAND_LEN])
{
    printf("ERROR: line %d in estimates file not recognised: '%s'\n", lineNum,line);
    exit(EXIT_FAILURE);
}

/**
 * Function     : estimatesOpener()
 * Parameters   : char fileName[]
 * Returns      : (none)   
 * Purpose      : Opens estimates file and parses its data into CronCommand struct
 */
void estimatesOpener(char fileName[])
{
    FILE* fp = fopen(fileName, "r");
    char buff[MAX_COMMAND_LEN]; //Used to store the line being read.
    char* pch;
    int line = 0; //Number of lines count variable
    char lineCpy[MAX_COMMAND_LEN]; //A copy of the buff used for error output
    int digCheck;
    char *ptr;
    
    if(fp == NULL){
        fprintf(stderr, "can't open %s: %s\n", fileName, strerror(errno)); // VERY SPECIFIC ERROR
        exit(EXIT_FAILURE); 
    }
    else
    {
        do
        {
            while(fgets(buff, MAX_COMMAND_LEN, fp) != NULL)
            {
                if(strchr(buff, '#') != NULL) // strchr instead of using array indexing, it goes through the whole array for you.
                {
                    ++line;
                    continue;
                }
                else
                {
                    ++line;
                    buff[strcspn(buff, "\n")] = 0;
                    strcpy(lineCpy,buff);
                    
                    pch = strtok(buff," ");
                    if(pch != NULL)
                    {
                        strcpy(currCmd[numOfcommands].commandName, pch);
                    }else {estimatesErrorMessage(line, lineCpy); }
                    
                    pch = strtok(NULL, "   ");
                    digCheck = strtol(pch, &ptr, 10);
                    if(digCheck != 0 || errno != EINVAL) //Ensures that there was only a number in this field.
                    {
                        strcpy(currCmd[numOfcommands].taskDuration, pch);
                    }else { estimatesErrorMessage(line, lineCpy); }
                    
                    //If there is more content in the file then abort as doesn't match format.
                     pch = strtok(NULL, "   ");
                     if(pch != NULL){
                         estimatesErrorMessage(line, lineCpy);
                     }
                    ++numOfcommands; // If the format of the file is correct, proceed and count;
                }
            }
        } while (!feof(fp)); // Stops at end of file
        fclose(fp);
    }
}

/**
 * Function     : checkCommandsExist()
 * Parameters   : (none)
 * Returns      : (none)   
 * Purpose      : Check if a command exists in both files and are both stored in their respective structs.
 */
void checkCommandsExist()
{
    for(int eachCrontab = 0; eachCrontab < numOfCrontabCommands; ++eachCrontab)
    {
        int valid = 0;
        
        for(int i = 0; i < numOfcommands; ++i)
        {
            if(strcmp(cronCmd[eachCrontab].commandName,currCmd[i].commandName) == 0)
            {
                valid = 1;
            }
        }
        if(valid == 0)
        {
            printf("'%s' is an unknwon command\n",cronCmd[eachCrontab].commandName);
            exit(EXIT_FAILURE);
        }
    }
}

/**
 * Function     : cronTabOpener()
 * Parameters   : char fileName[]
 * Returns      : (none)   
 * Purpose      : Opens crontab file and parses its data into CronTab struct
 */
void cronTabOpener(char fileName[])
{
    FILE* fp = fopen(fileName, "r");
    char mystring[MAX_COMMAND_LEN];
    char* pch;
    int commands = 0;
    int lineCount = 0;
    char lineCpy[MAX_COMMAND_LEN];
    int digCheck;
    char *ptr;
    
    if(fp==NULL)
    {
        fprintf(stderr, "can't open %s: %s\n", fileName, strerror(errno)); // VERY SPECIFIC ERROR
        exit(EXIT_FAILURE);  
    }
    else
    {
        do
        {
            while(fgets(mystring, MAX_COMMAND_LEN, fp) != NULL)
            {
                if(strchr(mystring, '#') != NULL) // strchr instead of using array indexing, it goes through the whole array for you.
                {
                    ++lineCount;
                    continue;
                }
                else
                {
                    ++lineCount; //Have reached a new line so increment the lineCount - used for error output.
                    
                    //Checks that the line isn't just a blank line (as if the first character on the line is "\n" it would be blank.
                    if(strcmp(&mystring[0],"\n") == 0){
                        crontabErrorMessage(lineCount," ");
                    }
                    
                    mystring[strcspn(mystring, "\n")] = 0; //Ensures a string doesn't end with '\n'
                    strcpy(lineCpy,mystring); //A copy of the line before it's modified - used in the case of printing error.
                    
                    //Gets the first element (minute) on the line and checks validity.
                    pch = strtok(mystring," ");
                    digCheck = strtol(pch, &ptr, 10);
                    if(pch != NULL && digCheck >= 0 && digCheck <= 59 && *ptr == '\0')
                    {
                        strcpy(cronCmd[commands].minute, pch);
                    }else { crontabErrorMessage(lineCount, lineCpy); }
                
                    //Gets the second element (hour) on the line and checks validity.
                    pch = strtok(NULL, "   ");
                    digCheck = strtol(pch, &ptr, 10);
                    if(pch != NULL && digCheck >= 0 && digCheck <= 23 && *ptr == '\0')
                    {
                        strcpy(cronCmd[commands].hour, pch);
                    }else { crontabErrorMessage(lineCount, lineCpy); }
                    
                    //Gets the third element (day of month) on the line and checks validity.
                    pch = strtok(NULL, "   ");
                    digCheck = strtol(pch, &ptr, 10);
                    if(pch != NULL && ((digCheck >= 1 && digCheck <= 31 && *ptr == '\0') || strchr(pch, '*') != NULL))
                    {
                        strcpy(cronCmd[commands].dayOfMonth, pch);
                    }else { crontabErrorMessage(lineCount, lineCpy); }
                    
                    //Gets the fourth element (month) on the line and checks validity.
                    pch = strtok(NULL, "   ");
                    digCheck = strtol(pch, &ptr, 10);
                    if(pch != NULL && ((digCheck >= 0 && digCheck <= 11 && *ptr == '\0') ||
                                       (strcmp(pch,"jan") == 0) || (strcmp(pch,"feb") == 0) ||
                                       (strcmp(pch,"mar") == 0) || (strcmp(pch,"apr") == 0) ||
                                       (strcmp(pch,"may") == 0) || (strcmp(pch,"jun") == 0) ||
                                       (strcmp(pch,"jul") == 0) || (strcmp(pch,"aug") == 0) ||
                                       (strcmp(pch,"sep") == 0) || (strcmp(pch,"oct") == 0) ||
                                       (strcmp(pch,"nov") == 0) || (strcmp(pch,"dec") == 0) ||
                                       ((strchr(pch, '*') != NULL) && strcmp(pch,"*") == 0)))
                    {
                        strcpy(cronCmd[commands].month, pch);
                    }else { crontabErrorMessage(lineCount, lineCpy); }
                    
                    //Gets the fifth element (day of week) on the line and checks validity.
                    pch = strtok(NULL, "   ");
                    if((pch != NULL && ((digCheck >= 0 && digCheck <= 6 && ptr != pch) ||
                                        (strcmp(pch,"mon") == 0) || (strcmp(pch,"tue") == 0) ||
                                        (strcmp(pch,"wed") == 0) || (strcmp(pch,"thu") == 0) ||
                                        (strcmp(pch,"fri") == 0) || (strcmp(pch,"sat") == 0) ||
                                        (strcmp(pch,"sun") == 0) || (strchr(pch, '*') != NULL))))
                    {
                        strcpy(cronCmd[commands].dayOfWeek, pch);
                    }else{ crontabErrorMessage(lineCount, lineCpy); }
                    
                    //Gets the last element (commandName) on the line and checks validity.
                    pch = strtok(NULL, "   ");
                    if(pch != NULL)
                    {
                        strcpy(cronCmd[commands].commandName, pch);
                    }else {crontabErrorMessage(lineCount, lineCpy); }
                    
                    //If there is more content in the file then abort as doesn't match format.
                    pch = strtok(NULL, "   ");
                    if(pch != NULL){
                        crontabErrorMessage(lineCount, lineCpy);
                    }
                    
                    ++numOfCrontabCommands; //If the format of the file is correct, proceed and count;
                    ++commands;
                }
            }
        }while (!feof(fp)); // stops at EOF
    }
    fclose(fp);
    checkCommandsExist();
}

/**
 * Function     : first_day_of_month()
 * Parameters   : int month, int year
 * Returns      : tm.tm_wday   
 * Purpose      : Used to find first day of month
 * Reference    : Chris McDonald Lecture Content
 */
int first_day_of_month(int month, int year){
    struct tm  tm;

    memset(&tm, 0, sizeof(tm));

    tm.tm_mday = 1;
    tm.tm_mon = month;
    tm.tm_year = year-1900;

    mktime(&tm);
    
    return tm.tm_wday;
}

/**
 * Function     : daysInMonth()
 * Parameters   : int month
 * Returns      : int   
 * Purpose      : Used to check how many days are in a particular month (i.e Jan = 0 -> days = 31)
 */

int daysInMonth(int month){
    switch (month) {
        case 0: return 31;
        case 1: return 28;
        case 2: return 31;
        case 3: return 30;
        case 4: return 31;
        case 5: return 30;
        case 6: return 31;
        case 7: return 31;
        case 8: return 30;
        case 9: return 31;
        case 10: return 30;
        case 11: return 31;
    }
    return 0;
}

/**
 * Function     : printOutput()
 * Parameters   : (none)
 * Returns      : (none)   
 * Purpose      : Prints the required output in the form -> most used     total invoked       maxRunning 
 */
void printOutput()
{   //Finds the most used command for output.
    char mostUsed[MAX_COMMANDNAME_LEN];
    int max = 0;
    for(int i = 0; i < numOfcommands; ++i){
        if(currCmd[i].invokeCount > max)
        {
            max = currCmd[i].invokeCount;
            strcpy(&mostUsed[0],currCmd[i].commandName);
        }
    }
    printf("%s\t%d\t%d\n",mostUsed,totalInvoked,maxRunning);
    
    for(int i = 0; i < numOfcommands; ++i){
        printf("command: %s ran %d times.\n",currCmd[i].commandName,currCmd[i].invokeCount);
    }
}

/**
 * Function     : simulator()
 * Parameters   : int month
 * Returns      : (none)   
 * Purpose      : Simulates the execution of the tasks and outputs the desired data.
 */
void simulator(int month)
{
    
    /* Check which month it is, which determines the number of days.
     * Leap years are not considered as per the Project outline. */
    int days_in_month = daysInMonth(month);
    int running = 0;    //Keeps track of number of currently running programs.
    int day_of_month = 1; //Keeps track of which day of the month it is.
    int current_day_of_week = first_day_of_month(month, 2022);
    long initialTime;    //Used to calculate finish times.
    long finishTime;     //Stores the finish time of the current command
    long finishHour;
    char *ptr;
    
    //Will go through every day of the month, checking what needs to run at each minute for each hour.
    while(day_of_month <= days_in_month){
        int currHour = 0; //The current hour - resets daily
        
        while(currHour < 23)
        {
  
            for(int minute = 0; minute < 60; minute++)
            {
                
                for(int i = 0; i < numOfCrontabCommands; ++i)
                {
                    
                    if(atoi(cronCmd[i].month) == month || strchr(cronCmd[i].month, '*') != NULL)
                    {
                            //Since 'invokeCount' is in the 'CronCommand' struct we need the index to increment it while we go through the cronTab data.
                            for(int j = 0; j < numOfCrontabCommands; ++j)
                            {
                                /*  If this strcmp() returns 0 the two strings are equal and we have found the jth index we'll need to be able to increment invokeCount.
                                    If we've got the indexes we need -> progess and check if the command should run or terminate. */
                                if(strcmp(currCmd[j].commandName,cronCmd[i].commandName) == 0)
                                {
                                    int commandDay = strtol(cronCmd[i].dayOfWeek,&ptr,10); //Checks which day of the week the current command runs.
                                    char *everyDayCheck = strchr(cronCmd[i].dayOfWeek, '*'); //Checks if current command runs every day
                                    
                                    /* Checks if the current command needs to run, checking all possible inputs for day: 3-char, number and '*',
                                     * also checking if it is supposed to run this minute of this hour, on this date. */
                                    if((
                                      ((strcmp(cronCmd[i].dayOfWeek,"mon") == 0 || (commandDay == 1 && *ptr == '\0') || everyDayCheck != NULL)  && current_day_of_week == 1) ||
                                      ((strcmp(cronCmd[i].dayOfWeek,"tue") == 0 || (commandDay == 2 && *ptr == '\0') || everyDayCheck != NULL)  && current_day_of_week == 2) ||
                                      ((strcmp(cronCmd[i].dayOfWeek,"wed") == 0 || (commandDay == 3 && *ptr == '\0') || everyDayCheck != NULL)  && current_day_of_week == 3) ||
                                      ((strcmp(cronCmd[i].dayOfWeek,"thu") == 0 || (commandDay == 4 && *ptr == '\0') || everyDayCheck != NULL)  && current_day_of_week == 4) ||
                                      ((strcmp(cronCmd[i].dayOfWeek,"fri") == 0 || (commandDay == 5 && *ptr == '\0') || everyDayCheck != NULL)  && current_day_of_week == 5) ||
                                      ((strcmp(cronCmd[i].dayOfWeek,"sat") == 0 || (commandDay == 6 && *ptr == '\0') || everyDayCheck != NULL)  && current_day_of_week == 6) ||
                                      ((strcmp(cronCmd[i].dayOfWeek,"sun") == 0 || (commandDay == 0 && *ptr == '\0') || everyDayCheck != NULL)  && current_day_of_week == 0))&&
                                      (atoi(cronCmd[i].dayOfMonth) == day_of_month || (strchr(cronCmd[i].dayOfMonth, '*') != NULL)) &&
                                      (atoi(cronCmd[i].hour) == currHour) && (atoi(cronCmd[i].minute) == minute) && running < 20)
                                    {
                                        ++running;
                                        ++currCmd[j].invokeCount;
                                        ++totalInvoked;
                                        
                                        //Calculations for the time that the command just 'executed' will finish.
                                        initialTime = (atoi(cronCmd[i].hour)*60) + atol(cronCmd[i].minute);
                                        finishTime = initialTime + atol(currCmd[j].taskDuration);
                                        finishHour = (finishTime / 60);
                                        for(int z = 0; z < 25; ++z){
                                            if(strlen(runningCommands[z].commandName) ==0) //Empty location in array
                                            {
                                                strcpy(runningCommands[z].commandName,cronCmd[i].commandName);
                                                runningCommands[z].finishDay = day_of_month + finishHour / 24;
                                                runningCommands[z].finishHour = finishHour % 24;
                                                runningCommands[z].finishMin = (finishTime % 60);
                                                printf("(%d/%d) Executing:\n@%d:%d | %s running:%d | ETA:'%d' @ %d:%d dow:%s vs cur-dow:%d\ncommandDay:%d\n",day_of_month,month,currHour,minute,runningCommands[z].commandName,running, runningCommands[z].finishDay, runningCommands[z].finishHour,runningCommands[z].finishMin,cronCmd[i].dayOfWeek,current_day_of_week,commandDay);
                                                break;
                                            }
                                        }
                                    }
                                    
                                    /* Checks if a task needs to terminate.
                                     * Iterates over the runningCommands array to see if the current command needs to finish.
                                     * Sets the commandName to nothing so it can be seen as a 'free space' for another command */
                                    for(int check = 0; check < 25; ++check)
                                    {
                                        if(strcmp(runningCommands[check].commandName,cronCmd[i].commandName) == 0 &&
                                            (runningCommands[check].finishDay == day_of_month) &&
                                            (runningCommands[check].finishHour == currHour) &&
                                            (runningCommands[check].finishMin == minute))
                                            {
                                                --running;
                                                printf("(%d/%d) Terminating:\n@%d:%d | %s running:%d | ETA:'%d' @ %d:%d\n\n",day_of_month,month,currHour,minute,runningCommands[check].commandName,running, runningCommands[check].finishDay, runningCommands[check].finishHour,runningCommands[check].finishMin);
                                                strcpy(runningCommands[check].commandName,"");
                                                break;
                                            }
                                            
                                        }
                                    
                                    
                                    /* Check to see if have reached a new maximum for running at once.
                                       Comes after decreasing running because they would've finished right before this current minute began. */
                                    if(running > maxRunning)
                                    {
                                        maxRunning = running;
                                    }
                                }
                            }
                    }else{ continue; }//command didn't need to start or end so move onto next command.
                }
            }
            ++currHour; //Reached end of hour, move onto next.
        }
        ++day_of_month; //Reached the end of the day, move onto next.
        ++current_day_of_week; //Reached end of day, increment the current day of the week and check if we are at the end of the week too.
        if(current_day_of_week > 6){
            current_day_of_week = 0 ;
        }
    }
    printOutput();
}

/**
 * Function     : simulator()
 * Parameters   : int month
 * Returns      : (none)   
 * Purpose      : -> month variable is a 3 digit character that is a month
*                            -> checks if month variable is an int between 0 to 11
*/
int returnMonth(char *month)
{
    int monthVar = 0;

    char *ptr;
    monthVar = strtol(month, &ptr, 10);
    
    if(strcmp(month, "jan") == 0 ){ monthVar = 0; }
    else if(strcmp(month, "feb") == 0 ){ monthVar = 1; }
    else if(strcmp(month, "mar") == 0 ){ monthVar = 2; } 
    else if(strcmp(month, "apr") == 0 ){ monthVar = 3; }
    else if(strcmp(month, "may") == 0 ){ monthVar = 4; }
    else if(strcmp(month, "jun") == 0 ){ monthVar = 5; }
    else if(strcmp(month, "jul") == 0 ){ monthVar = 6; }
    else if(strcmp(month, "aug") == 0 ){ monthVar = 7; }
    else if(strcmp(month, "sep") == 0 ){ monthVar = 8; }
    else if(strcmp(month, "oct") == 0 ){ monthVar = 9; }
    else if(strcmp(month, "nov") == 0 ){ monthVar = 10; }
    else if(strcmp(month, "dec") == 0 ){ monthVar = 11; }
    else if(monthVar >= 0 && monthVar <= 11  && errno != EINVAL)
    {
        return monthVar;
    }
    else
    {
        printf("Invalid month argument.\n");
        exit(EXIT_FAILURE);
    }

    return monthVar;
}


int main(int argc, char *argv[])
{
    //Ensures there are 4 inputs, else exit and print error message.
    if(argc != 4)
    {
        printf("Invalid argument count.\nUsage: ./estimatecron.c <month> <estimates-file> <crontab-file>\n");
        exit(EXIT_FAILURE);
    }

    // VALIDATION OF MONTH INPUT - either an integer 0..11, or a valid 3-character name
    int month = returnMonth(argv[1]);
 
    //Calls function to read estimates-file. Param is the file name.
    estimatesOpener(argv[3]);
    //Calls function to read crontab-file. Param is the file name
    cronTabOpener(argv[2]);
    //Call to the simulator function to calculate the three desired outputs.
    simulator(month);
        
    return 0;
}
    
