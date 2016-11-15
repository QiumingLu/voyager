#ifndef VOYAGER_PAXOS_TRANSFER_STATION_H_
#define VOYAGER_PAXOS_TRANSFER_STATION_H_

#include "voyager/port/countdownlatch.h"

class TransferStation {
 public:
  TransferStation();

 private:
  port::CountDownLatch latch_;

  // No copying allowed
  TransferStation(const TransferStation&);
  void operator=(const TransferStation&);
};

#endif  // VOYAGER_PAXOS_TRANSFER_STATION_H_
