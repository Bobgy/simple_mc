simple_mc  
=========  
  
Write a simple version of Minecraft as the project for Computer Graphics  
  
=========  
Control manual  
  
---------  
Cursor is automatically hidden.  
Mouse is used for changing your view direction.  
Chosen block is contained by black wire cube.  
Left click when choosing a block removes it if you can.  
Right click places a block(type is chosen by 0-9, see keyboard part) ajacent to the chosen block.  
  
---------  
The game has two modes, creative and survival, keys may have different functionality in different modes.  
"F2" toggles between these modes.  
  
General usage:  
"WASD": Move forward, backward, left, right.  
" "(space): Jump.  
"q": Leave the game.  
"o": Toggle between wire model or solid model. The default is solid model.  
"/": Save current stage to the file "stage/last_save.txt"  
"0-9": Choose the type of block you are going to place.  
"F5": Toggles view mode among FIRST_PERSON, THIRD_PERSON_BACK, THIRD_PERSON_FRONT. (default, FIRST_PERSON)  
"F6": Toggles whether the player ("Steve"), is shown. (default, not shown)  
"F7": Toggles whether sun light is moving. (default, not moving)  
"F9": Toggles whether the wire cube coordinates are shown near the player. (default, not shown)  
"F10": Toggles whether the shadow depth map is shown on the upper right corner of the screen. (default, not shown)  
"F11": Toggles fullscreen.  
"F12": Make a screenshot. (saved to the game's root directory)  
  
Creative mode:  
In creative mode, gravity for the player is turned off, you can use " "(space) to fly up.  
Conversely, use the key "LSHIFT" to fall down.  
  
There are some restrictions of the type of blocks you can destroy, while in creative mode, you can destroy any block.  
