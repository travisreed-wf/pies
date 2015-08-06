#!/bin/bash

load_type=cpu
if [ -n "$LOAD_TYPE" ]; then
   load_type=$LOAD_TYPE
fi

# CPU args
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

threads=1
if [ -n "$THREADS" ]; then
   threads=$THREADS
fi

# Memory args
mem_start_arg=
if [ -n "$MEM_START" ]; then
   mem_start_arg="-s $MEM_START"
fi

mem_inc_arg=
if [ -n "$MEM_INC" ]; then
   mem_inc_arg="-i $MEM_INC"
fi

mem_interval_arg=
if [ -n "$MEM_INTERVAL" ]; then
   mem_interval_arg="-n $MEM_INTERVAL"
fi

# Debug arg
debug_arg=
if [ -n "$DEBUG" ]; then
   debug_arg=-d
fi

if [ "$load_type" = "cpu" ]; then
   i=0
   while [ $i -lt $threads ]; do
      /usr/local/bin/load $workload_arg $arrival_arg $count_arg $seed_arg $debug_arg &
      i=$(($i + 1))
   done
   wait
elif [ "$load_type" = "mem" ]; then
   /usr/local/bin/memload $mem_starg_arg $mem_inc_arg $mem_interval_arg $debug 
else
   echo "Error: Unknown load_type '$load_type'"
   exit 1
fi

echo "Done"
exit 0
