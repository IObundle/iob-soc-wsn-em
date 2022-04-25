#include "id.h"
#include "cm_def.h"
   
int main() { 

  id_init(ID_BASE);
  				  
  if(!get_id()) {  
    sensor_node();
  } else {
    base_station();
  }
  
  return 0;
}
