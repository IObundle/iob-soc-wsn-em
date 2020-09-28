// Functions

// Initialize Iref
void iref_init(int base_address);

// Set power down
void iref_set_pd(char value);

// Set FC
void iref_set_fc(char value);

// Turn Iref On
#define iref_on() iref_set_pd(0)

// Turn Iref Off
#define iref_off() iref_set_pd(1)
