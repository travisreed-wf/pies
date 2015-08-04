#!/bin/bash

workload_arg=
if [ -n "$WORKLOAD_TYPE" ]; then
   workload_arg="-w $WORKLOAD_TYPE"
fi

arrival_arg=
if [ -n "$ARRIVAL_RATE" ]; then
   arrival_arg="-l $ARRIVAL_RATE"
fi

count_arg=
if [ -n "$COUNT" ]; then
   count_arg="-c $COUNT"
fi

seed_arg=
if [ -n "$SEED" ]; then
   seed_arg="-s $SEED"
fi

debug_arg=
if [ -n "$DEBUG" ]; then
   debug_arg=-d
fi

/usr/local/bin/load $workload_arg $arrival_arg $count_arg $seed_arg $debug_arg
