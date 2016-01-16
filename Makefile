CXXFLAGS = -std=c++11 -pipe -Wall -Wextra -I.
DFLAGS = 
OFLAGS = -O3

OBJS = Parser.o Result.o Sequence.o Writer.o main.o BasicEditDistance.o Solver.o SubmatrixCalculator.o
PROGS = bioinformatics

bioinformatics: pre $(OBJS)
		@$(CXX) -o bin/bioinformatics $(addprefix bin/, $(OBJS)) $(CXXFLAGS) $(OFLAGS) $(DFLAGS)
		@strip bin/bioinformatics

.PHONY: clean
clean:
	@rm -rf bin/

.PHONY: pre
pre:
	@mkdir -p bin

$(OBJS): %.o: src/%.cpp
	@$(CXX) -o bin/$@ $(CXXFLAGS) -c $<
	@echo "[$(CXX)] $@"
