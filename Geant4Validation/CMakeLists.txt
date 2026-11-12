cmake_minimum_required(VERSION 3.16)

project(Geant4Validation)

# Find Geant4 installation
find_package(Geant4 11.4.2 REQUIRED)

# Include Geant4 configuration
include(${Geant4_USE_FILE})

# Include project headers
include_directories(${PROJECT_SOURCE_DIR}/include)

# Source files
file(GLOB sources
    ${PROJECT_SOURCE_DIR}/src/*.cc
)

# Build executable
add_executable(Geant4Validation
    main.cc
    ${sources}
)

# Link Geant4 libraries
target_link_libraries(Geant4Validation
    ${Geant4_LIBRARIES}
)