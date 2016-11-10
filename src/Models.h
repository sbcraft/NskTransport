typedef struct Vehicle {
    char *name;
    char *arriveIn;
} Vehicle;

typedef struct Stop {
    int idx;
    float distance;
    char *name;
    Vehicle *vehicles;
} Stop;