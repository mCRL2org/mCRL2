#if defined(_WIN32)
#include <Windows.h>
#include "mcrl2/utilities/qt_tool.h"

namespace mcrl2
{
namespace utilities
{
namespace qt
{

void QtToolBase::attachConsole()
{
  BOOL success = AttachConsole(ATTACH_PARENT_PROCESS);
  if (success)
  {
    _cinbuf = std::cin.rdbuf();
    _console_cin.open("CONIN$");
    std::cin.rdbuf(_console_cin.rdbuf());
    _coutbuf = std::cout.rdbuf();
    _console_cout.open("CONOUT$");
    std::cout.rdbuf(_console_cout.rdbuf());
    _cerrbuf = std::cerr.rdbuf();
    _console_cerr.open("CONOUT$");
    std::cerr.rdbuf(_console_cerr.rdbuf());
  }
}

void QtToolBase::releaseConsole()
{
  _console_cout.close();
  std::cout.rdbuf(_coutbuf);
  _console_cin.close();
  std::cin.rdbuf(_cinbuf);
  _console_cerr.close();
  std::cerr.rdbuf(_cerrbuf);
  FreeConsole();
}

} // namespace qt
} // namespace utilities
} // namespace mcrl2

#endif // defined(_WIN32)
