NordenBombsight Addon
Created for the National Museum of World War II Aviation
Created by the Bombgineers of the University of Colorado, Colorado Springs
---------------------------------------------------------------------------
--------------------------------Bombgineers--------------------------------
---------------------------------------------------------------------------
Project Lead: Dylan McWharter
Hardware Lead: Jadon Wilson
Software Lead: Zack Dunn
Systems Lead: Haider Khan
---------------------------------------------------------------------------
---------------------------Operation Instructions--------------------------
---------------------------------------------------------------------------
1. Start Prepar3d.
2. Select Norden Sim 1 scenario via load button.
3. Pause the Simulation using "P" when it finishing loading.
4. Set Optional Camera Views using Views->New View ->Outside ->FlyBy
IMPORTANT NOTE: If the NordenCamera is not the main view, the addon crashes. Unsure why.
5. Start NordenBombsight.exe or via Visual Studio.
6. Addon will search for Arduino over serial connection.
7. Plug in Arduino(The COM port is set in the code to be COM5).
8. Addon will search for and connect to Prepar3d.
9. Addon will enter main loop.
10. Prepar3d starts with parking break on, press "." to turn this off.(Can be done while paused)
IMPORTANT NOTE: Addon will move NordenCamera while paused if dials are moved.
11. Unpause. Addon will force plane to autopilot. Values for this can be found in the Source/NordenBombsight/MainFile.cpp
---------------------------------------------------------------------------
-----------------------------Visual Studio Notes---------------------------
---------------------------------------------------------------------------
1. Ensure Prepar3d and the SDK are the same version, both major (v5), and minor (5.1.8.26742 currently).
   There is no debuging that specifically states this is the issue in Prepar3d.
   Debuging for Prepar3d can be initialized using SimConnect.ini file
   Documentation: https://www.prepar3d.com/SDKv5/sdk/simconnect_api/configuration_files/configuration_files_overview.html

2. Setting up the project can be tricky at first. The project properties should be set correctly, but may need to be
   changed if moving computers. Use: https://www.prepar3d.com/SDKv5/sdk/simconnect_api/c_simconnect_project_tutorial.html
   Note: The default samples in the SDK did not set the included directories or library correctly.

3. For some reason, the SimConnect Library does not like the debug configuation with this addon, use release instead.
   The newly built executatble(NordenBombsight.exe) can be found in /Source/NordenBombsight/x64/Release

4. The BrakesEvent.cpp and TestSerial.cpp were used as test files for library functions. They are set to be excluded from
   the project, but are left in the files for reference.
---------------------------------------------------------------------------
----------------------------Useful Documentation---------------------------
---------------------------------------------------------------------------
Prepar3d Simconnect Sample Codes: https://www.prepar3d.com/SDKv5/sdk/simconnect_api/samples/samples_overview.html

General Prepar3d Documentation: http://www.prepar3d.com/SDKv5/LearningCenter.php

Blender2P3DFSX Documentation: https://www.fsdeveloper.com/wiki/index.php?title=Blender2P3D/FSX

Useful Training Videos: https://www.youtube.com/watch?v=Rbtmb_JrX2g&ab_channel=PeriscopeFilm
                        https://www.youtube.com/watch?v=143vi97a4tY&ab_channel=PeriscopeFilm

---------------------------------------------------------------------------
-----------------------------Unfinished Sections---------------------------
---------------------------------------------------------------------------
1. Bomb and Target asset modeling
2. Bomb drop follow camera. Some forum references think this is impossible.
   Another addon found a workaround that made the bomb an 'aircarft' type.
   This would require an new, more complex sim.cfg file, but would allow for a camera to be added in said file.
3. Scoreboard/Distance from Target Calculations
4. Heading Control. The physical Norden Bombsight Replica does not currently have controls for this feature.
   When this feature is added, the planeHeading value in the PlaneData struct of the MainFile.cpp can be adjusted in
   the //Autopilot Values section.
5. Norden Bombsight Replica moving arm logic
6. When creating more scenarios, use Norden Sim 1 as an example. There is a wierd engine startup issue that was solved by changing
   the 4 Engine Parameters in the SimDirector. These are found at File ->Scenario ->Properties ->Additional ->Engine Parameters x.0 ->Pct Engine RPM.
   A a value of 0.59 seems to work.
   Throttle Percent is being set in the addon code, so it is unclear if this would still be an issue in a new scenario. If engines stall on scenario start,
   this is a good place to start looking. SimDirector is a bit of a pain to work with, it is recomended to use the C++ code as much as possible.

