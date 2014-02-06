add_mcrl2_tool(pbespgsolve
  SOURCES
    Abortable.cpp
    ComponentSolver.cpp
    DecycleSolver.cpp
    DeloopSolver.cpp
    FocusListLiftingStrategy.cpp
    Graph.cpp
    LiftingStrategy.cpp
    LinearLiftingStrategy.cpp
    MaxMeasureLiftingStrategy.cpp
    OldMaxMeasureLiftingStrategy.cpp
    ParityGame.cpp
    ParityGame_IO.cpp
    ParityGame_verify.cpp
    ParityGameSolver.cpp
    pbespgsolve.cpp
    PredecessorLiftingStrategy.cpp
    RecursiveSolver.cpp
    SmallProgressMeasures.cpp
  DEPENDS
    mcrl2_atermpp   HEADER_ONLY
    mcrl2_bes       HEADER_ONLY
    mcrl2_core      HEADER_ONLY
    mcrl2_data      HEADER_ONLY
    mcrl2_pbes
    mcrl2_utilities HEADER_ONLY
)
