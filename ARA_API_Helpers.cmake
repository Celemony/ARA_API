# ======================
# CMake helper functions related to ARA_API
#
# Copyright (c) 2020-2021, Celemony Software GmbH, All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# ======================

include_guard()

# ======================
# private helper function needed to deal with various different CMake versions - do not call externally!

function(ara_private_configure_ARA_API_interface_settings target visibility)
    target_include_directories(${target}
        ${visibility}
            "${CMAKE_CURRENT_LIST_DIR}/.."
    )

    # \todo test this - do we always see the correct value for ARA_ENABLE_DEPRECATION_WARNINGS?
    if(ARA_ENABLE_DEPRECATION_WARNINGS)
        target_compile_definitions(${target}
            ${visibility}
                ARA_ENABLE_DEPRECATION_WARNINGS=1
        )
    endif()
endfunction()


# ======================
# "link" given target against ARA_API
# a function is provided to encapsulate workarounds for older CMake versions

function(ara_link_ARA_API target visibility)

    if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.12)
        target_link_libraries(${target} ${visibility} ARA_API)
    else()
        # older CMake can't automatically pull the public properties from object targets,
        # therefore we need to set the relevant properties from ARA_API manually here.
        ara_private_configure_ARA_API_interface_settings(${target} ${visibility})
    endif()

endfunction()


# ======================
# apply the recommended warning configuration for our targets

function(ara_disable_unwanted_warnings target)

    # compiler-specific settings
    if(MSVC)
        target_compile_options(${target}
            PRIVATE
                /wd4351     # elements of array 'array' will be default initialized (deprecated)
                /wd4503     # decorated name length exceeded
                /wd26812    # prefer 'enum class' over 'enum'
        )
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        target_compile_options(${target}
            PRIVATE
                -Wno-unused-parameter
                -Wno-gnu-zero-variadic-macro-arguments
                -Wno-vla-extension
                -Wno-global-constructors
                -Wno-missing-variable-declarations
                -Wno-used-but-marked-unused
                $<$<COMPILE_LANGUAGE:C>:-Wno-missing-prototypes>
                $<$<COMPILE_LANGUAGE:CXX>:-Wno-weak-vtables>
        )
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        target_compile_options(${target}
            PRIVATE
                -Wno-unused-parameter
                -Wno-variadic-macros
                -Wno-vla
                $<$<COMPILE_LANGUAGE:C>:-Wno-missing-prototypes>
        )
    endif()

endfunction()


# ======================
# sort files in the given target by tree within the project

function(ara_group_target_files target)
    get_target_property(sources ${target} SOURCES)
    get_target_property(root_dir ${target} SOURCE_DIR)
    source_group(TREE "${root_dir}" FILES ${sources})
endfunction()
