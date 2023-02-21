//
//  interop2.m
//  CPDevVM
//
//  Created by Bartek Wójcik on 18/11/2022.
//

//#import <Foundation/Foundation.h>
#import "CPDevVM-Swift.h" //plik nagłówkowy projektu

#include <sys/sysctl.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>

#include "vm.h" //odwołanie do pliku z deklaracjami funkcji
#include "vm_data_access.h"


unsigned long long int calcTime;
unsigned long long int r;
unsigned long long int startTime;
//WM_BYTE *pgmData = NULL;
//size_t pgmDataSize = 0;


int VMP_LoadProgramAndData(const char* filename, int datasize) {
    pgmData = realloc(pgmData, datasize);
    if (pgmData == NULL) return -4;
    pgmCode = realloc(pgmCode, datasize);
    FILE* fp = fopen(filename, "rb");
    if(fp == NULL) return -5;
    size_t wczytano = fread(pgmCode, 1, datasize, fp);
    fclose(fp);
    // set initial status of the machine
    wStatus1 = WMSTAT_COLDRESTART;
    // clear cycle counter
    nCycles = 0;
    return 0;    // success
}

void VMP_ReadRTC(WM_DATE_AND_TIME *dt){
    time_t t;
    struct tm* ltime;
    time(&t);
    ltime = localtime(&t);
    dt->date.date.year = ltime->tm_year + 1900;
    dt->date.date.day = ltime->tm_mday;
    dt->date.date.month = ltime->tm_mon+1;
    dt->tod.tod.hour = ltime->tm_hour;
    dt->tod.tod.min = ltime->tm_min;
    dt->tod.tod.sec = ltime->tm_sec;
    dt->tod.tod.csec = 0;
    return;
}

WM_BOOL VMP_WriteRTC(WM_DATE_AND_TIME *dt)
{
    return 0;
}

WM_TIME VMP_CurrentTime(void){
   struct timeval boottime;
   int mib[2] = {CTL_KERN, KERN_BOOTTIME};
   size_t size = sizeof(boottime);
   time_t now;
   time_t uptime = -1;
   (void)time(&now);
   if (sysctl(mib, 2, &boottime, &size, NULL, 0) != -1 && boottime.tv_sec != 0){
       uptime = now - boottime.tv_sec;
       }
   return (WM_TIME)uptime * 1000;
}

WM_REAL VMP_GetRandom(void){
    int l = rand();
    WM_REAL r = ((WM_REAL)l)/RAND_MAX;
    return r;
}



void VMP_PreCycle(void){
   startTime = VMP_CurrentTime();
}

void VMP_PostCycle(void){
    unsigned long long int stopTime, cycleEnd;
    if (task_cycle == 0) return;
    stopTime = VMP_CurrentTime();
    //printf(" Stop at %llu", stopTime);
    cycleEnd = startTime + task_cycle;
    if (stopTime > cycleEnd){
        wStatus1 |= WMSTAT_CYCLEOVERRUN;
        printf("Cycle overrun !\n");
        return;
        }
    else
        wStatus1 &= ~WMSTAT_CYCLEOVERRUN;
        calcTime = stopTime - startTime;
        if (cycleEnd - stopTime > 0) sleep((cycleEnd - stopTime)/1000);
}

void VMP_PreProgram(void){
    // initialize random number generator
     r = VMP_GetRandom();
}
void VMP_PreNextCommand(void){
    //[DummyClass Swift_WMP_PreNextCommand];
}
void VMP_PostProgram(void){
    //[DummyClass Swift_WMP_PostProgram];
}

void VMP_Debug(WM_BYTE err, WM_WORD aux){
    //[DummyClass Swift_WMP_Debug:err Aux:aux];
    NSLog(@"VMP_Debug:: err:%d  aux:%d", err, aux);
}

WM_BYTE VMP_FlashDataRead(WM_ADDRESS StartAddress, WM_WORD Size){
    return 0;
}

WM_BYTE VMP_FlashDataWrite(WM_ADDRESS StartAddress, WM_WORD Size){
    return 0;
}
WM_WORD VMP_FlashDataControl(WM_WORD Command){
    return 0;
}
int VMP_ExecNativeBlock(WM_WORD fb_selector, WM_ADDRESS fb_instance)
{
    return 0;
}

int VMP_InitNativeBlock(WM_WORD fb_selector, WM_ADDRESS fb_instance)
{
    return 0;
}

WM_BYTE getByte__(WM_ADDRESS address){
    return getBYTE(address);
}

void setBool__(WM_ADDRESS address, WM_BOOL v){
    setBYTE(address, v);
}

WM_INT getInt__(WM_ADDRESS address){
    return getINT(address);
}
void setInt__(WM_ADDRESS adress, WM_INT v){
    setINT(adress, v);
}
