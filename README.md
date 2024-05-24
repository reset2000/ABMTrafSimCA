# ABMTrafSimCA
ABMTrafSimCA is a desktop application written in C++ with a user interface implemented with the Qt library. It contains all the basic functions which are necessary to carry out traffic modelling and simulation based on the CA model. The developed software enables users to adapt the system to their needs freely and allows the observation of the traffic phenomenon through two types of diagrams (space-time and fundamental diagrams). Such diagrams are the most expected by researchers dealing with the indicated topics. The implemented model serves a straight, two-lane road with a determined density of vehicles and periodic boundary conditions. Thanks to that, a constant percentage of occupied CA cells during the simulation is maintained.
# Features
- Simulating traffic based on parameters set by the user.
- Creating agents (driver profiles) with certain simulation properties.
- Creating sets of parameters for road such as length, number of cycles, diagram parameters etc.
- Customisation of desirable car lengths to achieve more realistic simulations.
- Implementation of a traffic model enabling realistic acceleration and braking of vehicles, including a safe distance and emergency braking.
- Space-time diagrams allow users to see the results of a single simulation iteration with a specific density for each lane separately.
- Fundamental diagrams allow users to see the flow of cars for set densities based on data averaged from multiple simulations.
- Storage of generated diagrams and sets of parameters in the database for later use.
- Customisation of display styles for space-time and fundamental diagrams.
# Requirements
- Windows or Linux 64-bit operating system
- C++ 17+
- Qt 6.5.0+
- Qt Creator 10.0.0+
# How to run the application
The application should be run through Qt Creator (version 10.0.0+). The software can be downloaded from the official Qt Group website avaliable under <a href="https://doc.qt.io/qt-6/gpl.html" target="_blank">THIS</a> adress.

<img src="https://github.com/reset2000/ABMTrafSimCA/assets/59997339/5a3ba80c-5eac-41c4-93a2-35362a20230a" width="70%" alt="image" style="display: block; margin-left: auto; margin-right: auto;" />

The installation process should be followed as intructed by the Qt Creator installer directions.

Before you can launch the application, you need to download this repository from Github:

<img src="https://github.com/reset2000/ABMTrafSimCA/assets/59997339/ffca1f9d-c7c8-43f1-8694-b258903970dd" width="70%" alt="image" style="display: block; margin-left: auto; margin-right: auto;" />

After this, it is necessary to unpack the .zip file.

To launch the application, the user should click “Open Project” button in Qt Creator.

<img src="https://github.com/reset2000/ABMTrafSimCA/assets/59997339/edf78059-cfa1-464a-a5b7-66afeb59aa6b" width="70%" alt="image" style="display: block; margin-left: auto; margin-right: auto;" />

Then choose “CMakeLists.txt” file from the downloaded and unpacked repository.

<img src="https://github.com/reset2000/ABMTrafSimCA/assets/59997339/b711b48c-fade-4114-8a32-0696a58ab5b5" width="70%" alt="image" style="display: block; margin-left: auto; margin-right: auto;" />

After opening the project, the user should run the application using “Run” button (or Ctrl + R shortcut on Windows).

<img src="https://github.com/reset2000/ABMTrafSimCA/assets/59997339/a8b0a613-5826-48dc-9242-d8e7bc136692" width="70%" alt="image" style="display: block; margin-left: auto; margin-right: auto;" />

After running the programm through Qt Creator, the main window of the application should appear.

<img src="https://github.com/reset2000/ABMTrafSimCA/assets/59997339/f2adbac3-7b39-4620-a642-e77944db47c0" width="70%" alt="image" style="display: block; margin-left: auto; margin-right: auto;" />

# Authors
Patryk Górka<br>
gp46518@zut.edu.pl

Krzysztof Małecki<br>
krzysztof.malecki@zut.edu.pl
# Licenses
ABMTrafSimCA was created using GPL v.3.0 license (see <a href="./LICENSE" target="_blank">LICENSE.TXT</a>).
Qt library is also avaliable under the same <a href="https://doc.qt.io/qt-6/gpl.html" target="_blank">GPL version 3</a> license. 
Eigen package, which was used in this application, is avaliable under <a href="https://www.mozilla.org/en-US/MPL/2.0/" target="_blank">MPL2</a> license.
# Version history
- 1.0.0 – implementation of console-based traffic simulator
  - 2.0.0 – first version of basic UI created in Qt
  - 2.1.0 – database and snapshot creation before closing program
    - 2.1.1 – loading snapshots from database
  - 2.2.0 – generating space-time and fundamental diagrams
  - 2.3.0 – added multithreading to shorten simulation time
  - 2.4.0 – saving and loading driver types using database
  - 2.5.0 – added different diagram styles and fonts
    - 2.5.1 – fixed colour being displayed incorrectly depending on the motive
  - 2.6.0 – updated project with the addition of the documentation in the "Documentation" folder