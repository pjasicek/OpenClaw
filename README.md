# Captain Claw remiplementation

 - This project is a multiplatform C++ reimplementation of original Captain Claw (1997) platformer game
 - Whole codebase was written from scratch
 - Game uses assets from original game archive (CLAW.REZ)

# Tech

  - SDL2 Libraries (SDL2, SDL_Image, SDL_TTF, SDL_Mixer, SDL2_Gfx) for graphics, input, font and audio
  - Box2D Library for Physics
  - Tinyxml library for data-driven approach

# Building and running
For all platforms you will need original CLAW.REZ game archive from original game
### Windows
  - Project contains VS2013 solution with all libraries and include directories preset

### Linux
  **Prerequisites for Ubuntu 16.04 (should be almost identical for Fedora/CentOS/*):**
  
  `sudo apt install libsdl2-dev  libsdl2-image-dev libsdl2-mixer-dev libsdl2-ttf-dev libsdl2-gfx-dev libtinyxml-dev` 
  
  **Compilation:**
  
  - git clone https://github.com/pjasicek/CaptainClaw.git
  - cd CaptainClaw
  - mkdir build
  - cd build
  - cmake ..
  - make -j4
  - cd ../Build_Release
  - ./captainclaw (Considering you have CLAW.REZ from original game in the same folder)
  
### Android
  
  - Compilation / Deployment steps will be updated in near future, game itself is successfully running on Android
 
# In-game images
![alt tag](http://s2.postimg.org/gwdm0ii7t/screenshot_26.png)
![alt tag](http://s2.postimg.org/yaxu8sfcp/screenshot_27.png)
![alt tag](http://s2.postimg.org/7r596net5/screenshot_28.png)
![alt tag](http://s2.postimg.org/az9qjp12x/screenshot_29.png)
