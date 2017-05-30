#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <stddef.h>
#include "post.h"
#include "comment.h"
#include "out_manager.h"
#include "utils.h"
#include "types.h"

#define OUT_TUPLE_ITEMS 5
/* create a type for struct out_tuple */


MPI_Datatype serialize_out_tuple()
{
    int blocklengths[OUT_TUPLE_ITEMS] = {1,1,1,1,1};
    MPI_Datatype types[OUT_TUPLE_ITEMS] = {MPI_LONG, MPI_LONG, MPI_LONG, MPI_INT, MPI_INT};
    MPI_Datatype MPI_new_type;
    MPI_Aint offsets[OUT_TUPLE_ITEMS];
    offsets[0] = offsetof(out_tuple, ts);
    offsets[1] = offsetof(out_tuple, post_id);
    offsets[2] = offsetof(out_tuple, user_id);
    offsets[3] = offsetof(out_tuple, num_commenters);
    offsets[4] = offsetof(out_tuple, score);
    MPI_Type_create_struct(OUT_TUPLE_ITEMS, blocklengths, offsets, types, &MPI_new_type);
    MPI_Type_commit(&MPI_new_type);
    return MPI_new_type;
}
