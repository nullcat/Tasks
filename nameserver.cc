#include "nameserver.h"

NameServer::NameServer( Printer &prt, unsigned int numVendingMachines, unsigned int numStudents ) :
	printer(prt), numStudents(numStudents), numMachines(numVendingMachines)
{
	machineList = new VendingMachine*[numMachines];
	studentAssignments = new unsigned int[numStudents];
}

void NameServer::main() {
	printer.print(Printer::NameServer, 'S');

	for(unsigned int i = 0; i < numMachines; i++) {
		_Accept(VMregister) { // register all machines
			machineList[i] = curMachine;
			printer.print(Printer::NameServer, 'R', curMachine->getId());
		}
	}

	// distribute students evenly across machines in round-robin fashion
	for (unsigned int i = 0; i < numStudents; i++) {
		studentAssignments[i] = i % numMachines;
	}

	while (true) {
		_Accept(~NameServer) {
			break;
		} or _Accept(getMachineList) {  // truck priority
		} or _Accept(getMachine) {
			studentAssignments[curStudent] = (studentAssignments[curStudent]+1) % numMachines;

			printer.print(Printer::NameServer, 'N', curStudent, curMachine->getId());
		}
	}

	printer.print(Printer::NameServer, 'F');
}

void NameServer::VMregister( VendingMachine *vendingmachine ) {
	curMachine = vendingmachine;
}

VendingMachine *NameServer::getMachine( unsigned int id ) {
	// store values to move student assignment logic to main
	curStudent = id;
	curMachine = machineList[studentAssignments[id]];

	return curMachine;
}

VendingMachine **NameServer::getMachineList() {
	return machineList;
}

NameServer::~NameServer() {
	delete [] machineList;
	delete [] studentAssignments;
}
