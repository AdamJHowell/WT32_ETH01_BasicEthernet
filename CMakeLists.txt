cmake_minimum_required( VERSION 3.24 )
project( WT32_ETH01_BasicEthernet )

set( CMAKE_CXX_STANDARD 14 )

include_directories( . )

add_executable( WT32_ETH01_BasicEthernet
		WT32_ETH01_BasicEthernet.ino )

set_target_properties( WT32_ETH01_BasicEthernet PROPERTIES LINKER_LANGUAGE CXX )
