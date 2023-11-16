# add_subdirectory(funcoes)
# add_subdirectory(metodos)
# add_subdirectory(outputs)
# add_subdirectory(ext)

target_sources(${PROJECT_NAME} PRIVATE
    Color.cpp
    Vector3d.cpp
    Ray.cpp
    main.cpp
)