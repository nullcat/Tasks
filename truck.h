#ifndef __TRUCK_H__
#define __TRUCK_H__

#include "printer.h"
#include "nameserver.h"
#include "bottlingplant.h"
#include "config.h"

_Task BottlingPlant;

_Task Truck {
    Printer &printer;
    NameServer &nameServer;
    BottlingPlant &plant;
    unsigned int numVendingMachines;
    unsigned int maxStockPerFlavour;
    unsigned int cargo[VendingMachine::NUM_FLAVOURS]; // sodas on truck by flavour

    void main();
  public:
    Truck( Printer &prt, NameServer &nameServer, BottlingPlant &plant,
           unsigned int numVendingMachines, unsigned int maxStockPerFlavour );
};

#endif
