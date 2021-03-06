﻿cmake_minimum_required(VERSION 3.6)

find_package(Boost COMPONENTS system REQUIRED)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/core/interface/)
set(SOURCE_FILES
        interface/CommandsInterface.cpp
        interface/CommandsInterface.h
        
        commands/BaseUserCommand.cpp
        commands/BaseUserCommand.h

        commands/trust_lines/SetOutgoingTrustLineCommand.h
        commands/trust_lines/SetOutgoingTrustLineCommand.cpp
        commands/trust_lines/CloseIncomingTrustLineCommand.h
        commands/trust_lines/CloseIncomingTrustLineCommand.cpp

        commands/payments/CreditUsageCommand.cpp
        commands/payments/CreditUsageCommand.h

        commands/max_flow_calculation/InitiateMaxFlowCalculationCommand.h
        commands/max_flow_calculation/InitiateMaxFlowCalculationCommand.cpp
        commands/max_flow_calculation/InitiateMaxFlowCalculationFullyCommand.h
        commands/max_flow_calculation/InitiateMaxFlowCalculationFullyCommand.cpp

        commands/total_balances/TotalBalancesCommand.h
        commands/total_balances/TotalBalancesCommand.cpp

        commands/history/HistoryPaymentsCommand.h
        commands/history/HistoryPaymentsCommand.cpp
        commands/history/HistoryTrustLinesCommand.h
        commands/history/HistoryTrustLinesCommand.cpp
        commands/history/HistoryWithContractorCommand.h
        commands/history/HistoryWithContractorCommand.cpp
        commands/history/HistoryAdditionalPaymentsCommand.h
        commands/history/HistoryAdditionalPaymentsCommand.cpp

        commands/trust_lines_list/GetFirstLevelContractorsCommand.h
        commands/trust_lines_list/GetFirstLevelContractorsCommand.cpp

        commands/trust_lines_list/GetTrustLinesCommand.h
        commands/trust_lines_list/GetTrustLinesCommand.cpp

        commands/trust_lines_list/GetTrustLineCommand.h
        commands/trust_lines_list/GetTrustLineCommand.cpp

        commands/subsystems_controller/SubsystemsInfluenceCommand.h
        commands/subsystems_controller/SubsystemsInfluenceCommand.cpp
        commands/subsystems_controller/TrustLinesInfluenceCommand.h
        commands/subsystems_controller/TrustLinesInfluenceCommand.cpp

        commands/blacklist/AddNodeToBlackListCommand.h
        commands/blacklist/AddNodeToBlackListCommand.cpp

        commands/blacklist/RemoveNodeFromBlackListCommand.h
        commands/blacklist/RemoveNodeFromBlackListCommand.cpp

        commands/blacklist/GetBlackListCommand.h
        commands/blacklist/GetBlackListCommand.cpp

        commands/blacklist/CheckIfNodeInBlackListCommand.h
        commands/blacklist/CheckIfNodeInBlackListCommand.cpp

        commands/transactions/PaymentTransactionByCommandUUIDCommand.h
        commands/transactions/PaymentTransactionByCommandUUIDCommand.cpp

        commands/trust_lines_list/EquivalentListCommand.h
        commands/trust_lines_list/EquivalentListCommand.cpp commands/trust_lines/EthereumAuditCommand.cpp commands/trust_lines/EthereumAuditCommand.h commands/trust_lines/EthereumCloseChannelCommand.cpp commands/trust_lines/EthereumCloseChannelCommand.h commands/payments/CreditUsageMultyCommand.cpp commands/payments/CreditUsageMultyCommand.h)

add_library(interface__commands ${SOURCE_FILES})
target_link_libraries(interface__commands
        ${Boost_SYSTEM_LIBRARY}
        ${Boost_FILESYSTEM_LIBRARY}
        common)
