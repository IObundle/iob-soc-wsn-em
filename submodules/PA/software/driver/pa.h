// Functions

// Initialize PA
void pa_init(int base_address);

// Set power down
void pa_set_pd(char value);

// Set mode
void pa_set_mode(char value);

// Turn PA On
#define pa_on() pa_set_pd(0)

// Turn PA Off
#define pa_off() pa_set_pd(1)
