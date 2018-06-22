#pragma once

bool ValidatePEImage64(void* image);
uint32_t FindKernel64Entry(const char* szFileName, char* buf, uint32_t& imageBase);