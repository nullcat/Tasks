#ifndef _STUDENT_H__
#define _STUDENT_H__

#include "printer.h"
#include "nameserver.h"
#include "watoffice.h"
#include "groupoff.h"

_Task Student {
    Printer &printer;
    NameServer &nameServer;
    WATCardOffice &cardOffice;
    Groupoff &groupoff;
    unsigned int id;
    unsigned int maxPurchases;

    unsigned int numBought; // keeps track of how many sodas student has bought

    void main();
  public:
    Student( Printer &prt, NameServer &nameServer, WATCardOffice &cardOffice, Groupoff &groupoff,
             unsigned int id, unsigned int maxPurchases );
};

#endif
