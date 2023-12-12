# Jala-Game
This is a framebuffer that I am developing for my Linux and Windows applications. It is entity oriented but it is far from being an ECS (entity-component systems)
______________________________________________
### Folders:
**Components:** Entity stuff<br>
* **Component.h:** It's just a base class that for now stores the type of component created.<br>
* **Physics.h:** It is a structure that contains everything related to physics, such as velocity, position and area of the entity.<br>
* **Properties.h:** size, alpha channel, width, height... etc.</sub><br>

**Framebuffer**<br>
* **Entity.h & Entity.cpp:** class that has a unique_ptr to the entity's data, also has the physics component and properties.<br>
* **EntityMngr.h & EntityMngr.cpp:** class that contains a map of Entities and knows how to render them using a reference to FrameBuffer
* **FrameBuffer.h & FrameBuffer.cpp:** class dedicated to drawing to a pixel buffer, knows how to draw Entities and CharSet objects.
* **CharSet.h & CharSet.cpp:** class which contains a vector of Entity* entities created for a specific string using a font loaded from FontLib.
* **FontLib:** minimal wrapper for freetype2 (Singleton)
* ## Device:
  * It is an interface of the device/OS, so far Windows or Linux, which must be implemented for each one, the implementations are inside the ./Linux and ./Windows folders
  * The generic class JalaGame uses this interface `class JalaGame : public draw::Device`
  * The idea is to use this interface to also implement Direct Draw or OpenGL.
* **Games/Pong**
  * The typical pong game that has helped me develop Framebuffer for Linux and Winodws (to control it: left side-> 'a' keys=up and 'q'=down, right side-> keys= up & down, space to shoot) 

__________________________________________________
### Dependencies
* **Linux**:
  * freetype2
  * X11
  * Xext (X Window System)
  * Xrandr
  * I've been using tinyPTC until I can develop my own X11 library.
* **Windows**
    * Visual Studio Community
  
