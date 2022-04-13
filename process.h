/**
 *  process.h
 *
 *  Full Name: Lucas Aguiar
 *  Course section: Section M
 *  Representation of a process in the system.
 *  
 */


#ifndef PROCESS_H
#define PROCESS_H

// representation of a a process
typedef struct Process {
    int  pid;
    int  cpu_time;
    int  io_time;  
    int  arrival_time;
    int  status;
    int  time_ran;
    int  turnaround_time;
} Process;

#endif
