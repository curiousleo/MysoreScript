/*
 * Copyright (c) 2014 David Chisnall
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include <iostream>
#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <time.h>
#include <unistd.h>
#include <gc.h>
#include "parser.hh"
#include "interpreter.hh"

/**
 * Flag indicating whether we should print timing information.
 */
static bool enableTiming = false;

/**
 * If timing is enabled then log a message indicating the amount of time that
 * has elapsed since c1 and the peak memory consumption.
 */
static void logTimeSince(clock_t c1, const char *msg)
{
	if (!enableTiming) { return; }
	clock_t c2 = clock();
	struct rusage r;
	getrusage(RUSAGE_SELF, &r);
	fprintf(stderr, "%s took %f seconds.	Peak used %ldKB.\n", msg,
		((double)c2 - (double)c1) / (double)CLOCKS_PER_SEC, r.ru_maxrss/1024);
}
/**
 * Print the usage message.
 */
void usage(const char *cmd)
{
	fprintf(stderr, "usage: %s [-himt] [-f {file name}]\n", cmd);
	fprintf(stderr, " -h          Display this help\n");
	fprintf(stderr, " -i          Interpreter, enable REPL mode\n");
	fprintf(stderr, " -m          Display memory usage stats on exit\n");
	fprintf(stderr, " -t          Display timing information\n");
	fprintf(stderr, " -f {file}   Load and execute file\n");
}

int main(int argc, char **argv)
{
	clock_t c1;
	// Are we in read-evaluate-print-loop mode?
	bool repl = false;
	// Are memory usage statistics requested?
	bool memstats = false;
	// What file should we print?
	const char *file = nullptr;
	if (argc < 1)
	{
		usage(argv[0]);
		return EXIT_FAILURE;
	}
	int c;
	// Parse the options that we understand
	while ((c = getopt(argc, argv, "hmitf:")) != -1)
	{
		switch (c)
		{
			case 'i':
				repl = true;
				break;
			case 'f':
				file = optarg;
				break;
			case 't':
				enableTiming = true;
				break;
			case 'm':
				memstats = true;
				break;
			case 'h':
				usage(argv[0]);
				break;
		}
	}
	c1 = clock();
	//Initialise the garbage collection library.  This must be called before
	//any objects are allocated.
	GC_init();

	// Set up a parser and interpreter context to use.
	Parser::MysoreScriptParser p;
	Interpreter::Context C;
	// Log the time taken for all of the program setup.
	logTimeSince(c1, "Setup");
	// The AST for the program loaded from a file, if there is one
	std::unique_ptr<AST::Statements> ast = 0;
	// If a filename was specified, then try to parse and execute it.
	if (file)
	{
		c1 = clock();
		// Open the file
		pegmatite::AsciiFileInput input(open(file, O_RDONLY));
		pegmatite::ErrorList el;
		c1 = clock();
		// Parse one or more statements, report errors if there are any
		if (!p.parse(input, p.g.statements, p.g.ignored, el, ast))
		{
			std::cerr << "errors: \n";
			for (auto &err : el)
			{
				std::cerr << "line " << err.start.line
						  << ", col " << err.finish.col <<  ": ";
				std::cerr << "syntax error" << std::endl;
			}
			return EXIT_FAILURE;
		}
		logTimeSince(c1, "Parsing program");
		c1 = clock();
		// Now interpret the parsed 
		ast->interpret(C);
		logTimeSince(c1, "Executing program");
	}
	// Keep all of the ASTs that we've parsed in the REPL environment in case
	// anything is referencing them.
	std::vector<std::unique_ptr<AST::Statements>> replASTs;
	// As long as we're in REPL mode, read, evaluate and loop - we don't
	// actually print the result, so technically this is REL...
	while (repl)
	{
		c1 = clock();
		GC_gcollect();
		logTimeSince(c1, "Garbage collection");
		std::string buffer;
		// Print the prompt
		std::cout << "\nMysoreScript> ";
		// Get a line
		std::getline(std::cin, buffer);
		// If it was an empty line, exit REPL mode
		if (buffer.size() == 0)
		{
			repl = false;
			break;
		}
		// Parse the line
		pegmatite::StringInput input(buffer);
		std::unique_ptr<AST::Statements> ast = 0;
		pegmatite::ErrorList el;
		c1 = clock();
		if (!p.parse(input, p.g.statements, p.g.ignored, el, ast))
		{
			std::cerr << "errors: \n";
			for (auto &err : el)
			{
				std::cerr << "line " << err.start.line
						  << ", col " << err.finish.col <<  ": ";
				std::cerr << "syntax error" << std::endl;
			}
			continue;
		}
		logTimeSince(c1, "Parsing program");
		c1 = clock();
		// Interpret the resulting AST
		ast->interpret(C);
		logTimeSince(c1, "Executing program");
		// Keep the AST around - it may contain things that we refer to later
		// (e.g. functions / classes).
		replASTs.push_back(std::move(ast));
	}
	// Print some memory usage stats, if requested.
	if (memstats)
	{
		fprintf(stderr, "Allocated %lld bytes during execution.\n", (long
					long)GC_get_total_bytes());
		fprintf(stderr, "GC heap size: %lld bytes.\n",
				(long long)GC_get_heap_size());
		ast = nullptr;
		replASTs.clear();
		GC_gcollect_and_unmap();
		fprintf(stderr, "After collection, GC heap size: %lld bytes.\n",
				(long long)GC_get_heap_size());
	}
	return 0;
}
