message (STATUS "testBMPosAng...")

add_executable(testBMPosAng
	testBMPosAng.cpp
	TrackMatch.hpp)

target_link_libraries(testBMPosAng
	${ROOT_LIBRARIES}
	${Geant4_LIBRARIES}
	${Boost_LIBRARIES}
	${B2MC_LIBRARY}
	libNTBM
)

# install the execute in the bin folder
install(TARGETS testBMPosAng DESTINATION "${CMAKE_INSTALL_BINDIR}/test")