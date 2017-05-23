#include "watoffice.h"
#include "MPRNG.h"

extern MPRNG mprng;

void WATCardOffice::main() {
	printer.print(Printer::WATCardOffice, 'S');

	while(true) {
		
		_Accept(~WATCardOffice) {
			isClosing = true;

			// allow each courier to end
			for (unsigned int i = 0; i < numCouriers; i++) {
				_Accept(requestWork);
			}

			break;
		} or _When (jobQueue.size() > 0)  _Accept(requestWork) {
			printer.print(Printer::WATCardOffice, 'W');
		} or _Accept(create) {
			printer.print(Printer::WATCardOffice, 'C', newJob->args.studentId, newJob->args.amount);
			jobQueue.push(newJob);
		} or _Accept(transfer) {
			printer.print(Printer::WATCardOffice, 'T', newJob->args.studentId, newJob->args.amount);
			jobQueue.push(newJob);
		}
	}

	printer.print(Printer::WATCardOffice, 'F');
}

WATCardOffice::WATCardOffice( Printer &prt, Bank &bank, unsigned int numCouriers ) :
	printer(prt), bank(bank), numCouriers(numCouriers),
	isClosing(false), newJob(NULL)
{
	couriers = new Courier*[numCouriers];
	for (unsigned int i = 0; i < numCouriers; i++) {
		couriers[i] = new Courier(i, *this, printer, bank);
	}
}

WATCardOffice::~WATCardOffice() {
	for (unsigned int i = 0; i < numCouriers; i++) {
		delete couriers[i];
	}

	delete [] couriers;
}

WATCard::FWATCard WATCardOffice::create( unsigned int studentId, unsigned int amount ) {
	return transfer(studentId, amount, new WATCard());
}

WATCard::FWATCard WATCardOffice::transfer( unsigned int studentId, unsigned int amount, WATCard *card ) {
	newJob = new Job(Args(studentId, amount, card));

	return newJob->result;
}

WATCardOffice::Job *WATCardOffice::requestWork() {
	if (isClosing) {
		return NULL;	// indicates office's destructor has been called
	}

	Job* assignedJob = jobQueue.front();
	jobQueue.pop();
	return assignedJob;
}

WATCardOffice::Courier::Courier( unsigned int id, WATCardOffice &office, Printer &printer, Bank &bank )
: id(id), office(office), printer(printer), bank(bank) {}

void WATCardOffice::Courier::main() {
	printer.print(Printer::Courier, id, 'S');

	while (true) {
		Job *job = office.requestWork(); // may blocked

		// null is like a sentinel that indicates office's destructor has been called (isClosing)
		if (job == NULL) {
			break;
		}

		Args args = job->args;

		// begin transfer funds
		printer.print(Printer::Courier, id, 't', args.studentId, args.amount);

		// on the way to bank, may lose card
		if (mprng(5) == 0) {	 // 0 - 5 means 1/6 chances to lose ccard
			job->result.exception(new Lost);

			printer.print(Printer::Courier, id, 'L', args.studentId);			
			if (args.card != NULL) {
				delete args.card;
			}
		} else {
			bank.withdraw(args.studentId, args.amount);

			args.card->deposit(args.amount);
			printer.print(Printer::Courier, id, 'T', args.studentId, args.amount);

			job->result.delivery(args.card);
		}

		delete job;
	}

	printer.print(Printer::Courier, id, 'F');
}
