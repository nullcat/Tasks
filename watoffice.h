#ifndef __WATOFFICE_H__
#define __WATOFFICE_H__

#include "printer.h"
#include "bank.h"
#include "watcard.h"
#include <queue>

_Task WATCardOffice {
    struct Args {
        unsigned int studentId;
        unsigned int amount;
        WATCard *card;
        Args( unsigned int studentId, unsigned int amount, WATCard *card)
        : studentId( studentId ), amount( amount ), card(card) {}
    };

    struct Job {                              // marshalled arguments and return future
        Args args;                            // call arguments (YOU DEFINE "Args")
        WATCard::FWATCard result;             // return future
        Job( Args args ) : args( args ) {}
    };

    _Task Courier {      // communicates with bank
        WATCardOffice &office;
        Printer &printer;
        Bank &bank;
        unsigned int id;

        void main();
      public:
        Courier( unsigned int id, WATCardOffice &office, Printer &printer, Bank &bank );
    };
    Courier **couriers;

    Printer &printer;
    Bank &bank;
    unsigned int numCouriers;
    bool isClosing; // used for ending courier tasks

    Job* newJob;
    std::queue<Job*> jobQueue;

    void main();
  public:
    _Event Lost {};                           // lost WATCard
    WATCardOffice( Printer &prt, Bank &bank, unsigned int numCouriers );
    ~WATCardOffice();
    WATCard::FWATCard create( unsigned int sid, unsigned int amount );
    WATCard::FWATCard transfer( unsigned int sid, unsigned int amount, WATCard *card );
    Job *requestWork();
};

#endif
