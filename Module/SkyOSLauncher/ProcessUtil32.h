#pragma once

bool ValidatePEImage(void* image);
uint32_t FindKernel32Entry(const char* szFileName, char* buf, uint32_t& imageBase);