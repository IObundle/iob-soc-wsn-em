# library search paths
set_attribute lib_search_path [list /opt/ic_tools/pdk/faraday/umc130/LL/fsc0l_d/2009Q2v3.0/GENERIC_CORE/FrontEnd/synopsys .]

set libs [glob *.lib]
set_attribute library [list fsc0l_d_generic_core_tt1p2v25c.lib $libs]

set_attribute hdl_search_path $INCLUDE

#verilog source files, defines and includes
echo "\n\n"
echo "INCLUDE=" $INCLUDE
echo "\n\n"
echo "DEFINE=" $DEFINE
echo "\n\n"
echo "VSRC=" $VSRC
echo "\n\n"

#verilog defines 
read_hdl -v2001 -define $DEFINE $VSRC
elaborate system
define_clock -name clk -period 31250 [find / -port clk] 
synthesize -to_mapped
#retime -prepare -min_delay
report gates > gates_report.txt
report area > area_report.txt
report timing > timing_report.txt
write_hdl -mapped > system_synth.v 
write_sdc > system_synth.sdc

exit
