cmake_minimum_required(VERSION 3.6)

set(SOURCE_FILES
        connector.h

        EthereumSiganturesManager.h
        EthereumSiganturesManager.cpp)

add_library(ethereum_components ${SOURCE_FILES})
#target_link_libraries(common )