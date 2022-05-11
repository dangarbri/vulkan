#ifndef _WINDOW_H_
#define _WINDOW_H_

/**
 * Encapsulates a subset of GLFW window handling
 */

class Window
{
 public:
  /**
   * Creates a window with the given TITLE
   */
  Window(const char* title);
  ~Window();

  GLFWwindow* GetWindow();

 private:
  struct impl;
  struct impl* _impl;
};

#endif
