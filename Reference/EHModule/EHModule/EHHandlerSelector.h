#pragma once
#include "EHEnum.h"

void MiniDumpHandler(enumDumpDetectionLevel eLevel);
void BugTrapHandler(enumDumpDetectionLevel eLevel);
void UserDefinedHandler(enumDumpDetectionLevel eLevel);
void BugTrapMixHandler(enumDumpDetectionLevel eLevel);
