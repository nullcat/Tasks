#ifndef __VENDINGMACHINE_H__
#define __VENDINGMACHINE_H__

#include "printer.h"
#include "nameserver.h"
#include "watcard.h"
#include "config.h"

_Task NameServer;

_Task VendingMachine {
    void main();
  public:
    enum Flavours { CHERRY, CREAM, DIET, ENERGY, NUM_FLAVOURS };                    // flavours of soda (YOU DEFINE)
    _Event Funds {};                          // insufficient funds
    _Event Stock {};                          // out of stock for particular flavour
    VendingMachine( Printer &prt, NameServer &nameServer, unsigned int id, unsigned int sodaCost,
                    unsigned int maxStockPerFlavour );
    void buy( Flavours flavour, WATCard &card );
    unsigned int *inventory();
    void restocked();
    _Nomutex unsigned int cost();
    _Nomutex unsigned int getId();

  private:
    Printer &printer;
    NameServer &nameServer;
    unsigned int id;
    unsigned int sodaCost;
    unsigned int maxStockPerFlavour;

    bool isRestocking;  // prevent calls to buy when restocking
    unsigned int sodaInventory[NUM_FLAVOURS];   // # of sodas for in stock for each flavour
};

#endif
