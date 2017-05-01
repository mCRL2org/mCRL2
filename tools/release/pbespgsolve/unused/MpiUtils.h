// Copyright (c) 2009-2013 University of Twente
// Copyright (c) 2009-2013 Michael Weber <michaelw@cs.utwente.nl>
// Copyright (c) 2009-2013 Maks Verver <maksverver@geocities.com>
// Copyright (c) 2009-2013 Eindhoven University of Technology
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef MPI_UTILS_INCLUDED
#define MPI_UTILS_INCLUDED

/*! \file MpiUtils.h

    Common routines which are useful for implementing distributed algorithms
    using MPI.  They are used by the MpiRecursiveSolver, MpiSpmSolver and the
    MpiAttractorAlgorithm classes.
*/

#include <mpi.h>

/*! MPI process rank. */
extern int mpi_rank;

/*! Number of MPI processes.

    This is zero if MPI is not initialized; in that case, none of the routines
    defined here may be called!
*/
extern int mpi_size;

//! Returns whether `local_value` is true in all of the MPI processes:
bool mpi_and(bool local_value);

//! Returns whether `local_value` is true in any of the MPI processes:
bool mpi_or(bool local_value);

//! Returns the sum of local values of all MPI processes:
int mpi_sum(int local_value);



/*! Termination detection using Friedemann Mattern's four-counter method.

    All processes keep track of the number of sent and received messages. When
    idle, the first process sends a probe that is circulated to other processes
    when they are idle, accumulating the total number of messages sent and
    received. This has to be done twice in order to confirm global termination,
    at which point the first process sends a termination signal to the other
    processes.

    All communication currently happens in the world communicator.
*/
class MpiTermination
{
public:
    /*! Construct an instance of the termination detection algorithm.
        It uses two tags for control messages (probe_tag and term_tag) and
        recognizes one tag used for application-specific data messages
        (data_tag).

        Additionally, the data count, type and a receive buffer for data
        messages must be provided.

        All worker processes start out as active processes, but may become idle
        over time. Whenever a data message is received by a process, it becomes
        active (non-idle) again. The purpose of this class is to detect the
        condition where all processes are idle and all data messages that were
        sent have been received by the corresponding processes.
    */
    MpiTermination(
        int data_count, const MPI::Datatype &data_type, void *data_buf,
        int probe_tag = 1, int term_tag = 2, int data_tag = 3 );

    ~MpiTermination();

    /*! Start listening for data messages. This invalidates the contents of the
        data buffer. May be called by the worker process to start listening for
        incoming data messages before calling recv() or test(). */
    void start();

    /*! Call this to indicate the current worker process is idle. If a data
        message is received while the process is idle, the process is
        automatically marked active again. This call is usually followed by
        a call to recv() to wait for new data or conformation of termination. */
    void idle();

    /*! Blocks while waiting for more data to arrive. Returns `true` if a data
        message has been received, which must be handled by the application.
        The data buffer is valid until the next call to recv() or test().
        Returns false if global termination has been dected. */
    bool recv();

    /*! Tests whether more data is available yet. Returns `true` if a data
        message has been received, which must be handled by the application.
        The data buffer is valid until the next call to recv() or test().
        Returns `false` if no data is available yet. In either case, the method
        does not block. */
    bool test();

    /*! Blocking send of data message.
        The tag and type are equal those passed in the constructor. */
    void send(void *buf, int count, int dest);

protected:
    int probe_tag_, term_tag_, data_tag_;   //!< tags used to identify messages
    int data_count_;             //!< number of elements in data receive buffer
    MPI::Datatype data_type_;                  //!< data type for data messages
    void *data_buf_;                      //!< receive buffer for data messages
    int num_send_, num_recv_;     //!< # of data messages sent/received locally
    int tot_send_, tot_recv_;    //!< # of data messages sent/received globally
    MPI::Prequest reqs_[3];                 //! asynchronous request structures
    bool data_started_;        //! indicates whether the data request is active
    bool idle_;               //! indicates whether the current process is idle
    bool terminated_;        //! indicates whether termination has been reached
    int  probe_val_[2];                        //! probe message receive buffer
    bool send_probe;   //! indicates whether this process must initiate probing
};

#endif /* ndef MPI_UTILS_INCLUDED */
