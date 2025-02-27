# Copyright 2022 Joe Drago. All rights reserved.
# SPDX-License-Identifier: BSD-2-Clause

# With testing enabled, all targets referenced by add_test() can be run
# at once with CMake's ctest command line tool from the build folder.
enable_testing()

################################################################################
# C tests and tools

add_executable(aviftest aviftest.c)
if(AVIF_LOCAL_LIBGAV1)
    set_target_properties(aviftest PROPERTIES LINKER_LANGUAGE "CXX")
endif()
target_link_libraries(aviftest avif ${AVIF_PLATFORM_LIBRARIES})
add_test(NAME aviftest COMMAND aviftest ${CMAKE_CURRENT_SOURCE_DIR}/data)

if(AVIF_ENABLE_COVERAGE)
    add_custom_target(
        avif_coverage
        COMMAND ${CMAKE_COMMAND} -E env "LLVM_PROFILE_FILE=${CMAKE_CURRENT_BINARY_DIR}/aviftest.profraw" $<TARGET_FILE:aviftest>
                ${CMAKE_CURRENT_SOURCE_DIR}/data
        COMMAND ${XCRUN} llvm-profdata merge -sparse ${CMAKE_CURRENT_BINARY_DIR}/aviftest.profraw -o
                ${CMAKE_CURRENT_BINARY_DIR}/aviftest.profdata
        COMMAND cmake -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/coverage
        COMMAND ${XCRUN} llvm-cov show $<TARGET_FILE:aviftest> -instr-profile=${CMAKE_CURRENT_BINARY_DIR}/aviftest.profdata
                -project-title=libavif --format html -output-dir=${CMAKE_CURRENT_BINARY_DIR}/coverage
        COMMAND echo Coverage report here: ${CMAKE_CURRENT_BINARY_DIR}/coverage/index.html
        DEPENDS aviftest
    )
endif()

add_executable(avifyuv avifyuv.c)
if(AVIF_LOCAL_LIBGAV1)
    set_target_properties(avifyuv PROPERTIES LINKER_LANGUAGE "CXX")
endif()
target_link_libraries(avifyuv avif ${AVIF_PLATFORM_LIBRARIES})
foreach(AVIFYUV_MODE limited rgb) # Modes drift and premultiply take more than 2 minutes each so they are disabled.
    add_test(NAME avifyuv_${AVIFYUV_MODE} COMMAND avifyuv -m ${AVIFYUV_MODE})
endforeach()

if(AVIF_ENABLE_GTEST OR AVIF_BUILD_APPS)
    enable_language(CXX)
    set(CMAKE_CXX_STANDARD 11)
    add_library(aviftest_helpers OBJECT gtest/aviftest_helpers.cc)
    target_link_libraries(aviftest_helpers avif_apps)
endif()

################################################################################
# GoogleTest

if(AVIF_ENABLE_GTEST)
    if(AVIF_LOCAL_GTEST)
        set(GTEST_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/ext/googletest/googletest/include)
        set(GTEST_LIBRARIES ${CMAKE_SOURCE_DIR}/ext/googletest/build/lib/${AVIF_LIBRARY_PREFIX}gtest${AVIF_LIBRARY_SUFFIX})
        set(GTEST_MAIN_LIBRARIES
            ${CMAKE_SOURCE_DIR}/ext/googletest/build/lib/${AVIF_LIBRARY_PREFIX}gtest_main${AVIF_LIBRARY_SUFFIX}
        )
        set(GTEST_BOTH_LIBRARIES ${GTEST_LIBRARIES} ${GTEST_MAIN_LIBRARIES})
        if(NOT EXISTS ${GTEST_INCLUDE_DIRS}/gtest/gtest.h)
            message(FATAL_ERROR "googletest(AVIF_LOCAL_GTEST): ${GTEST_INCLUDE_DIRS}/gtest/gtest.h is missing, bailing out")
        elseif(NOT EXISTS ${GTEST_LIBRARIES})
            message(FATAL_ERROR "googletest(AVIF_LOCAL_GTEST): ${GTEST_LIBRARIES} is missing, bailing out")
        elseif(NOT EXISTS ${GTEST_MAIN_LIBRARIES})
            message(FATAL_ERROR "googletest(AVIF_LOCAL_GTEST): ${GTEST_MAIN_LIBRARIES} is missing, bailing out")
        else()
            message(STATUS "Found local ext/googletest")
        endif()
    else()
        find_package(GTest REQUIRED)
    endif()

    add_executable(avifallocationtest gtest/avifallocationtest.cc)
    target_link_libraries(avifallocationtest aviftest_helpers ${GTEST_BOTH_LIBRARIES})
    target_include_directories(avifallocationtest PRIVATE ${GTEST_INCLUDE_DIRS})
    add_test(NAME avifallocationtest COMMAND avifallocationtest)

    add_executable(avifalphapremtest gtest/avifalphapremtest.cc)
    target_link_libraries(avifalphapremtest aviftest_helpers ${GTEST_BOTH_LIBRARIES})
    target_include_directories(avifalphapremtest PRIVATE ${GTEST_INCLUDE_DIRS})
    add_test(NAME avifalphapremtest COMMAND avifalphapremtest)

    add_executable(avifchangesettingtest gtest/avifchangesettingtest.cc)
    target_link_libraries(avifchangesettingtest aviftest_helpers ${GTEST_BOTH_LIBRARIES})
    target_include_directories(avifchangesettingtest PRIVATE ${GTEST_INCLUDE_DIRS})
    add_test(NAME avifchangesettingtest COMMAND avifchangesettingtest)

    add_executable(avifcllitest gtest/avifcllitest.cc)
    target_link_libraries(avifcllitest aviftest_helpers ${GTEST_BOTH_LIBRARIES})
    target_include_directories(avifcllitest PRIVATE ${GTEST_INCLUDE_DIRS})
    add_test(NAME avifcllitest COMMAND avifcllitest)

    add_executable(avifgridapitest gtest/avifgridapitest.cc)
    target_link_libraries(avifgridapitest aviftest_helpers ${GTEST_BOTH_LIBRARIES})
    target_include_directories(avifgridapitest PRIVATE ${GTEST_INCLUDE_DIRS})
    add_test(NAME avifgridapitest COMMAND avifgridapitest)

    add_library(avifincrtest_helpers OBJECT gtest/avifincrtest_helpers.cc)
    target_link_libraries(avifincrtest_helpers avif ${AVIF_PLATFORM_LIBRARIES} ${GTEST_LIBRARIES})
    target_include_directories(avifincrtest_helpers PUBLIC ${GTEST_INCLUDE_DIRS})

    add_executable(avifincrtest gtest/avifincrtest.cc)
    target_link_libraries(avifincrtest aviftest_helpers avifincrtest_helpers)
    add_test(NAME avifincrtest COMMAND avifincrtest ${CMAKE_CURRENT_SOURCE_DIR}/data/)

    add_executable(avifmetadatatest gtest/avifmetadatatest.cc)
    target_link_libraries(avifmetadatatest aviftest_helpers ${GTEST_LIBRARIES})
    target_include_directories(avifmetadatatest PRIVATE ${GTEST_INCLUDE_DIRS})
    add_test(NAME avifmetadatatest COMMAND avifmetadatatest ${CMAKE_CURRENT_SOURCE_DIR}/data/)

    add_executable(avifopaquetest gtest/avifopaquetest.cc)
    target_link_libraries(avifopaquetest aviftest_helpers ${GTEST_BOTH_LIBRARIES})
    target_include_directories(avifopaquetest PRIVATE ${GTEST_INCLUDE_DIRS})
    add_test(NAME avifopaquetest COMMAND avifopaquetest)

    add_executable(avifpng16bittest gtest/avifpng16bittest.cc)
    target_link_libraries(avifpng16bittest aviftest_helpers ${GTEST_LIBRARIES})
    target_include_directories(avifpng16bittest PRIVATE ${GTEST_INCLUDE_DIRS})
    add_test(NAME avifpng16bittest COMMAND avifpng16bittest ${CMAKE_CURRENT_SOURCE_DIR}/data/)

    add_executable(avifreadimagetest gtest/avifreadimagetest.cc)
    target_link_libraries(avifreadimagetest aviftest_helpers ${GTEST_LIBRARIES})
    target_include_directories(avifreadimagetest PRIVATE ${GTEST_INCLUDE_DIRS})
    add_test(NAME avifreadimagetest COMMAND avifreadimagetest ${CMAKE_CURRENT_SOURCE_DIR}/data/)

    add_executable(avifrgbtoyuvtest gtest/avifrgbtoyuvtest.cc)
    target_link_libraries(avifrgbtoyuvtest aviftest_helpers ${GTEST_BOTH_LIBRARIES})
    target_include_directories(avifrgbtoyuvtest PRIVATE ${GTEST_INCLUDE_DIRS})
    add_test(NAME avifrgbtoyuvtest COMMAND avifrgbtoyuvtest)

    add_executable(aviftilingtest gtest/aviftilingtest.cc)
    target_link_libraries(aviftilingtest avif_internal ${GTEST_BOTH_LIBRARIES})
    target_include_directories(aviftilingtest PRIVATE ${GTEST_INCLUDE_DIRS})
    add_test(NAME aviftilingtest COMMAND aviftilingtest)

    add_executable(avify4mtest gtest/avify4mtest.cc)
    target_link_libraries(avify4mtest aviftest_helpers ${GTEST_BOTH_LIBRARIES})
    target_include_directories(avify4mtest PRIVATE ${GTEST_INCLUDE_DIRS})
    add_test(NAME avify4mtest COMMAND avify4mtest)

    if(NOT AVIF_CODEC_AOM OR NOT AVIF_CODEC_AOM_ENCODE OR NOT AVIF_CODEC_AOM_DECODE)
        # These tests are supported with aom being the encoder and decoder. If aom is unavailable,
        # these tests are disabled because other codecs may not implement all the necessary features.
        # For example, SVT-AV1 requires 4:2:0 images with even dimensions of at least 64x64 px.
        set_tests_properties(avifallocationtest avifgridapitest avifmetadatatest avifincrtest PROPERTIES DISABLED True)

        message(STATUS "Some tests are disabled because aom is unavailable for encoding or decoding.")
    endif()

    if(NOT libsharpyuv_FOUND)
        message(STATUS "Some tests are skipped because libsharpyuv is unavailable.")
    endif()
else()
    message(STATUS "Most tests are disabled because AVIF_ENABLE_GTEST is OFF.")
endif()

################################################################################
# Bash tests

if(AVIF_BUILD_APPS)
    # When building apps, test the avifenc/avifdec.
    # 'are_images_equal' is used to make sure inputs/outputs are unchanged.
    add_executable(are_images_equal gtest/are_images_equal.cc)
    target_link_libraries(are_images_equal aviftest_helpers)
    add_test(NAME test_cmd COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/test_cmd.sh ${CMAKE_BINARY_DIR}
                                   ${CMAKE_CURRENT_SOURCE_DIR}/data
    )
    add_test(NAME test_cmd_grid COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/test_cmd_grid.sh ${CMAKE_BINARY_DIR}
                                        ${CMAKE_CURRENT_SOURCE_DIR}/data
    )
    add_test(NAME test_cmd_lossless COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/test_cmd_lossless.sh ${CMAKE_BINARY_DIR}
                                            ${CMAKE_CURRENT_SOURCE_DIR}/data
    )
    add_test(NAME test_cmd_metadata COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/test_cmd_metadata.sh ${CMAKE_BINARY_DIR}
                                            ${CMAKE_CURRENT_SOURCE_DIR}/data
    )

    if(NOT AVIF_CODEC_AOM OR NOT AVIF_CODEC_AOM_ENCODE)
        # Only aom encoder supports lossless encoding.
        set_property(TEST test_cmd_lossless PROPERTY DISABLED True)

        # SVT-AV1 does not support the images with odd dimensions that are used in this test.
        if(NOT AVIF_CODEC_RAV1E)
            set_property(TEST test_cmd_metadata PROPERTY DISABLED True)
        endif()

        message(STATUS "Some tests are disabled because aom is unavailable for encoding.")
    endif()
endif()
