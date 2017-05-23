#include "truck.h"
#include "MPRNG.h"

extern MPRNG mprng;

Truck::Truck( Printer &prt, NameServer &nameServer, BottlingPlant &plant, unsigned int numVendingMachines, unsigned int maxStockPerFlavour ) :
printer(prt), nameServer(nameServer), plant(plant), numVendingMachines(numVendingMachines), maxStockPerFlavour(maxStockPerFlavour)
{}

void Truck::main() {
	VendingMachine** machineList;

	printer.print(Printer::Truck, 'S');
	machineList = nameServer.getMachineList();

	// initialize empty cargos
	for (int i = 0; i < VendingMachine::NUM_FLAVOURS; i++) {
		cargo[i] = 0;
	}

	try {
		// for fairness, cycle starting at the vendine machine after the last restocked
		unsigned int curMachine = 0;
		
		for(;;) {
			yield(mprng(1, 10));

			plant.getShipment(cargo);

			int sodasInShipment = 0;
			for (int i = 0; i < VendingMachine::NUM_FLAVOURS; i++) {
				sodasInShipment += cargo[i];
			}

			printer.print(Printer::Truck, 'P', sodasInShipment); // received shipment

			for (unsigned int i = 0; i < numVendingMachines && sodasInShipment != 0; i++) {
				int machineId = machineList[curMachine]->getId();
				printer.print(Printer::Truck, 'd', machineId, sodasInShipment); // begin distributing shipment to machines

				unsigned int *machineInventory = machineList[curMachine]->inventory();
				
				int bottlesNotReplenished = 0;
				for (unsigned int j = 0; j < VendingMachine::NUM_FLAVOURS; j++) {
					// amount of sodas unfilled for this flavour
					int amountUnfilled = maxStockPerFlavour - machineInventory[j];
					int transferAmount;	// amount to transfer to machine for this flavour
					if (amountUnfilled > cargo[j]) {	// cargo cannot completely fill this flavour
						transferAmount = cargo[j];
					} else {
						transferAmount = amountUnfilled;
					}

					cargo[j] -= transferAmount;
					machineInventory[j] += transferAmount;						
					sodasInShipment -= transferAmount;

					// amount unfilled after stocking
					bottlesNotReplenished += maxStockPerFlavour - machineInventory[j];
				}

				if (bottlesNotReplenished > 0) {
					printer.print(Printer::Truck, 'U', machineId, bottlesNotReplenished);
				}

				machineList[curMachine]->restocked();
				printer.print(Printer::Truck, 'D', machineId, sodasInShipment);

				curMachine = (curMachine + 1) % numVendingMachines;	// cycle, ensure fairness
			}
		}
	} catch (BottlingPlant::Shutdown) {
		printer.print(Printer::Truck, 'F');
	}
}
