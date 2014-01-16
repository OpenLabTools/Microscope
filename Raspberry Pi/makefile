
FLAGS = -g -o
GRAPHICS = -I.. -Wall -W -ansi -pedantic -Dcimg_use_vt100 -I/usr/X11R6/include -lm -L/usr/X11R6/lib -lpthread -lX11
LINKING = -lboost_system 
#-lncurses



## 'basic' executable and compilation
basic: basic.compile
	@echo "\n\n** Running basic **\n"
	sudo ./basic.x
	
basic.compile:
	@echo "\n\n** Compiling basic.cpp **\n"
	g++ $(FLAGS) basic.x basic.cpp $(GRAPHICS) $(LINKING)



## 'testing' executable and compilation
testing: testing.compile
	@echo "\n\n** Running executable testing **\n"
	sudo ./testing.x

testing.compile: testing.cpp
	@echo "\n\n** Compiling testing.cpp in linux X11 environment **\n"
	g++ $(FLAGS) testing.x testing.cpp $(GRAPHICS) $(LINKING)



## 'focus_applied' executable and compilation
focus_full.run:
	@echo "\n\n** Running executable focus_full **\n"
	sudo ./focus_full.x
	
focus_tuning.run:
	@echo "\n\n** Running executable focus_tuning **\n"
	sudo ./focus_tuning.x
	
focus_sweep.run:
	@echo "\n\n** Running executable focus_sweep **\n"
	sudo ./focus_sweep.x
	
focus_test.run:
	@echo "\n\n** Running executable focus_applied **\n"
	sudo ./focus_test.x
	
focus_everything.run:
	@echo "\n\n** Running executable focus_applied **\n"
	sudo ./focus_everything.x


hocus_focus_everything: focus_everything.cpp
	@echo "\n\n** Compiling focus_everything.cpp in linux X11 environment **\n"
	g++ $(FLAGS) focus_everything.x focus_everything.cpp $(GRAPHICS) $(LINKING)
	
hocus_focus_full: focus_applied.cpp
	@echo "\n\n** Compiling focus_full.cpp in linux X11 environment **\n"
	g++ $(FLAGS) focus_full.x focus_full.cpp $(GRAPHICS) $(LINKING)
	
hocus_focus_tuning: focus_applied2.cpp
	@echo "\n\n** Compiling focus_tuning_only.cpp in linux X11 environment **\n"
	g++ $(FLAGS) focus_tuning.x focus_tuning_only.cpp $(GRAPHICS) $(LINKING)
	
hocus_focus_test: focus_applied2.cpp
	@echo "\n\n** Compiling focus_test.cpp in linux X11 environment **\n"
	g++ $(FLAGS) focus_test.x focus_test.cpp $(GRAPHICS) $(LINKING)
	
hocus_focus_sweep: focus_applied2.cpp
	@echo "\n\n** Compiling focus_sweep_only.cpp in linux X11 environment **\n"
	g++ $(FLAGS) focus_sweep.x focus_sweep_only.cpp $(GRAPHICS) $(LINKING)
	


## 'stack' executable and compilation
stack.run: stack
	@echo "\n\n** Running executable stack **\n"
	sudo ./stack.x
	
stack: stack.cpp
	@echo "\n\n** Compiling stack.cpp in linux X11 environment **\n"
	g++ $(FLAGS) stack.x stack.cpp $(GRAPHICS)



## 'edges' executable and compilation
edges.run:
	@echo "\n\n** Running executable edges **\n"
	sudo ./edges.x
	
edges: edges.cpp
	@echo "\n\n** Compiling edges.cpp in linux X11 environment **\n"
	g++ $(FLAGS) edges.x edges.cpp $(GRAPHICS) $(LINKING)
