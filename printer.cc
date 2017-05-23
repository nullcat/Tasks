#include <iostream>
#include "printer.h"

using namespace std;

Printer::Printer( unsigned int numStudents, unsigned int numVendingMachines, unsigned int numCouriers ) :
	numStudents(numStudents), numVendingMachines(numVendingMachines), numCouriers(numCouriers),
	emptyFlush(true) {
	
	// there are 6 singular tasks
	numTotalTasks = 6 + numStudents + numVendingMachines + numCouriers;
	actions = new Action[numTotalTasks];

	for (unsigned int i = 0; i < numTotalTasks; i++) {
		actions[i].hasAction = false;
	}

	cout<<"Parent"<<"\t"<<"Gropoff"<<"\t"<<"WATOff"<<"\t"<<"Names"<<"\t"<<"Truck"<<"\t"<<"Plant"<<"\t";
	
	for (unsigned int i = 0; i < numStudents; i++) {
		cout << "Stud" << i << "\t";
	}
	for (unsigned int i = 0; i < numVendingMachines; i++) {
		cout << "Mach" << i << "\t";
	}
	for (unsigned int i = 0; i < numCouriers; i++) {
		cout << "Cour" << i << "\t";
	}

	cout << endl;
	for (unsigned int i = 0; i < numTotalTasks; i++) {
		cout << "*******" << "\t";
	}

	cout << endl;
}

Printer::~Printer () {
	cout << "***********************" << endl;
	delete [] actions;
}

void Printer::flush () {
	if(emptyFlush) {
		return;
	}

	for (unsigned int i = 0; i < numTotalTasks; i++) {
		if (actions[i].hasAction) {
			cout << actions[i].state;
			
			if (actions[i].val1 != -1) {
				cout << actions[i].val1;

				if (actions[i].val2 != -1) {
					cout << ',' << actions[i].val2;
				}
			}

			// reset values
			actions[i].hasAction = false;
			actions[i].state = ' ';
			actions[i].val1 = -1;
			actions[i].val2 = -1;
		}

		if(i == numTotalTasks-1) break;	// prevent printing extra tab

		cout << "\t";
	}

	cout << endl;
	emptyFlush = true;
}

// main print method, public prints pipeline to this one
void Printer::printMain( unsigned int id, char state, int value1, int value2) {
	if (state == 'F') {
		flush();
		
		// finish
		for (unsigned int i = 0; i < numTotalTasks; i++) {
			if (i != id) {
				cout << "...";
			} else {
				cout << 'F';
			}
			
			cout << '\t';
		}

		cout << endl;
		return;
	}

	if (actions[id].hasAction) {
		flush();
	}

	emptyFlush = false; // indicate that there is something to flush
	actions[id].state  = state;
	actions[id].hasAction = true;
	actions[id].val1 = value1;
	actions[id].val2 = value2;
}


// Below are pipeline methods to the main print function

void Printer::print( Kind kind, char state ) {
// NOTE: kind enum corresponds to index in the actions array
	printMain(getRealId(kind), state, -1, -1); 
}

void Printer::print( Kind kind, char state, int value1 ) {
	printMain(getRealId(kind), state, value1, -1);
}
	
void Printer::print( Kind kind, char state, int value1, int value2 ) {
	printMain(getRealId(kind), state, value1, value2);
}

void Printer::print( Kind kind, unsigned int lid, char state ) {
	printMain(getRealId(kind, lid), state, -1, -1);
}

void Printer::print( Kind kind, unsigned int lid, char state, int value1 ) {
	printMain(getRealId(kind, lid), state, value1, -1);
}

void Printer::print( Kind kind, unsigned int lid, char state, int value1, int value2 ) {
	printMain(getRealId(kind, lid), state, value1, value2);
}

// get corresponding id on the printer for singular types
unsigned int Printer::getRealId( Kind kind ) {
	unsigned int offset;
    switch(kind) {
        case Parent:
            offset = 0;
            break;
        case Groupoff:
            offset = 1;
            break;
        case WATCardOffice:
            offset = 2;
            break;
        case NameServer:
            offset = 3;
            break;
        case Truck:
            offset = 4;
            break;
        case BottlingPlant:
            offset = 5;
            break;
        default:
            break;
    }

	return offset;
}


// For tasks with multiple elements, calculate their real ID in the actions array
unsigned int Printer::getRealId( Kind kind, unsigned int lid ) {
	unsigned int offset = 0;
	switch (kind) {
		case Courier:
			offset += numVendingMachines;
		case Vending:
			offset += numStudents;
		case Student:
			offset += 6;	//there are 6 singular tasks
			break;
		default:
			break;
	}

	return offset + lid;
}
