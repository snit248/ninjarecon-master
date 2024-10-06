message (STATUS "BMPositionAngleDist...")

add_executable(BMPositionAngleDist
	BMPositionAngleDist.cpp
	BabyMindFitter.cpp
)

target_link_libraries(BMPositionAngleDist
	${ROOT_LIBRARIES}
	${Geant4_LIBRARIES}
	${Boost_LIBRARIES}
	${B2MC_LIBRARY}
	libNTBM
)

target_include_directories(BMPositionAngleDist PRIVATE ${CMAKE_SOURCE_DIR}/include)

# install the execute in the bin folder
install(TARGETS BMPositionAngleDist DESTINATION "${CMAKE_INSTALL_BINDIR}/BMAnalysis")