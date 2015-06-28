# add the executable

set ( LIBS ${LIBS} ${Boost_LIBRARIES} ${MPI_LIBRARIES})

add_executable ( ComputeSubsample ComputeSubsample.cpp )
target_link_libraries ( ComputeSubsample ${LIBS} )

add_executable ( ComputeDistances ComputeDistances.cpp )
target_link_libraries ( ComputeDistances ${LIBS} )

if(MPI_COMPILE_FLAGS)
  set_target_properties(ComputeSubsample PROPERTIES
    COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
  set_target_properties(ComputeDistances PROPERTIES
    COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
endif()

if(MPI_LINK_FLAGS)
  set_target_properties(ComputeSubsample PROPERTIES
    LINK_FLAGS "${MPI_LINK_FLAGS}")
  set_target_properties(ComputeDistances PROPERTIES
    LINK_FLAGS "${MPI_LINK_FLAGS}")
endif()

install(TARGETS ComputeSubsample ComputeDistances
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
