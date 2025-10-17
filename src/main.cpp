#include <iostream>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <csignal>
#include <fcntl.h>
#include <poll.h>
#include <cstdlib>
#include "../include/Server.hpp"
#include "../include/utils.hpp"

// Global server pointer - needed for signal handler access
static Server *g_server = NULL;

/*
 * volatile: Prevents compiler optimization - variable can change unexpectedly
 * sig_atomic_t: Guaranteed atomic operations - safe for signal handlers
 * Combined: Safe shared variable between signal handler and main program
 */
static volatile sig_atomic_t g_shutdown_requested = 0;

/**
 * Signal handler for SIGINT (Ctrl+C) and SIGTERM
 * Must be async-signal-safe - only certain operations allowed
 */
static void handleSignal(int signal) {
    (void)signal;  // Avoid unused parameter warning
    
    // Second signal = force exit
    if (g_shutdown_requested) {
        std::cout << "\nForce shutdown...\n" << std::endl;
        std::exit(1);
    }
    
    // Atomic write - safe in signal handler
    g_shutdown_requested = 1;
    
    if (g_server) {
        std::cout << "\n\n==================================" << std::endl;
        std::cout << "Shutdown signal received..." << std::endl;
        std::cout << "Cleaning up and closing server..." << std::endl;
        std::cout << "==================================\n" << std::endl;
        
        g_server->shutdown();
        delete g_server;
        g_server = NULL;
    }
    
    std::exit(0);
}

/**
 * Configure signal handlers
 * Uses sigaction() for reliability and portability
 */
static void setupSignalHandlers() {
    struct sigaction sa;
    sa.sa_handler = handleSignal;      // Set our handler function
    sigemptyset(&sa.sa_mask);          // Don't block additional signals
    sa.sa_flags = 0;                   // Default behavior
    
    // Handle Ctrl+C (SIGINT = 2)
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        std::cerr << "Error: Failed to set SIGINT handler" << std::endl;
        std::exit(1);
    }
    
    // Handle termination signal (SIGTERM = 15)
    if (sigaction(SIGTERM, &sa, NULL) == -1) {
        std::cerr << "Error: Failed to set SIGTERM handler" << std::endl;
        std::exit(1);
    }
    
    // Ignore broken pipe - prevents crash when client disconnects
    signal(SIGPIPE, SIG_IGN);
}

/**
 * Display usage information
 */
static void printUsage(const char *programName) {
    std::cout << "\n==================================" << std::endl;
    std::cout << "IRC Server - Usage" << std::endl;
    std::cout << "==================================" << std::endl;
    std::cout << "Usage: " << programName << " <port> <password>\n" << std::endl;
    std::cout << "Arguments:" << std::endl;
    std::cout << "  port     : Port number (1-65535)" << std::endl;
    std::cout << "  password : Server password\n" << std::endl;
    std::cout << "Example:" << std::endl;
    std::cout << "  " << programName << " 6667 mypassword\n" << std::endl;
    std::cout << "==================================" << std::endl;
}

/**
 * Display server startup banner
 */
static void printBanner(int port, const std::string &password) {
    std::cout << "\n==================================" << std::endl;
    std::cout << "    IRC Server Starting" << std::endl;
    std::cout << "==================================" << std::endl;
    std::cout << "Port     : " << port << std::endl;
    std::cout << "Password : " << password << std::endl;
    std::cout << "==================================\n" << std::endl;
    std::cout << "Server is now running..." << std::endl;
    std::cout << "Press Ctrl+C to shutdown\n" << std::endl;
}

/**
 * Main entry point
 */
int main(int argc, char **argv) {
    // Validate argument count
    if (argc != 3) {
        printUsage(argv[0]);
        return 1;
    }
    
    // Setup signal handlers before anything else
    setupSignalHandlers();
    
    try {
        // Parse and validate command-line arguments
        int port = parsePort(argv[1]);
        std::string password = parsePassword(argv[2]);
        
        // Show startup information
        printBanner(port, password);
        
        // Create server instance
        g_server = new Server(port, password);
        
        // Start server main loop (blocks until shutdown)
        g_server->run();
        
    } catch (const std::invalid_argument &e) {
        // Invalid port or password format
        std::cerr << "\n[ERROR] Invalid argument: " << e.what() << std::endl;
        printUsage(argv[0]);
        if (g_server) {
            delete g_server;
            g_server = NULL;
        }
        return 1;
        
    } catch (const std::out_of_range &e) {
        // Port number out of valid range
        std::cerr << "\n[ERROR] Value out of range: " << e.what() << std::endl;
        printUsage(argv[0]);
        if (g_server) {
            delete g_server;
            g_server = NULL;
        }
        return 1;
        
    } catch (const std::runtime_error &e) {
        // Socket creation, bind, listen failures
        std::cerr << "\n[ERROR] Runtime error: " << e.what() << std::endl;
        if (g_server) {
            delete g_server;
            g_server = NULL;
        }
        return 1;
        
    } catch (const std::exception &e) {
        // Catch any other standard exceptions
        std::cerr << "\n[ERROR] Unexpected error: " << e.what() << std::endl;
        if (g_server) {
            delete g_server;
            g_server = NULL;
        }
        return 1;
        
    } catch (...) {
        // Catch-all for non-standard exceptions
        std::cerr << "\n[ERROR] Unknown error occurred" << std::endl;
        if (g_server) {
            delete g_server;
            g_server = NULL;
        }
        return 1;
    }
    
    // Normal shutdown path (rarely reached due to signal handling)
    std::cout << "\n==================================" << std::endl;
    std::cout << "Server shutdown complete" << std::endl;
    std::cout << "==================================" << std::endl;
    
    if (g_server) {
        delete g_server;
        g_server = NULL;
    }
    
    return 0;
}