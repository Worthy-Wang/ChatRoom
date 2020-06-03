SRCS:=$(wildcard *.cpp)
ELFS:=$(SRCS:%.cpp=%)
all:$(ELFS)
	@for i in $(ELFS);do g++ -o $${i} $${i}.cpp;done
clean:
	rm -rf $(ELFS)

