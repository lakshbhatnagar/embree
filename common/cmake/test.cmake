## ======================================================================== ##
## Copyright 2009-2015 Intel Corporation                                    ##
##                                                                          ##
## Licensed under the Apache License, Version 2.0 (the "License");          ##
## you may not use this file except in compliance with the License.         ##
## You may obtain a copy of the License at                                  ##
##                                                                          ##
##     http://www.apache.org/licenses/LICENSE-2.0                           ##
##                                                                          ##
## Unless required by applicable law or agreed to in writing, software      ##
## distributed under the License is distributed on an "AS IS" BASIS,        ##
## WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. ##
## See the License for the specific language governing permissions and      ##
## limitations under the License.                                           ##
## ======================================================================== ##

ENABLE_TESTING()
INCLUDE(CTest)

SET(EMBREE_MODEL_DIR /Users/swoop/Work/models/embree-models)

FILE(READ "${EMBREE_MODEL_DIR}/embree-models-subdiv.txt" models_subdiv)
STRING(REGEX REPLACE "\n" ";" models_subdiv "${models_subdiv}")

FILE(READ "${EMBREE_MODEL_DIR}/embree-models-small-win32.txt" models_small_win32)
STRING(REGEX REPLACE "\n" ";" models_small_win32 "${models_small_win32}")

FILE(READ "${EMBREE_MODEL_DIR}/embree-models-small-x64.txt" models_small_x64)
STRING(REGEX REPLACE "\n" ";" models_small_x64 "${models_small_x64}")

FILE(READ "${EMBREE_MODEL_DIR}/embree-models-large.txt" models_large)
STRING(REGEX REPLACE "\n" ";" models_large "${models_large}")

SET(models ${models_small_win32}) # FIXME: configure all models

#if platform == 'Win32': models += models_small_win32
#else:                   models += models_small_x64
#if platform == 'x64' and OS != 'macosx':
#  models += models_large

MACRO (ADD_EMBREE_TEST name executable)
  ADD_TEST(NAME ${name}
           WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
           COMMAND ${PROJECT_SOURCE_DIR}/scripts/invoke_test.py
                   --name ${name}
                   --modeldir ${EMBREE_MODEL_DIR}
                   --model default
                   --execute ${PROJECT_BINARY_DIR}/${executable})

#SET_TESTS_PROPERTIES(triangle_geometry PROPERTIES ATTACHED_FILES "triangle_geometry.tga")
ENDMACRO()

MACRO (ADD_EMBREE_MODELS_TEST name executable)
  FOREACH (model ${models})
    STRING(REGEX REPLACE "/" "_" modelname "${model}")
    STRING(REGEX REPLACE ".ecs" "" modelname "${modelname}")
    ADD_TEST(NAME "${name}_${modelname}"
             WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
             COMMAND ${PROJECT_SOURCE_DIR}/scripts/invoke_test.py
                     --name "${name}_${modelname}"
                     --modeldir ${EMBREE_MODEL_DIR}
                     --model ${model}
                     --execute ${PROJECT_BINARY_DIR}/${executable})
  ENDFOREACH()
ENDMACRO()
