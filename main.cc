#include "config.h"
#include "printer.h"
#include "bank.h"
#include "parent.h"
#include "watoffice.h"
#include "nameserver.h"
#include "vendingmachine.h"
#include "bottlingplant.h"
#include "student.h"

#include "MPRNG.h"

#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;                    // direct access to std
#include <cstdlib>                  // exit

MPRNG mprng;    // global

bool convert( int &val, char *buffer ) {        // convert C string to integer
    std::stringstream ss( buffer );         // connect stream and buffer
    string temp;
    ss >> dec >> val;                   // convert integer from buffer
    return ! ss.fail() &&               // conversion successful ?
        ! ( ss >> temp );               // characters after conversion all blank ?
} // convert

void usage( char *argv[] ) {
    cerr << "Usage: " << argv[0] << "[ config-file [ random-seed (> 0) ] ]" << endl;
    exit( EXIT_FAILURE );               // TERMINATE
} // usage

void uMain::main() {
    int seed = getpid();
    string configFile = "soda.config";

     switch ( argc ) {
      case 3:
        if (!convert(seed, argv[2]) || seed < 1) {
            usage(argv);
        }
      case 2:
        configFile = argv[1];
      case 1:                       // all defaults
        break;
      default:                      // wrong number of options
        usage( argv );
    } // switch

    ConfigParms params;
    processConfigFile(configFile.c_str(), params);

    mprng.set_seed(seed);

    Printer printer(params.numStudents, params.numVendingMachines, params.numCouriers);
    Bank bank(params.numStudents);
    Parent parent(printer, bank, params.numStudents, params.parentalDelay);
    WATCardOffice watoffice(printer, bank, params.numCouriers);
    Groupoff groupoff(printer, params.numStudents, params.sodaCost, params.groupoffDelay);
    NameServer nameserver(printer, params.numVendingMachines, params.numStudents);
    
    VendingMachine *vendingMachines[params.numVendingMachines];
    Student *students[params.numStudents];

    // initialize plant as a pointer because it needs to be manually deleted before vending machines to prevent deadlock
    BottlingPlant *bottlingPlant = new BottlingPlant(printer, nameserver, params.numVendingMachines, params.maxShippedPerFlavour, params.maxStockPerFlavour, params.timeBetweenShipments);

    for (unsigned int i = 0; i < params.numVendingMachines; i++) {
        vendingMachines[i] = new VendingMachine(printer, nameserver, i, params.sodaCost, params.maxStockPerFlavour);
    }

    for (unsigned int i = 0; i < params.numStudents; i++) {
        students[i] = new Student(printer, nameserver, watoffice, groupoff, i, params.maxPurchases);
    }

    // delete students first
    for (unsigned int i = 0; i < params.numStudents; i++) {
        delete students[i];
    }

    // avoid deadlock by deleting the plant before vendingMachines to allow truck to complete its final deliveries
    delete bottlingPlant;
    for (unsigned int i = 0; i < params.numVendingMachines; i++) {
        delete vendingMachines[i];
    }
}
