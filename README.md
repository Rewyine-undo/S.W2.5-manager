
Last updated: August 9, 2025

# S.W 2.5 manager
 
 
This application allows you to manage Sword World 2.5 character data.
 Currently supports data from Rulebooks I-III.
 Does not support data from other supplements.
 
# DEMO
 
 
 There is no demo introduction.
 
 
# Features
 
 
 It automatically handles complicated ability values and experience points.
 It can also record accessories and draw positional relationships in advanced combat.
 
# Requirements
 
 
 wxWidgets-3.2.6
 nlohmann/json
 
 
# Installation
 <for mac/Linux>
 https://github.com/Rewyine-undo/S.W2.5-manager
 Clone this GitHub repository and build it based on build process.txt.
 
 <for Windows>
 Currently, the library and executable files are compiled and distributed, so if you are using Windows OS,
 you do not need to install or compile the library.
 Simply launch “S.W 2.5 manager_(update date).exe” (executable file) to use it.
 
 
# Usage
 
 
 <<When creating a new character>>
     1. Select the “Ability Creation” tab.
     2. Set the name (can be changed later), race, and birth ability values.
     3. Press the “Create” button.
     4. The character's ability values will be generated three times, so look at the displayed information and
        if you like one of the attempts, click the “Create New with These Ability Values” button to the right of that attempt.
        If you don't like any of the attempts, click the “Close” button.
     5. You will be automatically moved to the “Data Display” tab, so continue setting the character data.
 
 
<<To save the created character data (.json file)>>
     1. Select the “File” menu in the upper left corner.
     2. Select “Save File As.”
     3. Save the file to any directory.
 
     Alternatively, you can press Ctrl+Shift+S to move to the file save screen.


 <<To load the created character data (.json file)>>
     1. Select the “File” menu in the upper left corner.
     2. Select “Load File.”
     3. Select the file from any directory.
 
     Alternatively, you can press Ctrl+F to go to the file save screen.


 <<To overwrite and save character data (.json file)>>
     1. Select the “File” menu in the upper left corner.
     2. Select “Save Overwrite File.”
     
 
     Alternatively, you can also save over the file by pressing Ctrl+S.
     However, if you have not performed “Save File” or “Load File” before this operation,
     you will be automatically taken to the “Save File As” screen.

<<When managing advanced combat>>

     1. Select the “Advanced Combat (for GMs)” tab.

     2. Enter the names or identifiers of your characters in the text box on the left.

     3. Pieces will appear at the top of the screen, and their positions will be displayed in red to the right of the text box.

     4. Use the slider to manage the positions of the pieces.

     5. If there are multiple allied characters, press the “Add Ally” button in the center of the screen.

     6. A new text box will be generated for the new character, so enter the information in the same way.

 

     Perform the same operation for enemy characters to recreate the battle scene.

     If a character is defeated or otherwise leaves the battle, press the “Delete” button to the right of the slider to

     remove the piece from the number line at the top of the screen.







# Note

 

 

* We have not verified operation on Linux OS, so we do not guarantee operation in any environment.



 * If you attempt to save the file with non-numeric elements entered in boxes that are intended for numeric values only,

   an error may occur.



* Ability values that handle weapon data such as judgment packages are displayed by selecting the maximum value in the entry field,

   so the characteristics of weapons with negative corrections are not reflected correctly.

   Please keep this specification in mind when using weapons with negative corrections as your main weapons.



 * Regarding the ability value correction function for constant skills, please enter the evasion action rank using Roman numerals (I, II) and  

   the weapon proficiency rank using uppercase letters (A, S).  

   If other notation is used, the correction will not be properly reflected.



 * Going forward, we will be implementing the following elements on an irregular basis.

     ・ Addition of fields to describe selected techniques, spells & final rhythms, equestrian arts, and magical arts

     ・ Implementation of automatic application of race feature enhancement effects

     ・ Simultaneous display of multiple character data

     ・ Addition of character portrait display fields

 

 * List of confirmed bugs

     ・ When loading files created in the previous version into the current version, data in the “Items possessed, etc.” field disappears.

     ・ In the “Advanced Combat (GM-oriented)” tab, if you add an enemy input field while the ally input field count is 0, the enemy input field moves to the

         left side of the screen.

 

We will continue to fix bugs on an irregular basis.

 

 

 

# Author

 

oktabrkrasnyj0@gmail.com

 

# License



“S.W 2.5 manager” is licensed under the [MIT license].





This software uses the following third-party libraries:

- wxWidgets 3.2.6 (wxWindows Library License)

  https://www.wxwidgets.org/about/licence/

- nlohmann/json (MIT License)

  https://github.com/nlohmann/json#license


