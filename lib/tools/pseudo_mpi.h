// pseudo_mpi.h
#ifndef PSEUDO_MPI_H
#define PSEUDO_MPI_H

#include <string.h>
#include <stdlib.h>

// Constants
#define MPI_COMM_WORLD 0
#define MPI_SUCCESS 0
#define MPI_INT 1
#define MPI_DOUBLE 2
#define MPI_CHAR 3
#define MPI_SUM 1
#define MPI_MAX 2
#define MPI_MIN 3
#define MPI_ANY_SOURCE -1
#define MPI_ANY_TAG -1

// Types
typedef int MPI_Comm;
typedef int MPI_Datatype;
typedef int MPI_Op;
typedef int MPI_Request;

typedef struct { 
    int MPI_SOURCE; 
    int MPI_TAG; 
    int MPI_ERROR;
    int _count;
    int _cancelled;
} MPI_Status;

// Basic MPI functions
inline int MPI_Init(int *argc, char ***argv) { return MPI_SUCCESS; }
inline int MPI_Finalize(void) { return MPI_SUCCESS; }
inline int MPI_Comm_rank(MPI_Comm comm, int *rank) { *rank = 0; return MPI_SUCCESS; }
inline int MPI_Comm_size(MPI_Comm comm, int *size) { *size = 1; return MPI_SUCCESS; }
inline int MPI_Barrier(MPI_Comm comm) { return MPI_SUCCESS; }

// Communication functions - single process means no actual communication
inline int MPI_Send(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm) { return MPI_SUCCESS; }
inline int MPI_Recv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status *status) { 
    if (status) { 
        status->MPI_SOURCE = 0; 
        status->MPI_TAG = tag; 
    }
    return MPI_SUCCESS; 
}
inline int MPI_Isend(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request) { 
    return MPI_SUCCESS; 
}
inline int MPI_Iprobe(int source, int tag, MPI_Comm comm, int *flag, MPI_Status *status) { 
    *flag = 0; // No messages available in single process
    if (status) {
        status->MPI_SOURCE = 0;
        status->MPI_TAG = 0;
        status->MPI_ERROR = MPI_SUCCESS;
        status->_count = 0;
        status->_cancelled = 0;
    }
    return MPI_SUCCESS; 
}
inline int MPI_Get_count(const MPI_Status *status, MPI_Datatype datatype, int *count) { 
    *count = 0; 
    return MPI_SUCCESS; 
}
inline int MPI_Cancel(MPI_Request *request) { 
    // No-op for single process
    return MPI_SUCCESS; 
}
inline int MPI_Wait(MPI_Request *request, MPI_Status *status) { 
    if (status) {
        status->MPI_SOURCE = 0;
        status->MPI_TAG = 0;
    }
    return MPI_SUCCESS; 
}
inline int MPI_Test(MPI_Request *request, int *flag, MPI_Status *status) { 
    *flag = 1; 
    if (status) {
        status->MPI_SOURCE = 0;
        status->MPI_TAG = 0;
    }
    return MPI_SUCCESS; 
}
inline int MPI_Sendrecv(const void *sendbuf, int sendcount, MPI_Datatype sendtype, int dest, int sendtag, 
                 void *recvbuf, int recvcount, MPI_Datatype recvtype, int source, int recvtag, 
                 MPI_Comm comm, MPI_Status *status) {
    // Single process: copy from sendbuf to recvbuf (self-communication)
    if (sendbuf && recvbuf && sendcount > 0 && recvcount > 0) {
        size_t send_size = sendcount;
        size_t recv_size = recvcount;
        if (sendtype == MPI_INT) send_size *= sizeof(int);
        else if (sendtype == MPI_DOUBLE) send_size *= sizeof(double);
        if (recvtype == MPI_INT) recv_size *= sizeof(int);
        else if (recvtype == MPI_DOUBLE) recv_size *= sizeof(double);
        
        size_t copy_size = (send_size < recv_size) ? send_size : recv_size;
        memcpy(recvbuf, sendbuf, copy_size);
    }
    if (status) { status->MPI_SOURCE = 0; status->MPI_TAG = recvtag; }
    return MPI_SUCCESS;
}

// Collective operations
inline int MPI_Bcast(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm) { return MPI_SUCCESS; }
inline int MPI_Allreduce(const void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm) {
    // For single process, just copy if different buffers
    if (sendbuf != recvbuf && sendbuf != NULL && recvbuf != NULL) {
        size_t size = count;
        if (datatype == MPI_INT) size *= sizeof(int);
        else if (datatype == MPI_DOUBLE) size *= sizeof(double);
        memcpy(recvbuf, sendbuf, size);
    }
    return MPI_SUCCESS;
}

#endif
