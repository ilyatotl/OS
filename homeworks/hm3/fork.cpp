#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>

#include <iostream>

//  g++ fork.cpp -o ./fork
//  ./fork value

int main(int argc, char* argv[]) {
    int n = atoi(argv[1]);
    
    pid_t chpid = fork();
    
    if (chpid == -1) {
    	std::cout << "Error while fork syscall\n";
    } else if (chpid == 0) {
    	// Child process
    	int res = 1;
    	for (int i = 1; i <= n; ++i) {
    	    res *= i;
    	}
    	std::cout << "Child process counted factorial value: " << n << "! = " << res << "\n";
    } else {
    	// parent process
    	int prev = 0, cur = 1;
    	if (n == 0) {
    	    std::cout << "Parent process counted Fibonacci number value: Fib(0) = " << prev << "\n";
    	    exit(0);
    	}
    	for (int i = 2; i <= n; ++i) {
    	    int next = prev + cur;
    	    prev = cur;
    	    cur = next;
    	}
    	std::cout << "Parent process counted Fibonacci number value: Fib(" << n << ") = " << cur << "\n";
    }
}
