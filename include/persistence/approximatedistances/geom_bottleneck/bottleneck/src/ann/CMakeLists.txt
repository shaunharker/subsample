# add the executable

set ( LIBS      ${LIBS}
                ${Boost_LIBRARIES} )

add_library ( libANN STATIC ANN.cpp
                            bd_fix_rad_search.cpp
                            bd_pr_search.cpp
                            bd_search.cpp
                            bd_tree.cpp
                            kd_dump.cpp
                            kd_fix_rad_search.cpp
                            kd_pr_search.cpp
                            kd_search.cpp
                            kd_split.cpp
                            kd_tree.cpp
                            kd_util.cpp )
set_target_properties(libANN PROPERTIES OUTPUT_NAME ANN)

add_library ( libANNshared SHARED ANN.cpp
                            bd_fix_rad_search.cpp
                            bd_pr_search.cpp
                            bd_search.cpp
                            bd_tree.cpp
                            kd_dump.cpp
                            kd_fix_rad_search.cpp
                            kd_pr_search.cpp
                            kd_search.cpp
                            kd_split.cpp
                            kd_tree.cpp
                            kd_util.cpp )
set_target_properties(libANNshared PROPERTIES OUTPUT_NAME ANN)

install(TARGETS libANN libANNshared
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
