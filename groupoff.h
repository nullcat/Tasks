#ifndef __GROUPOF_H__
#define __GROUPOF_H__

#include "printer.h"
#include "watcard.h"

_Task Groupoff {
    Printer &printer;
    unsigned int numStudents;
    unsigned int sodaCost;
    unsigned int groupoffDelay;

    WATCard::FWATCard* futureGiftCards; // holds giftcar
    unsigned int cardIndex; // keep track of current operating index for main and giftcard methods

    void main();
  public:
    Groupoff( Printer &prt, unsigned int numStudents, unsigned int sodaCost, unsigned int groupoffDelay );
    ~Groupoff();
    WATCard::FWATCard giftCard();
};

#endif
