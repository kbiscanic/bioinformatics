CXXFLAGS = -std=c++11 -pipe -Wall -Wextra -I.
DFLAGS = 
OFLAGS = -O3
LFLAGS = -pthread -L. -L/usr/local/lib/

OBJS = Parser.o Result.o Sequence.o Writer.o
PROGS = bioinformatics

bioinformatics: pre $(OBJS)
		@$(CXX) -o bin/bioinformatics $(addprefix bin/, $(OBJS)) $(CXXFLAGS) $(OFLAGS) $(DFLAGS) $(LFLAGS)
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
