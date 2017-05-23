#ifndef __PRINTER_H__
#define __PRINTER_H__

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
