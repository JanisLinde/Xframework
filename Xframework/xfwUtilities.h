#pragma once

#define SAFERELEASE(x) if(x) { x->Release(); x = nullptr; }