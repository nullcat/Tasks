#ifndef __NAMESERVER_H__
#define __NAMESERVER_H__

#include "printer.h"
#include "vendingmachine.h"

_Task VendingMachine;

_Task NameServer {
    Printer &printer;
    unsigned int numMachines;
    unsigned int numStudents;

    VendingMachine** machineList; // main machine list
    VendingMachine* curMachine; // used to move code into main (truck does less work)
    unsigned int* studentAssignments;
    unsigned int curStudent; // used to move code into main (studnet does less work)

    void main();
  public:
    NameServer( Printer &prt, unsigned int numVendingMachines, unsigned int numStudents );
    ~NameServer();
    void VMregister( VendingMachine *vendingmachine );
    VendingMachine *getMachine( unsigned int id );
    VendingMachine **getMachineList();
};

#endif
