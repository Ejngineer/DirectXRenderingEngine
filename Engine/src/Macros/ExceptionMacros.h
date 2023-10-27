#pragma once

#define ENGINE_HREXCEPT(hr) ExceptionManager::ExceptionManager(__LINE__,__FILE__,(hr))
#define ENGINE_LASTEXCEPT() ExceptionManager::ExceptionManager(__LINE__,__FILE__, GetLastError())