# add the executable

add_subdirectory (ann)


set ( LIBS      ${LIBS}
                libANNshared
                ${Boost_LIBRARIES} )

add_library ( libbottleneck STATIC bottleneck.cpp basic_defs.cpp bound_match.cpp neighb_oracle.cpp )
set_target_properties(libbottleneck PROPERTIES OUTPUT_NAME bottleneck)

add_library ( libbottleneckshared SHARED bottleneck.cpp basic_defs.cpp bound_match.cpp neighb_oracle.cpp )
target_link_libraries ( libbottleneckshared ${LIBS} )
set_target_properties(libbottleneckshared PROPERTIES OUTPUT_NAME bottleneck)

install(TARGETS libbottleneck libbottleneckshared
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
