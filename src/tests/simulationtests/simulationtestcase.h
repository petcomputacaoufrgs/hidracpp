#define PUT_INSTRUCTION_IN_MEMORY(machine, inst, param)\
machine.setMemoryValue(0, inst);\
machine.setMemoryValue(1, param); 