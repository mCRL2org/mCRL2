#ifndef PROCESS_STATUS_H
#define PROCESS_STATUS_H

typedef enum {Completed, Aborted} ProcessStatus;

class ExecutionException {
};

class ExecutionError : public ExecutionException {
};

class ExecutionDelayed : public ExecutionException {
};

#endif
