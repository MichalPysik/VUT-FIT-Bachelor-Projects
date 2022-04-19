#============================================================================
# Run: 
#    xtclsh proj3_ise.tcl  - creates XILINX ISE project file
#    ise proj3_project.ise - opens the project
#============================================================================
source "../../../../../base/xilinxise.tcl"

project_new "proj3_project"
project_set_props
puts "Adding source files"
xfile add "../../../../../fpga/units/clkgen/clkgen_config.vhd"
xfile add "proj3_config.vhd"
xfile add "../../../../../fpga/units/clkgen/clkgen.vhd"
xfile add "../../../../../fpga/units/math/math_pack.vhd"
xfile add "../../../../../fpga/ctrls/spi/spi_adc_entity.vhd"
xfile add "../../../../../fpga/ctrls/spi/spi_adc.vhd"
xfile add "../../../../../fpga/ctrls/spi/spi_adc_autoincr.vhd"
xfile add "../../../../../fpga/ctrls/spi/spi_reg.vhd"
xfile add "../../../../../fpga/ctrls/spi/spi_ctrl.vhd"
xfile add "../../../../../fpga/chips/fpga_xc3s50.vhd"
xfile add "../../../../../fpga/chips/architecture_gp/arch_gp_ifc.vhd"
xfile add "../../../../../fpga/chips/architecture_gp/tlv_gp_ifc.vhd"
xfile add "../../fpga/top.vhd"
xfile add "../../fpga/counter.vhd"
xfile add "../../fpga/display.vhd"
xfile add "../../fpga/matrix_pack.vhd"
xfile add "../../fpga/cell.vhd"
puts "Adding simulation files"
xfile add "../../fpga/sim/counter_tb.vhd" -view Simulation
xfile add "../../fpga/sim/display_tb.vhd" -view Simulation
xfile add "../../fpga/sim/matrix_pack_tb.vhd" -view Simulation
xfile add "../../fpga/sim/tb.vhd" -view Simulation
puts "Libraries"
project_set_isimscript "proj3_xsim.tcl"
project_set_top "fpga"
project_close
