HOW TO RUN
first type make
then type ./scheduler <filename> <alg-number>

The algorithm number can be 0, 1 or 2, denoting the following 3 scheduling algorithms:
0 - First Come First Serve
1 - Round Robin
2 - Shortest Job First

Assignment uses the following files:

*** Scheduler.c ***

Description: reads the provided file and schedules processes based on the algorithm number provided
Dependencies: stdio.h, stdlib.h, string.h, stdbool.h, math.h, queue.h, scheduler.h, process.h

*** Scheduler.h ***
Description: Defines the QUANTUM
Dependencies: None

*** Process.h ***
Description: Defines the representation for the process struct
Dependencies: None

*** Queue.h ***
Description: Defines a queue representation
Dependencies: stdbool.h, process.h

*** Queue.c ***
Description: Operations that the struct uses (like poll, offer, etc)
Dependencies: stdlib.h, stdio.h, string.h, queue.h, process.h

