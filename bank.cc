#include "bank.h"

Bank::Bank( unsigned int numStudents ) : numStudents(numStudents) {
  accounts = new unsigned int [numStudents];
  for (unsigned int i = 0; i < numStudents; i++) {
    accounts[i] = 0;  // initialize to 0 balance
  }

  accountlk = new uCondition [numStudents];
}

Bank::~Bank() {
  delete [] accountlk;
  delete [] accounts;
}

// parent calls this
void Bank::deposit( unsigned int id, unsigned int amount ) {
  accounts[id] += amount;
  accountlk[id].signal(); // signal account that there may be enough funds to withdraw
}

// courier calls this to withdraw on behalf of WATCardOffice
void Bank::withdraw( unsigned int id, unsigned int amount ) {
  while (accounts[id] < amount) {
    accountlk[id].wait();

    // signal if there are other couriers waiting
    if(!accountlk[id].empty()) {  // does this case occur?
      accountlk[id].signal();
    }
  }

  accounts[id] -= amount;
}
