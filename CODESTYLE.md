# Code Style

## Naming conventions

- File names: PascalCase
- Folder names: PascalCase

- Class names: PascalCase
- Struct names: PascalCase
- Class private and protected members: _camelCase
- Class public members: mCamelCase

- Functions: camelCase
- Local variables: camelCase

- Namespaces: camelCase

- Constants: SCREAMING_SNAKE_CASE

## Class Style

```C++
class Foo_Bar
{
private:
    int _number;

    void fooBar();

protected:
    int _ohterNumber;

    void barFoo();

public:
    int mPublicNumber;

    Foo_Bar();

    void foo();
    void bar();
};
```

- Private members at top, protected in middle, public at bottom
- Private functions at top, protected in middle, public at bottom
- Notice one spaces between each section
- For primitive class members may be no prefix "m" (e.g. coordinates) 

- Initilzation lists:

```C++
Foo::Foo(int x, int y)
    : x(x)
    , y(y) 
{
}
```

## Namespaces

- NO `using namespace std;` Your pull request ***will*** be denied if that is included
- Namespaces should be camelCase
- Notice a space between declaration namespace and code
- Nested namespaces:

```C++
namespace foo
{

namespace bar
{

void codeGoesHere()
{

}

}

}
```

## Includes and Forward Declares order:
- Standard Library Headers for 
- Other includes
- .hpp/.h file corresponding to this .cpp file (if applicable)
- Forward declarations
- Notice two spaces between includes and code

- Example:
```C++
#include <vector>
#include <memory>
#include <thread>
#include <mutex>

#include <SFML/System/Clock.hpp>

#include "../Physics/AABB.hpp"

#include "Chunk/Section.hpp"
#include "Chunk/Map.hpp"
#include "WorldSettings.hpp"


namespace renderer 
{  

class Master;

}

class Player;
class Camera;

class World
{
//etc..
```

## Constants
- Do not use C-Style "defines" for constants.
- Use constexpr instead. It is compile-time determined just like #define is.
- Functions can be marked as "constexpr" as well.

## Functions
- Function names should be descriptive of what they do and should be a verb ("doing word")
- Functions which return "bool" should be a question e.g. 

```C++
bool isXGreaterThan5(int x)
{
    return x > 5;
}
```

- Primitives can be passed by value, or reference
- Objects pass as either const reference (or reference), and NEVER BY VALUE
- Indent style:

```C++
bool isSomething(int arg1, const std::string& arg2)
{
    if (arg1 > 5)
    {
        std::cout << arg2 << '\n';
        return true;
    }
    return false;
}
```

- For setter functions, use R-Value references and move scematics (if feasible) e.g.

```C++
void Foo:setString(std::string&& str)
{
    m_string = std::move(str);
}
```

## Slash
- Don't use the `\`, it can cause errors on Linux. Correct use:
```C++
#include <SFML/Graphics.hpp>
```

- This goes for strings as file paths too!
```C++
    std::string filePath = "forward/slashes/so/it/works/cross/platform.png"
```

## Pointers
- Please prefer smart pointers to raw owning pointers as possible
```C++
int* x = new int(5); // No!
auto y = std::make_unique<int>(5) // Yes!
```

- If you have to use "new", then you are probably doing something wrong
- Only case raw pointers are fine is if they are a pointer to a variable (So a non-owning pointer)

## Enums
- Use enum class, not regular enums!

## Comments
- Prefer one-line comment "//" then multiline "/**/"
- Write comments only in English
- Each comment should end with the period (".")
- Notice an indentation between "//" and beginning of the comment
- No words on open and close line multiline comment (doesn't work with code blocks comments)!
- It's prefered to write the comments in places hard to understand
- Please use docs comments for functions, class-methods and class-members
```C++
class World
{
private:
    /**
     * \brief The variable to count before we can destroy dead body.
     */
    int         _Counter;
    
    /**
     * \brief The variable that contains level name.
     */
    std::string _levelName;
    
public:
    /**
     * \brief		        Default explicit constructor.
     * \param[out] window	Active window for rendering all stuff.
     * \param[in] textures      Textures holder for extracting necessary textures.
     * \param[in] fonts		Fonts holder for extracting necessary textures.
     * \param[in] sounds	Sounds holder for extracting necessary textures.
     * \param[in] playerInfo    Class that contains player's data.
     * \param[out] audioManager	Audio manager for manipulating app's music.
     */
    explicit				World(sf::RenderWindow& window, TextureHolder& textures,
						FontHolder& fonts, SoundBufferHolder& sounds, 
						PlayerInfo playerInfo, AudioManager& audioManager);
...
}

...
// Search next component that is selectable, wrap around if necessary.
int next = mSelectedChild;
do
{
  next = (next + 1) % mChildren.size();
}
while (!mChildren[next]->isSelectable());

// Select that component.
select(next);
...    
```

## Word wrapping
- Maximum lenght of line = 100 characters (OPTIONAL)
- It's prefered to word wrapping like:
```C++
void foo(// Many args..
         // continuation of args)
{
  int temp = doSomething(arg1, arg2, ... ,
                         ... argN);
  if (condition1 && cindition2 &&
    conditionN)
  {
  }
  
  int m = arg1 + agr2 + ... +
          + ... + argN;
}
```

## Curly brackets
- Prefer to place the brackets everywhere
```C++
void foo()
{
  int temp = doSomething();
  if (condition1)
  {
    // Do smth.
  }
  else
  {
    return;
  }
  
}
```
