cmake_minimum_required(VERSION 3.6)

set(SOURCE_FILES
        PaymentExchangeHandler.h
        PaymentExchangeHandler.cpp)


add_library(exchange ${SOURCE_FILES})
target_link_libraries(exchange
        common)