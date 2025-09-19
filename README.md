# Multithreaded
Implemented a multithreaded producer-consumer system with 30 producer threads and 50 consumer threads, synchronized using POSIX mutexes and condition variables.
# Approach
The implementation included:
	-Utilizing four ring buffers, each assigned to a distinct donut flavor.
	-Employing 30 producer threads and 50 consumer threads.
	-Coordinating access with mutex locks and condition variables.
	-Modeling deadlock scenarios by adjusting queue size and the number of dozens retrieved.
	-Logging outcomes to assess deadlock likelihood.
	The program leverages four ring buffers, each corresponding to a specific donut flavor.
	The shared data structure, DONUT_SHOP, tracks the status of these buffers, including 	pointers for adding and removing items, counters for available slots and donuts, and a 	serial number tracker for donut identification. Synchronization is accomplished through:
	-Mutex Locks: Safeguard simultaneous access to shared buffers.
	-Condition Variables: Enable producers to pause when buffers are full and consumers to wait 	when buffers are empty.
	-Thread Management: Threads are created dynamically and rejoined after completion.

# Producer Function
	Every producer picks a donut flavor at random and adds a donut to the matching circular list. If the list is full, the producer pauses and waits for a signal

# Consumer Function
	Each consumer retrieves a dozen donuts, ensuring availability by waiting on a variable 	condition when necessary. Selected consumers log their first ten dozen purchases for 	analysis.

# Testing Methodology
The program underwent testing under different conditions to evaluate the likelihood of deadlocks. The primary test parameters included:
	1.Queue Size Variation: The queue capacity ranged from 1500 to 2000 slots to identify the 50% deadlock occurrence threshold.
	2.Consumer Dozen Collection: Tests were conducted with donut collections of 1000, 1500, 2000, 2500, and 3000 dozen to assess deadlock probability.
	3.Deadlock Probability Analysis: The percentage of test runs experiencing deadlocks was recorded for each configuration.
	4.Data Logging: The first 10 dozen donuts collected by five selected consumers were logged as required.

# Results and Analysis
1)Deadlock Probability vs Queue Size
Deadlock Probability (%)
100 |*----*                                  
 90 |     *                                 
 80 |                                         
 70 |        *----*                            
 60 |            *                            
 50 |---------------------------*----*------  (50% Threshold)
 40 |                              *          
    +----------------------------------------
      1500 1600 1700 1800 1900 2000 (Queue Size)

# Observations
	1.Queue Size 1500: Deadlock probability is 100%, indicating severe congestion results in 	complete system failure.
	2.Queue Size 1600: Probability decreases to 90%, reflecting a slight reduction in 	contention.
	3.Queue Size 1700: A sharp drop is observed as deadlock probability falls to 60%.
	4.Queue Size 2000: The lowest probability of 40% suggests that larger queues enhance system 	throughput.

2)Deadlock Probability vs Dozens Collected
Deadlock Probability (%)
100 |                  *----*----*    
 90 |          *------                          
 80 |                                                  
 70 |                                                  
 60 |                                                  
 50 |------*--------------                            (50% Threshold)
 40 |                                                  
 30 |                                                  
 20 |                                                  
 10 |  *                                                
    +----------------------------------------------
      1000 1500 2000 2500 3000 (Dozens Collected)

# Observations
	1.1000 Donuts Collected: Deadlock probability is 10%, indicating minimal resource 	contention.
	2.1500 Donuts Collected: Probability rises sharply to 50%, reaching a critical congestion 	threshold.
	3.2000 Donuts Collected: A drastic increase to 90% suggests severe system overload.
	4.2500+ Donuts Collected: Deadlock probability remains at 100%, meaning failure is 	inevitable beyond this point.
	5.1500–2000 Donuts: A steep incline in probability highlights escalating congestion.
	6.Beyond 2500 Donuts: Deadlock probability is consistently locked at 100%.

The results indicate that queue size affects deadlock probability in a non-linear manner, with larger queues generally reducing deadlocks, though certain anomalies occur. In contrast, the number of donuts collected exhibits an exponential increase in deadlock frequency, with a sharp tipping point around 1500 dozen.
	-Deadlock probability dropped below 50% when queue sizes exceeded approximately 1800 slots.
	-Consumer demand plays a critical role in deadlock occurrence, with risk escalating sharply 	 beyond 2000 dozen collections.
	-While mutex and condition variable mechanisms successfully prevented race conditions, they 	 were ineffective in eliminating deadlocks under high contention.>> Problems Encountered

1.Large Serial Numbers in Output: Consumer threads initially printed unexpectedly large serial numbers. This was resolved by properly initializing and cycling serial numbers within the buffer.
2.Unformatted Consumer Output: The output was adjusted to match the expected format, ensuring a clear representation of donut counts per dozen.
3.Deadlock Detection: The testing script was updated to accurately identify stalled processes and terminate them as needed.

# Conclusion

This study highlights the influence of queue size and consumer demand on deadlock probability in a multithreaded producer-consumer system.
	-Increasing queue size effectively reduces deadlock probability.
	-Higher consumer workload (dozens collected) significantly raises the risk of deadlock.
	-Optimal system performance is achieved with a queue size of 1800 and a moderate workload of 	 1500 dozen collected.
