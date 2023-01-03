1) dining_deadlock_mutex_1.c

    a) Devise a way to synchronize this wrong code (using sleeps for example)
       so that it appears to work forever, but only because the deadlock
       condition is being starved itself. Implement and explain.

2) dining_starve_1.c

    a) Can this solution cause a livelock? Explain.
    
    b) Implement the livelock situation provided at wikipedia.
       Wait for 10 seconds instead of 10 minutes
        https://en.wikipedia.org/wiki/Dining_philosophers_problem#Problems

3) dining_working_1.c

    a) Explain why this solution is a derivative of the "Resource hierarchy solution"
        https://en.wikipedia.org/wiki/Dining_philosophers_problem#Resource_hierarchy_solution
        
    b) Implement the "Arbitrator solution" and explain your code.
        https://en.wikipedia.org/wiki/Dining_philosophers_problem#Arbitrator_solution
        
    c) Is it easier to solve the problem if philosophers can choose where to sit? Explain! Optionally implement.
    
    d) Is it possible to solve the problem with a queue? Explain! Optionally implement one or both.
        - if philosopher can choose where to sit
        - if philosopher place is predetermined
        
    e) Is it possible to solve the problem by forcing an interval of quiescence?
        For example: if one cannot eat for any reason, then anyone after that must wait until nobody is eating
        Explain how and what are the problems.
        Optionally implement and explain the code.

4) dining_working_2.c

    a) Is this solution related to the Chandy/Misra solution?
        https://en.wikipedia.org/wiki/Dining_philosophers_problem#Chandy/Misra_solution

    b) Implement the Chandy/Misra solution
    
    c) Devise a way to solve the 2nd order starvation problem. Optionally implement the modifications.
    
    d) Does the Chandy/Misra solution has a 2nd order starvation problem.
    
    e) Explain the Chandy/Misra problem with initial condition. Optionally implement.

5) dining_working_eventual_deadlock.c

    a) Devise a way to change this code so that the deadlock
       problem shows up with only a few iterations, without
       changing the overall behaviour. Explain.
       Hint: look at the take_chopstick function? Is this function atomic?
       
    b) There is a trivial way of making this code deadlock very fast.
       What is it?

6) Implemente one of the following:

    a) one of the optional implementations above
    
    b) an unlisted solution, explain the solution,
       show what problems and strengths it might have,
       give references to where you found this solution
       or where you found the base ideas
