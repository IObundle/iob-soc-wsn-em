// Functions

// Initialize Limiter
void limiter_init(int base_address);

// Set power down
void limiter_set_pd(char value);

// Turn Limiter On
#define limiter_on() limiter_set_pd(0)

// Turn Limiter Off
#define limiter_off() limiter_set_pd(1)
