// Copyright (c) 2009-2013 University of Twente
// Copyright (c) 2009-2013 Michael Weber <michaelw@cs.utwente.nl>
// Copyright (c) 2009-2013 Maks Verver <maksverver@geocities.com>
// Copyright (c) 2009-2013 Eindhoven University of Technology
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "MpiUtils.h"
#include <assert.h>

int mpi_rank, mpi_size;

bool mpi_and(bool local_value)
{
    int val = local_value, res = 0;
    MPI::COMM_WORLD.Allreduce(&val, &res, 1, MPI_INT, MPI_LAND);
    return res;
}

bool mpi_or(bool local_value)
{
    int val = local_value, res = 0;
    MPI::COMM_WORLD.Allreduce(&val, &res, 1, MPI_INT, MPI_LOR);
    return res;
}

int mpi_sum(int local_value)
{
    int global_sum = 0;
    MPI::COMM_WORLD.Allreduce(&local_value, &global_sum, 1, MPI_INT, MPI_SUM);
    return global_sum;
}


MpiTermination::MpiTermination(
        int data_count, const MPI::Datatype &data_type, void *data_buf,
        int probe_tag, int term_tag, int data_tag )
    : probe_tag_(probe_tag), term_tag_(term_tag), data_tag_(data_tag),
      data_count_(data_count), data_type_(data_type),
      num_send_(0), num_recv_(0), tot_send_(0), tot_recv_(0),
      data_started_(false), idle_(false), terminated_(false),
      send_probe(mpi_rank == 0)
{
    reqs_[0] = MPI::COMM_WORLD.Recv_init( probe_val_, 2, MPI_INT,
                                          MPI::ANY_SOURCE, probe_tag_);

    reqs_[1] = MPI::COMM_WORLD.Recv_init( NULL, 0, MPI_INT, 0, term_tag_ );

    reqs_[2] = MPI::COMM_WORLD.Recv_init( data_buf, data_count_, data_type_,
                                          MPI::ANY_SOURCE, data_tag_ );

    MPI::Prequest::Startall(3, reqs_);
    data_started_ = true;
}

MpiTermination::~MpiTermination()
{
    assert(idle_);
    assert(terminated_);
    for (int i = 0; i < 3; ++i)
    {
        reqs_[i].Cancel();
        reqs_[i].Wait();
        reqs_[i].Free();
    }
}

void MpiTermination::start()
{
    if (!data_started_)
    {
        reqs_[2].Start();
        data_started_ = true;
    }
}

void MpiTermination::idle()
{
    idle_ = true;
}

bool MpiTermination::test()
{
    start();
    if (reqs_[2].Test())
    {
        idle_ = false;
        data_started_ = false;
        ++num_recv_;
        return true;
    }
    return false;
}

bool MpiTermination::recv()
{
    assert(idle_);
    if (terminated_) return false;
    start();
    if (send_probe)
    {
        // With less than two processes, terminate immediately:
        if (mpi_size < 2)
        {
            terminated_ = true;
            return false;
        }

        // Start a probe:
        int probe[2] = { 0, 0 };
        MPI::COMM_WORLD.Send(probe, 2, MPI_INT, 1, probe_tag_);
        send_probe = false;
    }

    for (;;)
    {
        switch (MPI::Request::Waitany(3, reqs_))
        {
        case 0:  // Probe message received
            probe_val_[0] += num_send_;
            probe_val_[1] += num_recv_;
            if (mpi_rank == 0)  // first process checks for termination
            {
                if (probe_val_[0] == tot_recv_)
                {
                    // Termination detected!
                    assert(probe_val_[0] == probe_val_[1]);
                    assert(tot_send_ == tot_recv_);
                    for (int i = 1; i < mpi_size; ++i)
                    {
                        MPI::COMM_WORLD.Send(NULL, 0, MPI_INT, i, term_tag_);
                    }
                    terminated_ = true;
                    return false;
                }
                else
                {
                    // Not yet terminated; resend probe.
                    tot_send_ = probe_val_[0];
                    tot_recv_ = probe_val_[1];
                    probe_val_[0] = 0;
                    probe_val_[1] = 0;
                    MPI::COMM_WORLD.Send(probe_val_, 2, MPI_INT, 1, probe_tag_);
                }
            }
            else  // mpi_rank > 0
            {
                // Forward probe to next process
                int dest = mpi_rank + 1 == mpi_size ? 0 : mpi_rank + 1;
                MPI::COMM_WORLD.Send(probe_val_, 2, MPI_INT, dest, probe_tag_);
            }
            reqs_[0].Start();
            break;

        case 1:  // Termination message received from root process.
            assert(mpi_rank > 0);
            terminated_ = true;
            return false;

        case 2:  // Data message received
            ++num_recv_;
            idle_ = false;
            data_started_ = false;
            return true;

        default:
            assert(0);  // should never get here.
            break;
        }
    }
    assert(0);  // should never get here.
}

void MpiTermination::send(void *buf, int count, int dest)
{
    ++num_send_;
    MPI::COMM_WORLD.Send(buf, count, data_type_, dest, data_tag_);
}
