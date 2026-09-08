cmake_minimum_required(VERSION 3.16)
project(SimulationBank CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(WIN32 AND EXISTS "C:/Qt/6.11.2/mingw_64")
    list(APPEND CMAKE_PREFIX_PATH "C:/Qt/6.11.2/mingw_64")
endif()

add_library(simulation_core STATIC
    src/client/AbstractOperation.cpp
    src/client/AbstractClient.cpp
    src/simulation/Cashier.cpp
    src/simulation/Queue.cpp
    src/simulation/Bank.cpp
    src/simulation/StatisticManager.cpp
    src/simulation/SimulationUtility.cpp
    src/simulation/Simulation.cpp
)
target_include_directories(simulation_core PUBLIC include)

add_executable(simulation_console src/main_console.cpp)
target_link_libraries(simulation_console PRIVATE simulation_core)

# Qt-based targets (statistics persistence/charts and the real-time GUI).
# Only built when Qt6 is found, so teammates without it installed yet can
# still build and run simulation_console.
find_package(Qt6 QUIET COMPONENTS Widgets Charts Sql)
if(Qt6_FOUND)
    message(STATUS "Qt6 found: building persistence, chart and GUI targets.")
    set(CMAKE_AUTOMOC ON)

    add_library(simulation_persistence STATIC
        src/persistence/StatisticRepository.cpp
    )
    target_include_directories(simulation_persistence PUBLIC include)
    target_link_libraries(simulation_persistence PUBLIC simulation_core Qt6::Core Qt6::Sql)

    add_executable(simulation_db_test src/main_db_test.cpp)
    target_link_libraries(simulation_db_test PRIVATE simulation_persistence)

    add_executable(simulation_tests src/tests/spec_tests.cpp)
    target_link_libraries(simulation_tests PRIVATE simulation_persistence)

    qt6_wrap_cpp(SIMULATION_GUI_MOC_SOURCES
        include/gui/BankView.h
        include/gui/MainWindow.h
    )

    add_executable(simulation_gui
        src/main_gui.cpp
        src/gui/BankView.cpp
        src/gui/ResultsChartView.cpp
        src/gui/MainWindow.cpp
        ${SIMULATION_GUI_MOC_SOURCES}
    )
    target_include_directories(simulation_gui PRIVATE include)
    target_link_libraries(simulation_gui PRIVATE simulation_persistence Qt6::Widgets Qt6::Charts)
else()
    message(STATUS "Qt6 not found: only simulation_console will be built.")
endif()
