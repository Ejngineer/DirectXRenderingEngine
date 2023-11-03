#pragma once

#define ENGINE_HREXCEPT_NO_INFO(hrcall) if(FAILED(hr = (hrcall))) Graphics::Exception(__LINE__, __FILE__, hr);
#define ENGINE_HREXCEPT_INFO(hrcall) InfoManager.set(); if(FAILED(hr = (hrcall))) throw ENGINE_GFX_EXCEPT(hr);
#define ENGINE_HREXCEPT_INFO_ONLY(call) InfoManager.set(); (call); {auto v = InfoManager.getMessages(); if(!v.empty()){throw Graphics::InfoException::InfoException(__LINE__,__FILE__, v);}}
#define ENGINE_GFX_EXCEPT(hr) Graphics::Exception::Exception(__LINE__, __FILE__, hr, InfoManager.getMessages());
#define ENGINE_EXCEPT() ExceptionManager::ExceptionManager(__LINE__,__FILE__)
#define ENGINE_LASTEXCEPT() Window::WindowException(__LINE__,__FILE__, GetLastError())