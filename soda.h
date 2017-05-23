#ifndef _SODA_H__
#define _SODA_H__

#include <queue>
#include <uFuture.h>

//forward declarations
_Task NameServer;
_Monitor Printer;
_Task WATCardOffice;
_Task Groupoff;
_Monitor Bank;

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

class WATCard {
    unsigned int balance;

    WATCard( const WATCard & );               // prevent copying
    WATCard &operator=( const WATCard & );
  public:
    typedef Future_ISM<WATCard *> FWATCard;   // future watcard pointer
    WATCard();
    void deposit( unsigned int amount );
    void withdraw( unsigned int amount );
    unsigned int getBalance();
};

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

_Monitor Bank {
    unsigned int numStudents;

    unsigned int* accounts; // each student account
    uCondition* accountlk;  // couriers wait here if there is not enough money in the account for a specified student
  public:
    Bank( unsigned int numStudents );
    ~Bank();
    void deposit( unsigned int id, unsigned int amount );
    void withdraw( unsigned int id, unsigned int amount );
};

_Task Parent {
    Printer &printer;
    Bank &bank;
    unsigned int numStudents;
    unsigned int parentalDelay;

    void main();
  public:
    Parent( Printer &prt, Bank &bank, unsigned int numStudents, unsigned int parentalDelay );
};

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

_Task BottlingPlant {
    Printer &printer;
    NameServer &nameServer;
    unsigned int numVendingMachines;
    unsigned int maxShippedPerFlavour;
    unsigned int maxStockPerFlavour;
    unsigned int timeBetweenShipments;

    unsigned int producedSodas[VendingMachine::NUM_FLAVOURS]; // cargo that is copied into truck
    bool isClosing; // used to indicate to the truck that the plant is shutting down

    void produceSodas();    // production method
    void main();
  public:
    _Event Shutdown {};                       // shutdown plant
    BottlingPlant( Printer &prt, NameServer &nameServer, unsigned int numVendingMachines,
                 unsigned int maxShippedPerFlavour, unsigned int maxStockPerFlavour,
                 unsigned int timeBetweenShipments );
    void getShipment( unsigned int cargo[] );
};

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

_Monitor Printer {
  public:
    enum Kind { Parent, Groupoff, WATCardOffice, NameServer, Truck, BottlingPlant, Student, Vending, Courier };
    Printer( unsigned int numStudents, unsigned int numVendingMachines, unsigned int numCouriers );
    ~Printer();
    void print( Kind kind, char state );
    void print( Kind kind, char state, int value1 );
    void print( Kind kind, char state, int value1, int value2 );
    void print( Kind kind, unsigned int lid, char state );
    void print( Kind kind, unsigned int lid, char state, int value1 );
    void print( Kind kind, unsigned int lid, char state, int value1, int value2 );

  private:
    unsigned int numStudents;
    unsigned int numVendingMachines;
    unsigned int numCouriers;
    unsigned int numTotalTasks;

    struct Action {
        Kind kind;
        char state;
        bool hasAction; // indicates if this action exists to prevent printing
        int val1;
        int val2;
    };

    Action* actions;

    bool emptyFlush; // true when a finish 'F' has occured and no new states are stored
    void flush();
    void printMain(unsigned int id, char state, int value1, int value2); // other print methods pipeline to this
    
    // below methods help with identifying which column the task will be printed at
    unsigned int getRealId(Kind kind);
    unsigned int getRealId(Kind kind, unsigned int lid);
};

#endif
