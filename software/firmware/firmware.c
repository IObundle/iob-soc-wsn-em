#include "id.h"
#include "cm_def.h"
   
int main() { 

  id_init(ID_BASE);
  				  
  if(get_id() == 1) {  
    base_station();
  } else {    
    sensor_node();
  }
  
  return 0;
}
