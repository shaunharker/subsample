# add the executable

set ( LIBS      ${LIBS}
                ${Boost_LIBRARIES} )

add_library ( libwasserstein STATIC wasserstein.cpp basic_defs.cpp auction_oracle.cpp )
set_target_properties(libwasserstein PROPERTIES OUTPUT_NAME wasserstein)

add_library ( libwassersteinshared SHARED wasserstein.cpp basic_defs.cpp auction_oracle.cpp )
target_link_libraries ( libwassersteinshared ${LIBS} )
set_target_properties(libwassersteinshared PROPERTIES OUTPUT_NAME wasserstein)

install(TARGETS libwasserstein libwassersteinshared
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
