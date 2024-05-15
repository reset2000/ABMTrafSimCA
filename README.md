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
# Installation
The application should be run through Qt Creator (version 10.0.0+). To launch the application, the user should click “Open Project” button and choose “CMakeLists.txt” file from the downloaded repository, then run the app using “Run” button (Ctrl + R shortcut on Windows).
# Authors
Patryk Górka<br>
gp46518@zut.edu.pl

Krzysztof Małecki<br>
krzysztof.malecki@zut.edu.pl
# Licenses
ABMTrafSimCA was created using GPL v.3.0 license (see <a href="./LICENSE">LICENSE.TXT</a>).
Qt library is also avaliable under the same <a href="https://doc.qt.io/qt-6/gpl.html">GPL version 3</a> license. 
Eigen package, which was used in this application, is avaliable under <a href="https://www.mozilla.org/en-US/MPL/2.0/">MPL2</a> license.
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
